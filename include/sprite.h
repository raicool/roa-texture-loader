#pragma once

#include <filesystem>

#include <loader/log.h>

struct TexturePack;
struct SpriteRemapData
{
	std::filesystem::path resource_path;

	int64_t last_draw_time = 0;

	// new sprite properties
	double xoffset = 0;
	double yoffset = 0;
	double speed = 0;
	int speedtype = 0;

	int sprite_index = 0;

	bool loaded = false;
};

void reset_sprites();
void load_sprite_to_memory(SpriteRemapData* remap_data);
void unload_sprite_from_memory(SpriteRemapData* remap_data);
void load_sprites(TexturePack* pack, const std::vector<std::filesystem::path>& sprite_paths);
void overwrite_sprite_properties(const std::filesystem::path& pack_dir);