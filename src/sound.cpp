#include "sound.h"

#include "riff.h"
#include "pack.h"

#include "loader/yyc.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <memory>

enum SoundDataType
{
	SOUND_STREAM = 0,
	SOUND_BUFFER = 1
};

struct SoundAssetReference
{
	uint8_t type;
	std::shared_ptr<RValue> ref;
	std::shared_ptr<RValue> buffer_ref = nullptr; //< set to nullptr if not a buffer asset
};

std::unordered_map<double, SoundAssetReference> loaded_sound;

void try_overwrite_sound_id(RValue* sound_id)
{
	if (loaded_sound.contains(sound_id->get_s32()))
	{
		const SoundAssetReference& overwriting_sound = loaded_sound[sound_id->get_s32()];

		switch (overwriting_sound.type)
		{
		case SOUND_STREAM:
			if (overwriting_sound.ref)
			{
				sound_id->set_real(overwriting_sound.ref->get_real());
			}
			break;

		case SOUND_BUFFER:
			if (overwriting_sound.buffer_ref)
			{
				sound_id->set_real(overwriting_sound.buffer_ref->get_real());
			}
			break;
		}
	}
}

PFUNC_YYInternal audio_play_sound_original;
PFUNC_YYInternal audio_play_sound_at_original;
PFUNC_YYInternal audio_play_sound_on_original;
void audio_play_sound_detour(CInstance* self, CInstance* other, RValue* out, uint32_t arg_count, RValue* args)
{
	RValue* sound_id = &args[0];
	try_overwrite_sound_id(sound_id);
	audio_play_sound_original(self, other, out, arg_count, args);
}

void audio_play_sound_at_detour(CInstance* self, CInstance* other, RValue* out, uint32_t arg_count, RValue* args)
{
	RValue* sound_id = &args[0];
	try_overwrite_sound_id(sound_id);
	audio_play_sound_at_original(self, other, out, arg_count, args);
}

void audio_play_sound_on_detour(CInstance* self, CInstance* other, RValue* out, uint32_t arg_count, RValue* args)
{
	RValue* sound_id = &args[1];
	try_overwrite_sound_id(sound_id);
	audio_play_sound_on_original(self, other, out, arg_count, args);
}

void setup_sound()
{
	uint32_t audio_play_sound    = (uint32_t)loader_get_builtin("audio_play_sound");
	uint32_t audio_play_sound_at = (uint32_t)loader_get_builtin("audio_play_sound_at");
	uint32_t audio_play_sound_on = (uint32_t)loader_get_builtin("audio_play_sound_on");

	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound),
		&audio_play_sound_detour, 
		reinterpret_cast<void**>(&audio_play_sound_original)
	);

	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound_at),
		&audio_play_sound_at_detour,
		reinterpret_cast<void**>(&audio_play_sound_at_original)
	);

	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound_on),
		&audio_play_sound_on_detour,
		reinterpret_cast<void**>(&audio_play_sound_on_original)
	);

	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound));
	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound_at));
	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound_on));
}

void destroy_sounds()
{
	RValue cur_id;
	for (auto& [_id, _sound_asset] : loaded_sound)
	{
		RValue result = 0;
		switch (_sound_asset.type)
		{
		case SOUND_STREAM:
			result = loader_call_builtin_ex("audio_destroy_stream", nullptr, nullptr, { *_sound_asset.ref });
			break;

		case SOUND_BUFFER:
			if (_sound_asset.ref->value_real == -1)
				break;

			result = loader_call_builtin_ex("audio_free_buffer_sound", nullptr, nullptr, { *_sound_asset.buffer_ref });

			if (_sound_asset.buffer_ref)
			{
				loader_call_builtin_ex("buffer_delete", nullptr, nullptr, { *_sound_asset.buffer_ref });
			}
			break;
		}

		if (result.truthy() == false)
		{
			loader_log_error("destroy_sound(): error occured trying to delete sound asset with id \"{}\" (0x{:08x})", _id, (int)_sound_asset.ref.get());
		}
	}

	loaded_sound.clear();
}

RValue buffer_create(int size, YYBufferType type, int alignment)
{
	return loader_call_builtin_ex("buffer_create", nullptr, nullptr, { size, type, alignment });
}

RValue buffer_get_address(RValue buffer)
{
	return loader_call_builtin_ex("buffer_get_address", nullptr, nullptr, { buffer });
}

RValue buffer_load_partial(RValue buffer, const char* filename, int offset, int src_len, int dest_offset)
{
	return loader_call_builtin_ex("buffer_load_partial", nullptr, nullptr, { buffer, filename, offset, src_len, dest_offset });
}

RValue buffer_fill(RValue buffer, int offset, int type, int value, int size)
{
	return loader_call_builtin_ex("buffer_fill", nullptr, nullptr, { buffer, size, type, value, size });
}

std::shared_ptr<RValue> load_vorbis(const std::filesystem::path& entry)
{
	std::string file_stem = entry.stem().generic_string();
	std::string file_path = entry.generic_string();

	const int original_sound_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { file_stem }).get_s32();

	if (loader_call_builtin_ex("audio_exists", nullptr, nullptr, { original_sound_id }).truthy())
	{
		std::shared_ptr<RValue> new_sound_asset = std::make_shared<RValue>();
		*new_sound_asset = loader_call_builtin_ex("audio_create_stream", nullptr, nullptr, { file_path });

		loaded_sound[original_sound_id] = { SOUND_STREAM, std::move(new_sound_asset)};
		return loaded_sound[original_sound_id].buffer_ref;
	}
	return nullptr;
}

std::shared_ptr<RValue> load_waveform(const std::filesystem::path& entry)
{
	std::string file_stem = entry.stem().generic_string();
	std::string file_path = entry.generic_string();

	const int original_sound_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { file_stem }).get_s32();

	if (loader_call_builtin_ex("audio_exists", nullptr, nullptr, { original_sound_id }).truthy())
	{
		Riff* waveform_data = read_riff(file_path);

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

		std::unique_ptr<RValue> new_buffer = std::make_unique<RValue>();
		*new_buffer = buffer_create(data_size, buffer_wrap, 1);
		if (new_buffer->value_real < 0)
		{
			std::string err_string = "load_waveform(): error creating buffer; ";
			switch (int(new_buffer->value_real))
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

		buffer_fill(*new_buffer, 0, buffer_u8, 24, data_size);
		RValue buffer_addr = buffer_get_address(new_buffer.get());
		if (buffer_addr.value_ptr == nullptr)
		{
			loader_log_error("load_waveform(): error creating buffer; (buffer_addr == nullptr)");
			return nullptr;
		}

		memcpy(buffer_addr.value_ptr, waveform_data->data, data_size);
		
		enum
		{
			audio_mono,
			audio_stereo,
			audio_3d
		};

		int buff_fmt = waveform_data->format_block.bit_depth == 16 ? buffer_s16 : buffer_u8;
		int buff_sample_rate = int(waveform_data->format_block.sample_speed);
		int buff_offset = int(waveform_data->data_offset);
		int buff_length = int(waveform_data->file_size - waveform_data->data_offset);
		int buff_channels;

		if (waveform_data->format_block.channel_count > 2)
			buff_channels = int(audio_3d);
		else
			//buff_channels = int(audio_3d);
			buff_channels = int(waveform_data->format_block.channel_count == 1 ? audio_mono : audio_stereo);

		std::unique_ptr<RValue> new_sound_asset = std::make_unique<RValue>();
		*new_sound_asset = loader_call_builtin_ex(
			"audio_create_buffer_sound",
			nullptr,
			nullptr,
			{
				*new_buffer,
				buff_fmt,
				buff_sample_rate,
				buff_offset,
				buff_length,
				buff_channels,
			}
		);
		
		waveform_data->free();
		delete waveform_data;

		if (new_sound_asset->value_real == -1)
		{
			loader_log_error("load_waveform(): error while attempting to load waveform \"{}\"", file_path);
			return nullptr;
		}

		loaded_sound[original_sound_id] = { SOUND_BUFFER, std::move(new_sound_asset), std::move(new_buffer)};

		return loaded_sound[original_sound_id].buffer_ref;
	}
	return nullptr;
}

void load_sounds(TexturePack* pack, const std::vector<std::filesystem::path>& sound_paths)
{
	loader_log_info("loading {} sound for texture pack \"{}\"", sound_paths.size(), pack->name);

	for (auto& entry : sound_paths)
	{
		logger_nestscope nest;

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