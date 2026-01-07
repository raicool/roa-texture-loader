#include "callback.h"

#include "mod.h"
#include "ui.h"
#include "packlist.h"
#include "pack.h"

#include <loader/d3d11_hook.h>

DWORD WINAPI entry(LPVOID hModule)
{
	setup_ui();

	update_packlist();
	apply_packs();

	std::string mod_name = MOD_NAME;
	loader_fetch_mod_repository(mod_name);

	// add a callback to be called whenever a frame is rendered
	loader_add_present_callback(dx_present);

	// add a callback to be called whenever rival's window attempts to process an event
	loader_add_wndproc_callback(handle_wndproc);

	return TRUE;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		HANDLE hHandle = CreateThread(NULL, 0, entry, hModule, 0, NULL);
		if (hHandle != NULL)
		{
			CloseHandle(hHandle);
		}
	}
	else if (fdwReason == DLL_PROCESS_DETACH && !lpReserved)
	{
		FreeLibraryAndExitThread(hModule, 0);
	}

	return TRUE;
}