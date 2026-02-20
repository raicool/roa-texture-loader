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

void call_audio_play_sound(const char* sound_name, float priority, bool loop, float gain)
{
	RValue _rv_sound_name = sound_name;
	RValue* _asset_get_index_args[] = { &_rv_sound_name };
	RValue* _sound_id = loader_yyc_call_func(asset_get_index, 1, _asset_get_index_args);

	RValue _rv_priority = priority;
	RValue _rv_loop = false;
	RValue _rv_gain = gain;

	RValue* audio_play_sound_args[] = { _sound_id, &_rv_priority, &_rv_loop, &_rv_gain };
	loader_yyc_call_func("audio_play_sound", 4, audio_play_sound_args);
}