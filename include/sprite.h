#pragma once

#include <filesystem>

void reset_sprites();
void load_sprite(const std::filesystem::path& entry);
void overwrite_sprite_properties(const std::filesystem::path& pack_dir);

struct RValue;

// sprite helper funcs
RValue* get_sprite_name(int id);
RValue* get_sprite_texture(int id);
RValue* get_sprite_width(int id);
RValue* get_sprite_height(int id);

// texture page helper funcs
RValue* get_texture_width(RValue* texture);
RValue* get_texture_height(RValue* texture);
RValue* get_texture_texel_width(RValue* texture);
RValue* get_texture_texel_height(RValue* texture);