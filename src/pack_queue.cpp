#include "pack_queue.h"

#include "sound.h"
#include "sprite.h"

#include <loader/yyc.h>
#include <loader/yoyo.h>

#include <filesystem>
#include <vector>

TexturePackList loaded_packs;

void update_pack_queue()
{
	loaded_packs.clear();

	char module_path[128];
	GetModuleFileName(0, module_path, 128);
	const std::string module_folder = std::filesystem::path(module_path).parent_path().string();
	const std::string mods_folder = std::format("{}/{}", module_folder, SPRITE_DIRECTORY);

	std::filesystem::create_directory(mods_folder);

	for (auto& entry : std::filesystem::directory_iterator(mods_folder))
	{
		if (entry.is_directory())
		{
			std::shared_ptr<TexturePack> new_pack = open_texture_pack(entry.path());
			if (new_pack != nullptr)
			{
				loaded_packs.emplace(new_pack);
			}
			
		}
	}

	loaded_packs.sort();
}

void apply_packs(bool play_sound)
{
	reset_sprites();
	destroy_sounds();

	if (play_sound)
	{
		RValue sound_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { "mfx_notice" });
		loader_call_builtin_ex("audio_play_sound", nullptr, nullptr, { sound_id, 1, false, 0.3 });
	}

	for (auto iter = loaded_packs.rbegin(); iter != loaded_packs.rend(); ++iter)
	{
		auto* _pack = iter->get();
		if (_pack->enabled)
		{
			// load sprites
			std::vector<std::filesystem::path> sprite_paths;
			if (std::filesystem::exists(_pack->sprite_path))
			{
				for (const auto& _sprite_file : std::filesystem::directory_iterator(_pack->sprite_path))
				{
					sprite_paths.emplace_back(_sprite_file);
				}

				load_sprites(_pack, sprite_paths);
				overwrite_sprite_properties(_pack->pack_path);

				sprite_paths.clear();
			}

			// load sounds
			std::vector<std::filesystem::path> sound_paths;
			if (std::filesystem::exists(_pack->sound_path))
			{
				for (const auto& _sound_file : std::filesystem::directory_iterator(_pack->sound_path))
				{
					sound_paths.emplace_back(_sound_file);
				}

				load_sounds(_pack, sprite_paths);

				sound_paths.clear();
			}
		}
	}
}

TexturePackList* get_loaded_packs()
{
	return &loaded_packs;
}