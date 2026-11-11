#pragma once

#include "pack_queue.h"

#include <memory>

void setup_ui();
void render_ui();
struct TexturePack;
void render_packdata(TexturePackList&, const std::shared_ptr<TexturePack>& packdata);
void render_table(TexturePackList& pack_list);