#include "packlist.h"

#include "pack.h"
#include "sound.h"
#include "sprite.h"

#include <filesystem>
#include <vector>

packlist loaded_packs;

void update_packlist()
{
	loaded_packs.clear();

	std::filesystem::create_directory(SPRITE_DIRECTORY);

	for (auto& entry : std::filesystem::directory_iterator(SPRITE_DIRECTORY))
	{
		if (entry.is_directory())
		{
			std::shared_ptr<pack> new_pack = open_texture_pack(entry.path());
			new_pack->enabled = true;

			loaded_packs.emplace_back(new_pack);
		}
	}

	// this probably doesnt need to be a thread
	// it waits for a valid directx swapchain before 
	// creating d3d11 textures for a queue of pack images
	std::thread fetch(fetch_packimg);
	fetch.join(); //< todo: remove the need to join thread
}

void apply_packs()
{
	reset_sprites();

	for (auto& _pack : loaded_packs)
	{
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
				for (const auto& _sound_file : std::filesystem::directory_iterator(_pack->sound_path))
				{
					load_ogg(_sound_file.path());
				}
		}
	}
}

packlist* get_packlist()
{
	return &loaded_packs;
}