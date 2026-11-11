#include "sprite.h"

#include "pack.h"

#include <loader/yyc.h>

#include <filesystem>
#include <unordered_map>

#include <yaml-cpp/yaml.h>
#include <yaml_helper.h>

std::unordered_map<uint32_t, SpriteRemapData> g_sprite_remap;

void reset_sprites()
{
	for (auto& entry : g_sprite_remap)
	{
		if (entry.second.loaded)
		{
			loader_call_builtin_ex("sprite_delete", nullptr, nullptr, { entry.second.sprite_index });
		}
	}

	g_sprite_remap.clear();
}

void load_sprites(TexturePack* pack, const std::vector<std::filesystem::path>& sprite_paths)
{
	loader_log_info("loading {} sprites for texture pack \"{}\"", sprite_paths.size(), pack->name);

	// increments if a loaded sprite overwrites another custom sprite
	int deleted_textures = 0;

	{
		logger_nestscope nest;

		for (auto& entry : sprite_paths)
		{
			if (entry.extension() != ".png")
			{
				continue;
			}

			std::string file_stem = entry.stem().generic_string();
			std::string file_path = entry.generic_string();
			std::string sprite_name_str = file_stem.substr(0, file_stem.find("_strip"));

			int sprite_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { sprite_name_str }).get_s32();

			if (loader_call_builtin_ex("sprite_exists", nullptr, nullptr, { sprite_id }).truthy())
			{
				if (g_sprite_remap.contains(sprite_id))
				{
					auto& remap_data = g_sprite_remap[sprite_id];

					if (remap_data.loaded)
					{
						unload_sprite_from_memory(&remap_data);
					}

					g_sprite_remap.erase(sprite_id);

					deleted_textures++;
				}

				auto now = std::chrono::high_resolution_clock::now();

				SpriteRemapData mapping;

				mapping.resource_path = entry;
				mapping.xoffset = loader_call_builtin_ex("sprite_get_xoffset", nullptr, nullptr, { sprite_id }).get_real();
				mapping.yoffset = loader_call_builtin_ex("sprite_get_yoffset", nullptr, nullptr, { sprite_id }).get_real();
				mapping.last_draw_time = 0;
				mapping.sprite_index = 0;
				mapping.loaded = false;

				g_sprite_remap[sprite_id] = std::move(mapping);
			}
		}

		if (deleted_textures > 0)
		{
			loader_log_warn("texture pack \"{}\" overwrote {} custom textures...", pack->name, deleted_textures);
		}
	}
}

void load_sprite_to_memory(SpriteRemapData* remap_data)
{
	int framecount = 1;
	std::string file_stem = remap_data->resource_path.stem().generic_string();
	std::string file_path = remap_data->resource_path.generic_string();
	std::string sprite_name_str = file_stem.substr(0, file_stem.find("_strip"));

	//loader_log_debug("loading custom sprite \"{}\" into memory", sprite_name_str);

	int sprite_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { sprite_name_str }).get_s32();

	if (loader_call_builtin_ex("sprite_exists", nullptr, nullptr, { sprite_id }).truthy())
	{
		auto is_digits = [=](std::string& str)
			{
				for (char ch : str)
				{
					int v = ch; // ASCII Val converted
					if (!(ch >= 48 && ch <= 57))
					{
						return false;
					}
				}

				return true;
			};

		if (file_stem.contains("_strip"))
		{
			std::string frames_str = file_stem.substr(file_stem.find("_strip") + 6, file_stem.size());

			if (is_digits(frames_str))
			{
				framecount = std::stoi(frames_str);
			}
		}

		int sprite_new = loader_call_builtin_ex("sprite_add", nullptr, nullptr, { file_path, framecount, false, false, remap_data->xoffset, remap_data->yoffset }).get_s32();

		remap_data->loaded = true;
		remap_data->sprite_index = sprite_new;
	}
}

void unload_sprite_from_memory(SpriteRemapData* remap_data)
{
	std::string file_stem = remap_data->resource_path.stem().generic_string();
	std::string sprite_name_str = file_stem.substr(0, file_stem.find("_strip"));

	//loader_log_debug("unloading custom sprite \"{}\" from memory", sprite_name_str);

	loader_call_builtin_ex("sprite_delete", nullptr, nullptr, { remap_data->sprite_index });

	remap_data->loaded = false;
}

void overwrite_sprite_properties(const std::filesystem::path& pack_dir)
{
	const std::string path = pack_dir.generic_string() + "/spr_prop.ini";
	if (!file_exists(path))
	{
		// no spr_prop file found, return quietely
		return;
	}

	YAML::Node spr_prop;
	try
	{
		spr_prop = YAML::LoadFile(path);
	}
	catch (YAML::ParserException& _exc)
	{
		loader_log_error("overwrite_sprite_properties(): ParserException caught from \"{}\"", path);
		loader_log_error("\t^~~~~~~ {}", _exc.what());
		return;
	}

	for (YAML::const_iterator it = spr_prop.begin(); it != spr_prop.end(); ++it)
	{
		auto name = it->first.as<std::string>();

		int sprite_id = loader_call_builtin_ex("asset_get_index", nullptr, nullptr, { name }).get_s32();

		if (loader_call_builtin_ex("sprite_exists", nullptr, nullptr, { sprite_id }).truthy() && g_sprite_remap.contains(sprite_id))
		{
			SpriteRemapData& remap_data = g_sprite_remap[sprite_id];

			double xoffset = loader_call_builtin_ex("sprite_get_xoffset", nullptr, nullptr, { sprite_id }).get_real();
			double yoffset = loader_call_builtin_ex("sprite_get_yoffset", nullptr, nullptr, { sprite_id }).get_real();
			double speed = loader_call_builtin_ex("sprite_get_speed", nullptr, nullptr, { sprite_id }).get_real();
			int s_type = loader_call_builtin_ex("sprite_get_speed_type", nullptr, nullptr, { sprite_id }).get_s32();

			if (it->second["xoffset"].IsDefined())
			{
				xoffset = it->second["xoffset"].as<double>();
			}

			if (it->second["yoffset"].IsDefined())
			{
				yoffset = it->second["yoffset"].as<double>();
			}

			if (it->second["speed"].IsDefined())
			{
				speed = it->second["speed"].as<double>();
			}

			remap_data.xoffset = xoffset;
			remap_data.yoffset = yoffset;
			remap_data.speed = speed;
			remap_data.speedtype = s_type;

			loader_log_info("set props for texture {}", name);
		}
	}
}