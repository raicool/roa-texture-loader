#include "config.h"

#include "mod.h"
#include "yaml_helper.h"

#include <string>
#include <format>
#include <any>
#include <filesystem>
#include <fstream>

std::vector<config_entry> config =
{
	{"save_original_sprites", "enables the ability to restore original sprites after overwriting by duplicating the sprites into memory (takes up a lot of memory)", false}
};

void load_config()
{
	std::string config_file_path = std::format("mods/{}/config.ini", MOD_NAME);
	if (file_exists(config_file_path))
	{
		YAML::Node config_yaml = YAML::LoadFile(config_file_path);

		config[SAVE_ORIGINAL_SPRITES].value = get_nodeleaf_safe(
			config_yaml,
			"save_original_sprites",
			false
		).as<bool>();
	}
}

void save_config()
{
	YAML::Node config_yaml;

	for (auto& entry : config)
	{
		config_yaml[entry.identifier] = entry.value;
	}

	std::filesystem::create_directory(std::format("mods/{}", MOD_NAME));
	std::string config_file_path = std::format("mods/{}/config.ini", MOD_NAME);
	std::ofstream out_file(config_file_path, std::ios::out);
	out_file << config_yaml;
	out_file.close();
}

void set_config_entry(uint8_t entry, bool value)
{
	config[entry].value = value;
}

config_entry* get_config_entry(uint8_t entry)
{
	if (config.size() <= entry)
	{
		return &config[entry];
	}
}