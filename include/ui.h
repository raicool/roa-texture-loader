#pragma once

#include <memory>

void setup_ui();
void render_ui();
struct pack;
void render_packdata(const std::shared_ptr<pack>& packdata);