#include "pack_queue.h"

#include "sound.h"
#include "sprite.h"
#include "func_ids.h"

#include <loader/yyc.h>
#include <GMLScriptEnv/yoyo.h>

#include <filesystem>
#include <vector>

pack_queue loaded_packs;

void update_pack_queue()
{
	loaded_packs.clear();

	std::filesystem::create_directory(SPRITE_DIRECTORY);

	for (auto& entry : std::filesystem::directory_iterator(SPRITE_DIRECTORY))
	{
		if (entry.is_directory())
		{
			std::shared_ptr<pack> new_pack = open_texture_pack(entry.path());
			if (new_pack != nullptr)
			{
				loaded_packs.emplace(new_pack);
			}
			
		}
	}

	loaded_packs.sort();

	// this probably doesnt need to be a thread
	// it waits for a valid directx swapchain before 
	// creating d3d11 textures for a queue of pack images
	std::thread fetch(fetch_packimg);
	fetch.join(); //< todo: remove the need to join thread
}

void apply_packs(bool play_sound)
{
	reset_sprites();

	if (play_sound)
	{
		call_audio_play_sound("mfx_notice", 1, false, 0.5);
	}

	for (auto iter = loaded_packs.rbegin(); iter != loaded_packs.rend(); ++iter)
	{
		auto* _pack = iter->get();
		if (_pack->enabled)
		{
			// load sprites
			if (std::filesystem::exists(_pack->sprite_path))
			{
				for (const auto& _sprite_file : std::filesystem::directory_iterator(_pack->sprite_path))
				{
					load_sprite(_sprite_file.path());
				}

				overwrite_sprite_properties(_pack->pack_path);
			}

			// load sounds
			if (std::filesystem::exists(_pack->sound_path))
			{
				for (const auto& _sound_file : std::filesystem::directory_iterator(_pack->sound_path))
				{
					load_ogg(_sound_file.path());
				}
			}
		}
	}
}

pack_queue* get_loaded_packs()
{
	return &loaded_packs;
}