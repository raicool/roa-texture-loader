#include "callback.h"

#include "draw_cmd.h"
#include "mod.h"
#include "pack.h"
#include "pack_queue.h"
#include "sound.h"
#include "sprite.h"
#include "ui.h"

#include <loader/d3d11_hook.h>

LOADER_MOD_ENTRY initialize(loader_info* loaderinfo)
{
	loader_mod& mod = loader_mod::get();
	mod.name = MOD_NAME;
	mod.description = "custom texture loader";
	mod.repository = "https://github.com/raicool/roa-texture-loader";
	mod.commit_hash = GIT_COMMIT_HASH;

	// setup hooks/function ids
	init_draw_command_hooks();
	setup_sound();
	setup_ui();

	// read from custom_sprites directory and apply sprite overwrites
	update_pack_queue();
	apply_packs();

	// add a callback to be called whenever a frame is rendered
	loader_add_present_callback(dx_present);

	// add a callback to be called whenever rival's window attempts to process an event
	loader_add_wndproc_callback(handle_wndproc);

	return mod;
}

LOADER_MOD_EXIT shutdown()
{
	loader_log_info("shutting down texture thread...");
	{
		logger_nestscope nest;

		terminate_texture_thread();
	}
}