#pragma once

#include "pack.h"

#define SPRITE_DIRECTORY "custom_sprites/"

using packlist = std::vector<std::shared_ptr<pack>>;

void update_packlist();
void apply_packs();
packlist* get_packlist();