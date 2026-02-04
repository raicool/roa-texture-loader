#include "sprite.h"

#include "func_ids.h"

#include <loader/yyc.h>

#include <filesystem>
#include <unordered_map>

#include <yaml-cpp/yaml.h>
#include <yaml_helper.h>

std::unordered_map<double, RValue*> original_sprites;
//std::unordered_map<double, RValue*> new_sprites;

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
		RValue sprite_id_var = RValue(sprite_id);
		RValue* args[] = { &sprite_id_var, sprite_asset };
		RValue* delete_args[] = { sprite_asset };
		loader_yyc_call_func(sprite_assign, 2, args);
		loader_yyc_call_func(sprite_delete, 1, delete_args);
		delete sprite_asset;
	}

// 	for (auto& [sprite_id, sprite_asset] : new_sprites)
// 	{
// 		RValue* delete_args[] = { sprite_asset };
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

		RValue sprite_name = RValue(sprite_name_str);
		RValue* args[] = { &sprite_name };
		RValue* sprite_id = loader_yyc_call_func(asset_get_index, 1, args);

		args[0] = sprite_id;
		if (loader_yyc_call_func(sprite_exists, 1, args)->truthy())
		{
			// i really have to do this
			RValue zero = RValue();
			zero.type = GML_TYPE_INT32;
			zero.valueInt32 = 0;

			RValue frames = RValue();
			frames.type = GML_TYPE_INT32;
			frames.valueInt32 = 1;

			RValue* xoffset = loader_yyc_call_func(sprite_get_xoffset, 1, args);
			RValue* yoffset = loader_yyc_call_func(sprite_get_yoffset, 1, args);

			// TODO: duplicating sprites is extremely buggy and will 
			// often crash with no way of debugging :)
			// so idk what to do here
			
// 			if (!original_sprites.contains(sprite_id->valueReal))
// 			{
// 				RValue* oldsprite = loader_yyc_call_func(sprite_duplicate, 1, args);
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
// 				RValue* delete_args[] = { new_sprites[sprite_id->valueReal] };
// 				loader_yyc_call_func(sprite_delete, 1, delete_args);
// 				delete new_sprites[sprite_id->valueReal];
// 			}

			RValue sprite_filename = RValue(file_path);
// 			RValue* args_replace[] = { &sprite_filename, &frames, &zero, &zero, xoffset, yoffset };
// 			RValue* sprite_new = loader_yyc_call_func(sprite_add, 6, args_replace);

			RValue* args_replace[] = { sprite_id, &sprite_filename, &frames, &zero, &zero, xoffset, yoffset };
			RValue* sprite_new = loader_yyc_call_func(sprite_replace, 7, args_replace);

//			new_sprites[sprite_id->valueReal] = sprite_new;
// 			RValue* args_assign[] = { sprite_id, sprite_new };
// 			loader_yyc_call_func(sprite_assign, 2, args_assign);
// 
// 			RValue* args_delete[] = { sprite_new };
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

		RValue sprite_name = RValue(it->first.as<std::string>());
		RValue* asset_get_index_args[] = { &sprite_name };

		RValue* sprite_id = loader_yyc_call_func(asset_get_index, 1, asset_get_index_args);
		RValue* sprite_exists_args[] = { sprite_id };
		if (loader_yyc_call_func(sprite_exists, 1, sprite_exists_args)->truthy())
		{
			RValue xoffset = RValue(0.0f);
			RValue yoffset = RValue(0.0f);
			RValue speed = RValue(0.0f);

			if (it->second["xoffset"].IsDefined())
			{
				xoffset = RValue(it->second["xoffset"].as<double>());
			}

			if (it->second["yoffset"].IsDefined())
			{
				yoffset = RValue(it->second["yoffset"].as<double>());
			}

			if (it->second["speed"].IsDefined())
			{
				speed = RValue(it->second["speed"].as<double>());
			}

			RValue* s_type = loader_yyc_call_func(sprite_get_speed_type, 1, sprite_exists_args);
			RValue* sprite_set_offset_args[] = { sprite_id, &xoffset, &yoffset };
			RValue* sprite_set_speed_args[] = { sprite_id, &speed, s_type};

			loader_yyc_call_func(sprite_set_offset, 3, sprite_set_offset_args);
			loader_yyc_call_func(sprite_set_speed, 3, sprite_set_speed_args);

			delete sprite_id;
			delete s_type;
		}
	}
}

RValue* get_sprite_name(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	RValue sprite_id = RValue(id);

	RValue* sprite_get_name_args[] = { &sprite_id };
	RValue* name = loader_yyc_call_func(sprite_get_name, 1, sprite_get_name_args);

	return name;
}

RValue* get_sprite_texture(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	RValue sprite_id = RValue(id);

	// i really have to do this
	RValue zero = RValue(0);
	RValue* sprite_get_texture_args[] = { &sprite_id, &zero };
	RValue* uvs = loader_yyc_call_func(sprite_get_texture, 2, sprite_get_texture_args);

	return uvs;
}

RValue* get_sprite_width(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	RValue sprite_id = RValue(id);

	RValue* args[] = { &sprite_id };
	return loader_yyc_call_func(sprite_get_width, 1, args);
}

RValue* get_sprite_height(int id)
{
	if (!ready)
	{
		__setup_funcids();
	}

	RValue sprite_id = RValue(id);

	RValue* args[] = { &sprite_id };
	return loader_yyc_call_func(sprite_get_height, 1, args);
}

RValue* get_texture_width(RValue* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture width from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	RValue* args[] = { texture };
	return loader_yyc_call_func(texture_get_width, 1, args);
}

RValue* get_texture_height(RValue* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture height from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	RValue* args[] = { texture };
	return loader_yyc_call_func(texture_get_height, 1, args);
}

RValue* get_texture_texel_width(RValue* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture texel width from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	RValue* args[] = { texture };
	return loader_yyc_call_func(texture_get_texel_width, 1, args);
}

RValue* get_texture_texel_height(RValue* texture)
{
	if (texture->type != GML_TYPE_POINTER)
	{
		loader_log_warn("attempted to get texture texel height from an invalid variable type (texture->type != GML_TYPE_POINTER)");
		return nullptr;
	}

	RValue* args[] = { texture };
	return loader_yyc_call_func(texture_get_texel_height, 1, args);
}