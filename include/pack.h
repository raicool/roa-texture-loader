#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <thread>

struct ID3D11ShaderResourceView;
struct pack
{
	std::string name; //< name of the pack
	std::string author; //< pack creator
	std::string description; //< pack description
	uint32_t version; //< game version that pack is intended to be used with

	ID3D11ShaderResourceView* pack_img; //< pack image to be used in the texture pack menu

	std::filesystem::path sound_path; //< path to all sound files for texture pack
	std::filesystem::path sprite_path; //< path to all sprite files for texture pack
	std::filesystem::path shader_path; //< path to all shader files for texture pack
	std::filesystem::path pack_path; //< path to texture pack

	bool enabled; //< if pack should overwrite ingame textures
};

std::shared_ptr<pack> open_texture_pack(const std::filesystem::path& pack_dir);
void fetch_packimg();