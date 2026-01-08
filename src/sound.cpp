#include "sound.h"

#include "func_ids.h"

#include "loader/yyc.h"

#include <string>
#include <filesystem>
#include <unordered_map>

std::unordered_map<double, GMLVar*> loaded_sound;

//
//	args
//	audio_play_sound(index, priority, loop, [gain], [offset], [pitch], [listener_mask]);
//
void (*audio_play_sound_original)(
	GMLVar*,
	void*,
	void*,
	uint32_t,
	GMLVar*,
	GMLVar*,
	GMLVar*,
	GMLVar*,
	GMLVar*,
	GMLVar*,
	GMLVar*
	);
void audio_play_sound_detour(
	GMLVar* _unknown_1, 
	void* _unknown_2, 
	void* _unknown_3,
	uint32_t arg_count, 
	GMLVar* index, 
	GMLVar* priority, 
	GMLVar* loop,
	GMLVar* gain,
	GMLVar* offset,
	GMLVar* pitch,
	GMLVar* listener_mask
)
{
	if (loaded_sound.size() <= 0)
		goto leave_detour;

	if (index->type == GML_TYPE_REAL)
	{
		if (loaded_sound.contains(index->valueReal))
		{
			index->valueReal = loaded_sound[(uint32_t)index->valueReal]->valueReal;
		}
	}
	else
	{
		loader_log_warn("audio_play_sound_detour(): index->type was not GML_TYPE_REAL");
	}

leave_detour:
	audio_play_sound_original(
		_unknown_1,
		_unknown_2,
		_unknown_3,
		arg_count,
		index,
		priority,
		loop,
		gain,
		offset,
		pitch,
		listener_mask
	);
}

void __setup_sound()
{
	uint32_t audio_play_sound = (uint32_t)loader_get_yyc_func_ptr("audio_play_sound");
	loader_hook_create(
		reinterpret_cast<void*>(audio_play_sound),
		&audio_play_sound_detour, 
		reinterpret_cast<void**>(&audio_play_sound_original)
	);
	loader_hook_enable(reinterpret_cast<void*>(audio_play_sound));
}

void destroy_sounds()
{
	GMLVar cur_id;
	for (auto& [_id, _sound_asset] : loaded_sound)
	{
		cur_id.type = GML_TYPE_REAL;
		cur_id.valueReal = _id;

		GMLVar* destroy_args[] = { &cur_id };
		GMLVar* result = loader_yyc_call_func(audio_destroy_stream, 1, destroy_args);

		if (result->valueReal == 1)
		{
			// successfully destroyed audio stream
			delete _sound_asset;
			loaded_sound.erase(_id);
		}
		else
		{
			loader_log_error("destroy_sound(): error occured trying to delete sound asset with id \"{}\" (0x{:08x})", _id, (void*)_sound_asset);
		}
	}
}

void load_ogg(const std::filesystem::path& entry)
{
	if (!ready)
	{
		__setup_funcids();
	}

	if (entry.extension() == ".ogg")
	{	
		std::string file_stem = entry.stem().generic_string();
		std::string file_path = entry.generic_string();

		GMLVar sound_name = GMLVar(file_stem);
		GMLVar* args[] = { &sound_name };
		GMLVar* sound_id = loader_yyc_call_func(asset_get_index, 1, args);

		args[0] = sound_id;
		if (loader_yyc_call_func(audio_exists, 1, args)->truthy())
		{
			GMLVar sound_filename = GMLVar(file_path);

			args[0] = &sound_filename;
			GMLVar* new_sound_asset = loader_yyc_call_func(audio_create_stream, 1, args);

			loaded_sound[(uint32_t)sound_id->valueReal] = new_sound_asset;
		}
	}
}