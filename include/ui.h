#pragma once

#include "pack_queue.h"

#include <memory>

void __setup_ui();
void render_ui();
struct pack;
void render_packdata(pack_queue&, const std::shared_ptr<pack>& packdata);
void render_table(pack_queue& pack_list);