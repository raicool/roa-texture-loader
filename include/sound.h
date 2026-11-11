#pragma once

#include <filesystem>

struct TexturePack;

void setup_sound();
void destroy_sounds();
void load_sounds(TexturePack* pack, const std::vector<std::filesystem::path>& sound_paths);