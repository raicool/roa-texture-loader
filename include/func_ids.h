#pragma once

inline bool ready = false;

// function ids
inline int asset_get_index;
inline int audio_exists;
inline int audio_create_stream;
inline int audio_destroy_stream;
inline int sprite_get_name;
inline int sprite_add;
inline int sprite_replace;
inline int sprite_delete;
inline int sprite_assign;
inline int sprite_duplicate;
inline int sprite_exists;
inline int sprite_get_number;
inline int sprite_get_uvs;
inline int sprite_get_xoffset;
inline int sprite_get_yoffset;
inline int sprite_get_speed_type;
inline int sprite_get_texture;
inline int sprite_get_width;
inline int sprite_get_height;
inline int sprite_set_offset;
inline int sprite_set_speed;
inline int texture_get_width;
inline int texture_get_height;
inline int texture_get_texel_width;
inline int texture_get_texel_height;

void __setup_funcids();