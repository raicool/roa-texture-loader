#pragma once

#include "packlist.h"

#include <memory>

void setup_ui();
void render_ui();
struct pack;
void render_packdata(const std::shared_ptr<pack>& packdata);
void render_texturepack_table(packlist* pack_list);