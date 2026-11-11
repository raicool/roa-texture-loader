#include "draw_cmd.h"

#include <loader/memory.h>
#include <sprite.h>

extern std::unordered_map<uint32_t, SpriteRemapData> g_sprite_remap;

struct CSprite
{
	virtual void free() = 0;

	int data[19];
	uint32_t image_width;
	uint32_t image_height;
	uint32_t origin_x;
	uint32_t origin_y;
	uint32_t unknown_0;
	uint32_t unknown_1;
	uint32_t sprite_id;

	void Draw(float p1, float p2, float p3, float p4, float p5, float p6, uint32_t p7, float p8);
	void DrawSimple(int p1, float p2, float p3);
	void DrawStretched(int p1, float p2, float p3, float p4, float p5, uint32_t p6, float p7);
	void DrawPos(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11);
	void DrawTile(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11);
	void DrawTiled(int p1, float p2, float p3, float p4, float p5, bool p6, bool p7, float p8, float p9, float p10, float p11, uint32_t p12, float p13);
};

CSprite* (__cdecl* GetSpriteFromIndex)(uint32_t index) = nullptr;

void(__thiscall* CSprite__Draw_orig)(CSprite* this_sprite, float p1, float p2, float p3, float p4, float p5, float p6, uint32_t p7, float p8) = nullptr;
void(__thiscall* CSprite__DrawSimple_orig)(CSprite* this_sprite, int p1, float p2, float p3) = nullptr;
void(__thiscall* CSprite__DrawStretched_orig)(CSprite* this_sprite, int p1, float p2, float p3, float p4, float p5, uint32_t p6, float p7) = nullptr;
void(__thiscall* CSprite__DrawPos_orig)(CSprite* this_sprite, int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11) = nullptr;
void(__thiscall* CSprite__DrawTile_orig)(CSprite* this_sprite, int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11) = nullptr;
void(__thiscall* CSprite__DrawTiled_orig)(CSprite* this_sprite, int p1, float p2, float p3, float p4, float p5, bool p6, bool p7, float p8, float p9, float p10, float p11, uint32_t p12, float p13) = nullptr;

PFUNC_YYGMLScript AdvanceGameFrame_orig = nullptr;

CSprite* get_remapped_sprite(uint32_t sprite_id)
{
	if (g_sprite_remap.contains(sprite_id))
	{
		auto& remap = g_sprite_remap[sprite_id];

		if (remap.loaded == false)
		{
			load_sprite_to_memory(&remap);
		}

		auto now = std::chrono::high_resolution_clock::now();
		remap.last_draw_time = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		CSprite* overwriting_csprite = GetSpriteFromIndex(remap.sprite_index);

		if (overwriting_csprite == nullptr)
		{
			loader_log_warn("get_remapped_sprite(): GetSpriteFromIndex(overwriting_sprite_id) == nullptr (id = \"{}\")", remap.sprite_index);
		}

		return overwriting_csprite;
	}
	return nullptr;
}

void __thiscall CSprite::Draw(float p1, float p2, float p3, float p4, float p5, float p6, uint32_t p7, float p8)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__Draw_orig(custom_sprite, p1, p2, p3, p4, p5, p6, p7, p8);
	}

	CSprite__Draw_orig(this, p1, p2, p3, p4, p5, p6, p7, p8);
}

void __thiscall CSprite::DrawSimple(int p1, float p2, float p3)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__DrawSimple_orig(custom_sprite, p1, p2, p3);
	}

	CSprite__DrawSimple_orig(this, p1, p2, p3);
}

void __thiscall CSprite::DrawStretched(int p1, float p2, float p3, float p4, float p5, uint32_t p6, float p7)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__DrawStretched_orig(custom_sprite, p1, p2, p3, p4, p5, p6, p7);
	}

	CSprite__DrawStretched_orig(this, p1, p2, p3, p4, p5, p6, p7);
}

void __thiscall CSprite::DrawPos(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__DrawPos_orig(custom_sprite, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
	}

	CSprite__DrawPos_orig(this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
}

void __thiscall CSprite::DrawTile(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__DrawTile_orig(custom_sprite, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
	}

	CSprite__DrawTile_orig(this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
}

void __thiscall CSprite::DrawTiled(int p1, float p2, float p3, float p4, float p5, bool p6, bool p7, float p8, float p9, float p10, float p11, uint32_t p12, float p13)
{
	CSprite* custom_sprite = get_remapped_sprite(this->sprite_id);

	if (custom_sprite)
	{
		return CSprite__DrawTiled_orig(custom_sprite, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
	}

	CSprite__DrawTiled_orig(this, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
}

std::chrono::steady_clock::time_point interval;
RValue& AdvanceGameFrame_detour(CInstance* self, CInstance* other, RValue* out, int argc, RValue** argv)
{
	using namespace std::chrono_literals;

	auto now = std::chrono::high_resolution_clock::now();

	if (now - interval >= 5s)
	{
		for (auto& entry : g_sprite_remap)
		{
			auto& remap_data = entry.second;

			if (remap_data.loaded && (duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() - remap_data.last_draw_time) > 10000)
			{
				unload_sprite_from_memory(&remap_data);
			}
		}
		interval = now;
	}

	return AdvanceGameFrame_orig(self, other, out, argc, argv);
}

void hook_scan(void* detour, void** original, const std::vector<uint8_t>& pattern)
{
	void* loc = loader_search_memory(pattern);

	if (loc != nullptr)
	{
		loader_hook_create(loc, detour, original);
		loader_hook_enable(loc);
	}
	else
	{
		loader_log_warn("hook_scan failed to find a pattern!!");
	}
}

void init_draw_command_hooks()
{
	union
	{
		void* csd;
		void (CSprite::* DrawHook)(float p1, float p2, float p3, float p4, float p5, float p6, uint32_t p7, float p8);
		void (CSprite::* DrawSimpleHook)(int p1, float p2, float p3);
		void (CSprite::* DrawStretchedHook)(int p1, float p2, float p3, float p4, float p5, uint32_t p6, float p7);
		void (CSprite::* DrawPosHook)(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11);
		void (CSprite::* DrawTileHook)(int p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, uint32_t p10, float p11);
		void (CSprite::* DrawTiledHook)(int p1, float p2, float p3, float p4, float p5, bool p6, bool p7, float p8, float p9, float p10, float p11, uint32_t p12, float p13);
	};

	DrawHook = &CSprite::Draw;
	hook_scan(csd, reinterpret_cast<void**>(&CSprite__Draw_orig), { 0x55, 0x8b, 0xec, 0x6a, 0xff, 0x68, '?', '?', '?', '?', 0x64, 0xa1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x83, 0xec, 0x18, 0x56, 0x57, 0xa1, '?', '?', '?', '?', 0x33, 0xc5, 0x50, 0x8d, 0x45, '?', 0x64, 0xa3, 0x00, 0x00, 0x00, 0x00, 0x8b, 0xf1, 0x83, 0x7e, '?', '?', 0x0f, 0x8e, '?', '?', '?', '?' });
	DrawStretchedHook = &CSprite::DrawStretched;
	hook_scan(csd, reinterpret_cast<void**>(&CSprite__DrawStretched_orig), { 0x56, 0x8b, 0xf1, 0x8b, 0x4e, 0x4c, 0x85, 0xc9, 0x0f, 0x8e, 0xf3, 0x00, 0x00, 0x00, 0xf3, 0x0f, 0x10, 0x4c, 0x24, 0x14, 0x0f, 0x57, 0xc0, 0x0f, 0x2f, 0xc1, 0x0f, 0x83, 0xe1, 0x00, 0x00, 0x00 });
	DrawPosHook = &CSprite::DrawPos;
	hook_scan(csd, reinterpret_cast<void**>(&CSprite__DrawPos_orig), { 0x56, 0x57, 0x8b, 0xf9, 0x8b, 0x77, 0x4c, 0x85, 0xf6, 0x0f, 0x8e, 0xaf, 0x00, 0x00, 0x00, 0x8b, 0x44, 0x24, 0x0c, 0x99, 0xf7, 0xfe, 0x85, 0xd2, 0x79, 0x02, 0x03, 0xd6, 0x8b, 0x47, 0x1c, 0x85, 0xc0, 0x0f, 0x84, 0x97, 0x00, 0x00, 0x00, 0xf3, 0x0f, 0x10, 0x44, 0x24, 0x34 });
	DrawTileHook = &CSprite::DrawTile;
	hook_scan(csd, reinterpret_cast<void**>(&CSprite__DrawTile_orig), { 0x56, 0x8b, 0xf1, 0x8b, 0x4e, 0x4c, 0x85, 0xc9, 0x0f, 0x8e, '?', '?', '?', '?', 0x8b, 0x44, 0x24, 0x08, 0x99, 0xf7, 0xf9, 0x85, 0xd2, 0x79, '?', 0x03, 0xd1 });
	DrawTiledHook = &CSprite::DrawTiled;
	hook_scan(csd, reinterpret_cast<void**>(&CSprite__DrawTiled_orig), { 0x56, 0x8b, 0xf1, 0x8b, 0x4e, 0x4c, 0x85, 0xc9, 0x0f, 0x8e, '?', '?', '?', '?', 0x8b, 0x44, 0x24, 0x08, 0x99, 0xf7, 0xf9, 0x85, 0xd2, 0x79, '?', 0x03, 0xd1, 0xf3, 0x0f, 0x10, 0x44, 0x24, 0x38, 0x51, 0xf3, 0x0f, 0x11, 0x04, 0x24, 0xff, 0x74, 0x24, 0x38, 0xf3, 0x0f, 0x10, 0x44, 0x24, 0x38 });

	// search for CSprite::DrawSimple()
	DrawSimpleHook = &CSprite::DrawSimple;
	void* loc = loader_search_memory({ 0xf3, 0x0f, 0x10, 0x44, 0x24, '?', 0x83, 0xec, '?', 0x8b, 0xc8, 0xf3, 0x0f, 0x11, 0x44, 0x24, '?', 0xf3, 0x0f, 0x10, 0x44, 0x24, '?', 0xf3, 0x0f, 0x11, 0x04, 0x24 });

	if (loc != nullptr)
	{
		loc = loader_search_memory_local({ 0xe8, '?', '?', '?', '?', 0x5e }, loc, 0x100);
	}
	if (loc != nullptr)
	{
		auto func_addr = ((uint32_t)loc + *(int32_t*)INCREMENT_POINTER(loc, 1)) + 5;
		loader_hook_create((void*)func_addr, csd, reinterpret_cast<void**>(&CSprite__DrawSimple_orig));
		loader_hook_enable((void*)func_addr);
	}
	// search for GetSpriteFromIndex()
	loc = loader_search_memory({ 0x0f, 0x10, 0x87, '?', 0x00, 0x00, 0x00, 0x8b, 0x87, '?', '?', 0x00, 0x00, 0xc7, 0x45, '?', 0x00, 0x00, 0x00, 0x00, 0x0f, 0x11, 0x45, '?', 0x85, 0xc0, 0x78, '?', 0x50, 0xeb, '?' });

	if (loc != nullptr)
	{
		loc = loader_search_memory_local({ 0xe8, '?', '?', '?', '?', 0x8b, 0xd8 }, loc, 0x100);
	}
	if (loc != nullptr)
	{
		auto func_addr = ((uint32_t)loc + *(int32_t*)INCREMENT_POINTER(loc, 1)) + 5;

		GetSpriteFromIndex = (CSprite * (__cdecl*)(uint32_t))func_addr;
	}

	void* gml_Script_AdvanceGameFrame = loader_get_script("gml_Script_AdvanceGameFrame");
	loader_hook_create(gml_Script_AdvanceGameFrame, &AdvanceGameFrame_detour, reinterpret_cast<void**>(&AdvanceGameFrame_orig));
	loader_hook_enable(gml_Script_AdvanceGameFrame);
}