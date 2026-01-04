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
			render_texturepack_table(pack_list);
		}
	}
	ImGui::End();
}

void render_texturepack_table(packlist* pack_list)
{
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_HighlightHoveredColumn;

	if (ImGui::BeginTable("##", 3, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 32);
		ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 128);
		for (const std::shared_ptr<pack>& _pack : *pack_list)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			// use pointer as id
			ImGui::PushID((int32_t)_pack.get());
			render_packdata(_pack);
			ImGui::PopID();
		}

		ImGui::EndTable();
	}
}

void render_packdata(const std::shared_ptr<pack>& packdata)
{
	if (packdata->pack_img)
	{
		ImGui::Image(packdata->pack_img, { 32, 32 });
	}

	ImGui::TableNextColumn();

	ImGui::Text("%s", packdata->name.c_str());
	ImGui::Text("by %s", packdata->author.c_str());
	ImGui::Text("%s", packdata->description.c_str());

	ImGui::TableNextColumn();
	ImGui::Checkbox("Enabled?", &packdata->enabled);
}