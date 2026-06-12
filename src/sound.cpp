#include "sound.h"

#include "func_ids.h"
#include "riff.h"

#include "loader/yyc.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>

enum buffer_type
{
	buffer_fixed,
	buffer_grow,
	buffer_wrap,
	buffer_fast
};

enum buffer_format
{
	buffer_null,
	buffer_u8,
	buffer_s8,
	buffer_u16,
	buffer_s16,
	buffer_u32,
	buffer_s32,
	buffer_u64,
	buffer_f16,
	buffer_f32,
	buffer_f64,
	buffer_bool,
	buffer_string,
	buffer_text,
};

enum e_sound_data_type
{
	SOUND_STREAM = 0,
	SOUND_BUFFER = 1
};

struct ref_sound_asset
{
	uint8_t type;
	RValue* ref;
	RValue* buffer_ref = nullptr; //< set to nullptr if not a buffer asset
};

std::unordered_map<double, ref_sound_asset> loaded_sound;

//
//	args
//	audio_play_sound(index, priority, loop, [gain], [offset], [pitch], [listener_mask]);
//
PFUNC_YYGMLScript audio_play_sound_original;
PFUNC_YYGMLScript audio_play_sound_at_original;
void audio_play_sound_detour(GMLInstance* self, GMLInstance* other, RValue& out, uint32_t arg_count, RValue* args)
{
	if (loaded_sound.size() > 0)
	{
		if (args[0].type == GML_TYPE_REAL)
		{
			if (loaded_sound.contains(args[0].valueReal))
			{
				args[0].valueReal = loaded_sound[args[0].valueReal].ref->valueReal;
			}
		}
		else
		{
			loader_log_warn("audio_play_sound_detour(): index->type was not GML_TYPE_REAL");
		}
	}

	audio_play_sound_original(self, other, out, arg_count, args);
}

void __setup_sound()
{
	uint32_t audio_play_sound = (uint32_t)loader_get_yyc_func_ptr("audio_play_sound");
	uint32_t audio_play_sound_at = (uint32_t)loader_get_yyc_func_ptr("audio_play_sound_at");

	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound),
		&audio_play_sound_detour, 
		reinterpret_cast<void**>(&audio_play_sound_original)
	);

	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound_at),
		&audio_play_sound_detour,
		reinterpret_cast<void**>(&audio_play_sound_at_original)
	);

	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound));
	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound_at));
}

void destroy_sounds()
{
	RValue cur_id;
	for (auto& [_id, _sound_asset] : loaded_sound)
	{
		cur_id.type = GML_TYPE_REAL;
		cur_id.valueReal = _id;

		RValue* destroy_args[] = { &cur_id };


		RValue* result = 0;
		switch (_sound_asset.type)
		{
		case SOUND_STREAM:
			result = loader_yyc_call_func(audio_destroy_stream, 1, destroy_args);

		case SOUND_BUFFER:
			if (_sound_asset.ref->valueReal == -1)
				break;

			result = loader_yyc_call_func("audio_free_buffer_sound", 1, destroy_args);

			if (_sound_asset.buffer_ref)
			{
				RValue* delete_args[] = { _sound_asset.buffer_ref };
				loader_yyc_call_func("buffer_delete", 1, delete_args);
				delete _sound_asset.buffer_ref;
			}
			break;
		}

		if (result->valueReal == 1)
		{
			// successfully destroyed audio stream
			delete _sound_asset.ref;
			loaded_sound.erase(_id);
		}
		else
		{
			loader_log_error("destroy_sound(): error occured trying to delete sound asset with id \"{}\" (0x{:08x})", _id, (void*)_sound_asset.ref);
		}
	}
}

RValue* buffer_create(int size, buffer_type type, int alignment)
{
	RValue _rv_size = RValue(size);
	RValue _rv_type = RValue(type);
	RValue _rv_alignment = RValue(alignment);
	RValue* _rv_args[] = { &_rv_size, &_rv_type, &_rv_alignment };
	return loader_yyc_call_func("buffer_create", 3, _rv_args);
}

RValue* buffer_get_address(RValue* buffer)
{
	RValue* _rv_args[] = { buffer };
	return loader_yyc_call_func("buffer_get_address", 1, _rv_args);
}

RValue* buffer_load_partial(RValue* buffer, const char* filename, int offset, int src_len, int dest_offset)
{
	RValue _rv_filename = RValue(filename);
	RValue _rv_offset = RValue(offset);
	RValue _rv_src_len = RValue(src_len);
	RValue _rv_dest_offset = RValue(dest_offset);
	RValue* _rv_args[] = { buffer, &_rv_filename, &_rv_offset, &_rv_src_len, &_rv_dest_offset };
	return loader_yyc_call_func("buffer_load_partial", 5, _rv_args);
}

RValue* buffer_fill(RValue* buffer, int offset, int type, int value, int size)
{
	RValue _rv_offset = RValue(offset);
	RValue _rv_type = RValue(type);
	RValue _rv_value = RValue(value);
	RValue _rv_size = RValue(size);
	RValue* _rv_args[] = { buffer, &_rv_size, &_rv_type, &_rv_value, &_rv_size };
	return loader_yyc_call_func("buffer_fill", 5, _rv_args);
}

RValue* load_vorbis(const std::filesystem::path& entry)
{
	std::string file_stem = entry.stem().generic_string();
	std::string file_path = entry.generic_string();

	RValue sound_name = RValue(file_stem);
	RValue* args[] = { &sound_name };
	RValue* sound_id = loader_yyc_call_func(asset_get_index, 1, args);

	args[0] = sound_id;
	if (loader_yyc_call_func(audio_exists, 1, args)->truthy())
	{
		RValue sound_filename = RValue(file_path);

		args[0] = &sound_filename;
		RValue* new_sound_asset = loader_yyc_call_func(audio_create_stream, 1, args);

		loaded_sound[(uint32_t)sound_id->valueReal] = { SOUND_STREAM, new_sound_asset };
		return new_sound_asset;
	}
	return nullptr;
}

RValue* load_waveform(const std::filesystem::path& entry)
{
	std::string file_stem = entry.stem().generic_string();
	std::string file_path = entry.generic_string();

	RValue sound_name = RValue(file_stem);
	RValue* args[] = { &sound_name };
	RValue* sound_id = loader_yyc_call_func(asset_get_index, 1, args);

	args[0] = sound_id;
	if (loader_yyc_call_func(audio_exists, 1, args)->truthy())
	{
		riff* waveform_data = read_riff(file_path);

		if (waveform_data->format_block.audio_format != 1)
		{
			loader_log_error("load_waveform(): error while attempting to load waveform \"{}\"", file_path);
			loader_log_error("\t^~~~~ audio format type was not PCM");
			return nullptr;
		}

		if (waveform_data->format_block.bit_depth != 8 && waveform_data->format_block.bit_depth != 16)
		{
			loader_log_error("load_waveform(): error while attempting to load waveform \"{}\"", file_path);
			loader_log_error("\t^~~~~ bit depth was {} (expected 8 or 16 bits)", waveform_data->format_block.bit_depth);
			return nullptr;
		}

		const uint32_t data_size = waveform_data->file_size;

		RValue* new_buffer = buffer_create(data_size, buffer_wrap, 1);
		if (new_buffer->valueReal < 0)
		{
			std::string err_string = "load_waveform(): error creating buffer; ";
			switch (int(new_buffer->valueReal))
			{
				case -1:
					err_string.append("buffer_error_general");
					break;

				case -2:
					err_string.append("buffer_error_out_of_space");
					break;

				case -4:
					err_string.append("buffer_error_invalid_type");
					break;
			}
			loader_log_error(err_string);
			return nullptr;
		}

		buffer_fill(new_buffer, 0, buffer_u8, 24, data_size);
		RValue* buffer_addr = buffer_get_address(new_buffer);
		memcpy(buffer_addr->valuePointer, waveform_data->data, data_size);
		
		enum
		{
			audio_mono,
			audio_stereo,
			audio_3d
		};

		RValue buff_fmt = waveform_data->format_block.bit_depth == 16 ? buffer_s16 : buffer_u8;
		RValue buff_sample_rate = int(waveform_data->format_block.sample_speed);
		RValue buff_offset = int(waveform_data->data_offset);
		RValue buff_length = int(waveform_data->file_size - waveform_data->data_offset);
		RValue buff_channels;

		if (waveform_data->format_block.channel_count > 2)
			buff_channels = int(audio_3d);
		else
			//buff_channels = int(audio_3d);
			buff_channels = int(waveform_data->format_block.channel_count == 1 ? audio_mono : audio_stereo);

		RValue* _rv_create_buffer_args[] = 
		{ 
			new_buffer,
			&buff_fmt,
			&buff_sample_rate,
			&buff_offset,
			&buff_length,
			&buff_channels,
		};

		RValue* new_sound_asset = loader_yyc_call_func(audio_create_buffer_sound, 6, _rv_create_buffer_args);
		
		waveform_data->free();
		delete waveform_data;

		if (new_sound_asset->valueReal == -1)
		{
			loader_log_error("load_waveform(): error while attempting to load waveform \"{}\"", file_path);
			return nullptr;
		}

		loaded_sound[(uint32_t)sound_id->valueReal] = { SOUND_BUFFER, new_sound_asset, new_buffer };

		return new_sound_asset;
	}
	return nullptr;
}

void load_sound(const std::filesystem::path& entry)
{
	if (!ready)
	{
		__setup_funcids();
	}

	if (entry.extension() == ".ogg")
	{
		load_vorbis(entry);
		return;
	}

	if (entry.extension() == ".wav")
	{
		load_waveform(entry);
		return;
	}

	// set the loop points to 0
	// nvm rivals uses an old ass gamemaker version that doesnt have these functions
// 	if (loader_yyc_call_func(audio_sound_get_loop, 1, args))
// 	{
// 		RValue zero = 0;
// 		RValue* loop_args[] = { sound_id, &zero };
// 		loader_yyc_call_func(audio_sound_loop_start, 2, loop_args);
// 		loader_yyc_call_func(audio_sound_loop_end, 2, loop_args);
// 	}
}