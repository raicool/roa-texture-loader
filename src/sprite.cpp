#include "sprite.h"

#include "func_ids.h"

#include <loader/yyc.h>

#include <filesystem>
#include <unordered_map>

#include <yaml-cpp/yaml.h>
#include <yaml_helper.h>

std::unordered_map<double, GMLVar*> original_sprites;
//std::unordered_map<double, GMLVar*> new_sprites;

bool is_digits(std::string& str)
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
}
void reset_sprites()
{
	for (auto& [sprite_id, sprite_asset] : original_sprites)
	{
		GMLVar sprite_id_var = GMLVar(sprite_id);
		GMLVar* args[] = { &sprite_id_var, sprite_asset };
		GMLVar* delete_args[] = { sprite_asset };
		loader_yyc_call_func(sprite_assign, 2, args);
		loader_yyc_call_func(sprite_delete, 1, delete_args);
		delete sprite_asset;
	}

// 	for (auto& [sprite_id, sprite_asset] : new_sprites)
// 	{
// 		GMLVar* delete_args[] = { sprite_asset };
// 		loader_yyc_call_func(sprite_delete, 1, delete_args);
// 		delete sprite_asset;
// 	}

	original_sprites.clear();
//	new_sprites.clear();
}

void load_sprite(const std::filesystem::path& entry)
{
	if (!ready)
	{
		__setup_funcids();
	}

	if (entry.extension() == ".png")
	{
		std::string file_stem = entry.stem().generic_string();
		std::string file_path = entry.generic_string();
		std::string sprite_name_str = file_stem.substr(0, file_stem.find("_strip"));

		GMLVar sprite_name = GMLVar(sprite_name_str);
		GMLVar* args[] = { &sprite_name };
		GMLVar* sprite_id = loader_yyc_call_func(asset_get_index, 1, args);

		args[0] = sprite_id;
		if (loader_yyc_call_func(sprite_exists, 1, args)->truthy())
		{
			// i really have to do this
			GMLVar zero = GMLVar();
			zero.type = GML_TYPE_INT32;
			zero.valueInt32 = 0;

			GMLVar frames = GMLVar();
			frames.type = GML_TYPE_INT32;
			frames.valueInt32 = 1;

			GMLVar* xoffset = loader_yyc_call_func(sprite_get_xoffset, 1, args);
			GMLVar* yoffset = loader_yyc_call_func(sprite_get_yoffset, 1, args);

			// TODO: duplicating sprites is extremely buggy and will 
			// often crash with no way of debugging :)
			// so idk what to do here
			
// 			if (!original_sprites.contains(sprite_id->valueReal))
// 			{
// 				GMLVar* oldsprite = loader_yyc_call_func(sprite_duplicate, 1, args);
// 				original_sprites[sprite_id->valueReal] = oldsprite;
// 			}

			if (file_stem.contains("_strip"))
			{
				std::string frames_str = file_stem.substr(file_stem.find("_strip") + 6, file_stem.size());

				if (is_digits(frames_str))
				{
					frames.valueInt32 = std::stoi(frames_str);
				}
			}

			// delete previous custom texture in memory for sprite if one exists
// 			if (new_sprites.contains(sprite_id->valueReal))
// 			{
// 				GMLVar* delete_args[] = { new_sprites[sprite_id->valueReal] };
// 				loader_yyc_call_func(sprite_delete, 1, delete_args);
// 				delete new_sprites[sprite_id->valueReal];
// 			}

			GMLVar sprite_filename = GMLVar(file_path);
// 			GMLVar* args_replace[] = { &sprite_filename, &frames, &zero, &zero, xoffset, yoffset };
// 			GMLVar* sprite_new = loader_yyc_call_func(sprite_add, 6, args_replace);

			GMLVar* args_replace[] = { sprite_id, &sprite_filename, &frames, &zero, &zero, xoffset, yoffset };
			GMLVar* sprite_new = loader_yyc_call_func(sprite_replace, 7, args_replace);

//			new_sprites[sprite_id->valueReal] = sprite_new;
// 			GMLVar* args_assign[] = { sprite_id, sprite_new };
// 			loader_yyc_call_func(sprite_assign, 2, args_assign);
// 
// 			GMLVar* args_delete[] = { sprite_new };
// 			loader_yyc_call_func(sprite_delete, 1, args_assign);
// 			delete sprite_new;
			delete xoffset;
			delete yoffset;

			loader_log_debug("custom texture \"{}\" (id = {}) has been loaded", sprite_name_str, sprite_id->valueReal);
		}
		delete sprite_id;
	}
}

void overwrite_sprite_properties(const std::filesystem::path& pack_dir)
{
	if (!file_exists(pack_dir.generic_string() + "/spr_prop.ini"))
	{
		// no spr_prop file found, return quietely
		return;
	}

	const YAML::Node spr_prop = YAML::LoadFile(pack_dir.generic_string() + "/spr_prop.ini");

	for (YAML::const_iterator it = spr_prop.begin(); it != spr_prop.end(); ++it)
	{
		loader_log_debug(it->first.as<std::string>());

		GMLVar sprite_name = GMLVar(it->first.as<std::string>());
		GMLVar* asset_get_index_args[] = { &sprite_name };

		GMLVar* sprite_id = loader_yyc_call_func(asset_get_index, 1, asset_get_index_args);
		GMLVar* sprite_exists_args[] = { sprite_id };
		if (loader_yyc_call_func(sprite_exists, 1, sprite_exists_args)->truthy())
		{
			GMLVar xoffset = GMLVar(0.0f);
			GMLVar yoffset = GMLVar(0.0f);
			GMLVar speed = GMLVar(0.0f);

			if (it->second["xoffset"].IsDefined())
			{
				xoffset = GMLVar(it->second["xoffset"].as<double>());
			}

			if (it->second["yoffset"].IsDefined())
			{
				yoffset = GMLVar(it->second["yoffset"].as<double>());
			}

			if (it->second["speed"].IsDefined())
			{
				speed = GMLVar(it->second["speed"].as<double>());
			}
			GMLVar* s_type = loader_yyc_call_func(sprite_get_speed_type, 1, sprite_exists_args);
			GMLVar* sprite_set_offset_args[] = { sprite_id, &xoffset, &yoffset };
			GMLVar* sprite_set_speed_args[] = { sprite_id, &speed, s_type};

			loader_yyc_call_func(sprite_set_offset, 3, sprite_set_offset_args);
			loader_yyc_call_func(sprite_set_speed, 3, sprite_set_speed_args);

			delete sprite_id;
			delete s_type;
		}
	}
}

GMLVar* get_sprite_name(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	GMLVar sprite_id = GMLVar(id);

	GMLVar* sprite_get_name_args[] = { &sprite_id };
	GMLVar* name = loader_yyc_call_func(sprite_get_name, 1, sprite_get_name_args);

	return name;
}

GMLVar* get_sprite_texture(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	GMLVar sprite_id = GMLVar(id);

	// i really have to do this
	GMLVar zero = GMLVar(0);
	GMLVar* sprite_get_texture_args[] = { &sprite_id, &zero };
	GMLVar* uvs = loader_yyc_call_func(sprite_get_texture, 2, sprite_get_texture_args);

	return uvs;
}

GMLVar* get_sprite_width(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	GMLVar sprite_id = GMLVar(id);

	GMLVar* args[] = { &sprite_id };
	return loader_yyc_call_func(sprite_get_width, 1, args);
}

GMLVar* get_sprite_height(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	GMLVar sprite_id = GMLVar(id);

	GMLVar* args[] = { &sprite_id };
	return loader_yyc_call_func(sprite_get_height, 1, args);
}

GMLVar* get_texture_width(GMLVar* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture width from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	GMLVar* args[] = { texture };
	return loader_yyc_call_func(texture_get_width, 1, args);
}

GMLVar* get_texture_height(GMLVar* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture height from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	GMLVar* args[] = { texture };
	return loader_yyc_call_func(texture_get_height, 1, args);
}

GMLVar* get_texture_texel_width(GMLVar* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture texel width from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	GMLVar* args[] = { texture };
	return loader_yyc_call_func(texture_get_texel_width, 1, args);
}

GMLVar* get_texture_texel_height(GMLVar* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture texel height from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	GMLVar* args[] = { texture };
	return loader_yyc_call_func(texture_get_texel_height, 1, args);
}