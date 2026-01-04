#include "ui.h"

#include "mod.h"
#include "sprite.h"

#include <loader/d3d11_hook.h>

#include <imgui.h>
#include <imgui_impl_win32.h>


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
		ImGui::Text("Hello!");
		if (ImGui::InputInt("sprite_id", &sprite_id))
		{
			sprite = get_sprite_name(sprite_id);
		}

		if (sprite)
		{
			ImGui::Text("%s", sprite->getCString());

			GMLVar* texture = get_sprite_texture(sprite_id);
			
			if (texture->type == GML_TYPE_POINTER)
			{
				ImGui::Text("0x%08x", texture->type);

				GMLVar* width = get_sprite_width(sprite_id);
				GMLVar* height = get_sprite_height(sprite_id);

				ImGui::InputDouble("width", &width->valueReal);
				ImGui::InputDouble("height", &height->valueReal);

				//ImGui::Image(texture->valuePointer, { (float)width->valueReal, (float)height->valueReal });
			}
		}

		ImGui::End();
	}
}