#pragma once

#include <any>
#include <string>

enum config_order
{
	SAVE_ORIGINAL_SPRITES = 0,
};

struct config_entry
{
	std::string identifier;
	std::string description; // for use in imgui

	bool value;
};

void load_config();
void save_config();
void set_config_entry(uint8_t entry, bool value);
config_entry* get_config_entry(uint8_t entry);