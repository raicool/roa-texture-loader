#include "ui.h"

#include "mod.h"
#include "packlist.h"

#include <loader/d3d11_hook.h>

#include <imgui.h>
#include <imgui_impl_win32.h>

#include <memory>

void setup_ui()
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(loader_get_window());
}

static int sprite_id = 0;
GMLVar* sprite;
void render_ui()
{
	if (ImGui::Begin(MOD_NAME))
	{
		packlist* pack_list = get_packlist();

		if (pack_list)
		{
			for (const std::shared_ptr<pack>& _pack : *pack_list)
			{
				render_packdata(_pack);
			}
		}

		ImGui::End();
	}
}

void render_packdata(const std::shared_ptr<pack>& packdata)
{
	ImGui::Image(packdata->pack_img, { 32, 32 });
	ImGui::Text("%s", packdata->name.c_str());
	ImGui::Text("by %s", packdata->author.c_str());
	ImGui::Text("%s", packdata->description.c_str());
}