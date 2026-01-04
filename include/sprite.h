#pragma once

#include <filesystem>

void __setup_funcids();

void reset_sprites();
void overwrite_sprite(const std::filesystem::path& entry);

struct GMLVar;

// sprite helper funcs
GMLVar* get_sprite_name(int id);
GMLVar* get_sprite_texture(int id);
GMLVar* get_sprite_width(int id);
GMLVar* get_sprite_height(int id);

// texture page helper funcs
GMLVar* get_texture_width(GMLVar* texture);
GMLVar* get_texture_height(GMLVar* texture);
GMLVar* get_texture_texel_width(GMLVar* texture);
GMLVar* get_texture_texel_height(GMLVar* texture);