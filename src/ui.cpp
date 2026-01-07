#include "ui.h"

#include "mod.h"
#include "packlist.h"

#include <loader/d3d11_hook.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_win32.h>

#include <memory>
#include <config.h>

void __setup_ui()
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
		if (ImGui::BeginMenuBar())
		{
			config_entry* entry = get_config_entry(SAVE_ORIGINAL_SPRITES);
			if (ImGui::MenuItem("Save Original Sprites", 0, false, entry->value))
			{
				set_config_entry(SAVE_ORIGINAL_SPRITES, !entry->value);
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(entry->description.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::EndMenuBar();
		}

		if (ImGui::Button("Reload Textures"))
		{
			apply_packs();
		}

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
		ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 48);
		ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 128);
		ImGui::TableHeadersRow();
		uint32_t i = 0;
		for (const std::shared_ptr<pack>& _pack : *pack_list)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::PushID(i);

			render_packdata(_pack);

			ImGui::PopID();
			i++;
		}

		ImGui::EndTable();
	}
}

void render_packdata(const std::shared_ptr<pack>& packdata)
{

	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%s", packdata->name.c_str());
	ImGui::Text("by %s", packdata->author.c_str());
	ImGui::Text("%s", packdata->description.c_str());

	ImGui::TableNextColumn();
	ImGui::Checkbox("Enabled?", &packdata->enabled);

	// render pack image last since we need to know the height of the table row
	ImGui::TableSetColumnIndex(0);
	auto* table = ImGui::GetCurrentContext()->CurrentTable;
	if (packdata->pack_img)
	{
		auto rect = ImGui::TableGetCellBgRect(table, table->CurrentColumn);
		ImGui::Image(packdata->pack_img, { rect.GetHeight(), rect.GetHeight() });
	}
}