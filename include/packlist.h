#pragma once

#include "pack.h"

#define SPRITE_DIRECTORY "custom_sprites/"

using packlist = std::vector<std::shared_ptr<pack>>;

void update_packlist();
void apply_packs(bool play_apply_sound = false);
packlist* get_packlist();