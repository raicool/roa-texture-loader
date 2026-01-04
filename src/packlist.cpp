#include "packlist.h"

#include "pack.h"
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

	std::thread fetch(fetch_packimg);
	fetch.join(); //< todo: remove the need to join thread
}

void apply_packs()
{
	for (auto& _pack : loaded_packs)
	{
		if (_pack->enabled)
		{
			for (const auto& _sprite_file : std::filesystem::directory_iterator(_pack->sprite_path))
			{
				overwrite_sprite(_sprite_file.path());
			}
		}
	}
}

packlist* get_packlist()
{
	return &loaded_packs;
}