#include "func_ids.h"

#include <loader/yyc.h>

void __setup_funcids()
{
	asset_get_index = loader_yyc_get_funcid("asset_get_index");
	audio_exists = loader_yyc_get_funcid("audio_exists");
	audio_create_stream = loader_yyc_get_funcid("audio_create_stream");
	audio_destroy_stream = loader_yyc_get_funcid("audio_destroy_stream");
	audio_exists = loader_yyc_get_funcid("audio_exists");
	sprite_get_name = loader_yyc_get_funcid("sprite_get_name");
	sprite_add = loader_yyc_get_funcid("sprite_add");
	sprite_replace = loader_yyc_get_funcid("sprite_replace");
	sprite_delete = loader_yyc_get_funcid("sprite_delete");
	sprite_assign = loader_yyc_get_funcid("sprite_assign");
	sprite_duplicate = loader_yyc_get_funcid("sprite_duplicate");
	sprite_exists = loader_yyc_get_funcid("sprite_exists");
	sprite_get_number = loader_yyc_get_funcid("sprite_get_number");
	sprite_get_uvs = loader_yyc_get_funcid("sprite_get_uvs");
	sprite_get_xoffset = loader_yyc_get_funcid("sprite_get_xoffset");
	sprite_get_yoffset = loader_yyc_get_funcid("sprite_get_yoffset");
	sprite_get_speed_type = loader_yyc_get_funcid("sprite_get_speed_type");
	sprite_set_offset = loader_yyc_get_funcid("sprite_set_offset");
	sprite_set_speed = loader_yyc_get_funcid("sprite_set_speed");
	sprite_get_texture = loader_yyc_get_funcid("sprite_get_texture");
	sprite_get_width = loader_yyc_get_funcid("sprite_get_width");
	sprite_get_height = loader_yyc_get_funcid("sprite_get_height");
	texture_get_width = loader_yyc_get_funcid("texture_get_width");
	texture_get_height = loader_yyc_get_funcid("texture_get_height");
	texture_get_texel_width = loader_yyc_get_funcid("texture_get_texel_width");
	texture_get_texel_height = loader_yyc_get_funcid("texture_get_texel_height");

	ready = true;
}