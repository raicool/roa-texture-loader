#include "pack.h"

#include <loader/log.h>
#include <loader/d3d11_hook.h>

#include <yaml-cpp/yaml.h>

#include <WICTextureLoader.h>

#include <fstream>
#include <filesystem>
#include <memory>
#include <format>

std::shared_ptr<pack> open_texture_pack(const std::filesystem::path& pack_dir)
{
	if (std::filesystem::is_directory(pack_dir) == false)
	{
		loader_log_warn("texture pack directory \"{}\" cannot be found.");
		return nullptr;
	}

	std::shared_ptr<pack> _pack = std::make_shared<pack>();

	const YAML::Node config = YAML::LoadFile(pack_dir.generic_string() + "/pack.ini");

	std::string err_string = std::format("unable to open texture pack: {}", pack_dir.generic_string());
	if (!config)
	{
		err_string.append("\n\t^~~~~~ pack.ini could not be opened properly. does it exist?");
		loader_log_error(err_string);
		return nullptr;
	}

	// texture pack name
	_pack->name = get_node_safe(
		config,
		"pack-name",
		"???").as<std::string>();

	_pack->description = get_node_safe(
		config,
		"pack-description",
		"missing description").as<std::string>();

	_pack->author = get_node_safe(
		config,
		"author",
		"").as<std::string>();

	std::basic_ifstream<uint8_t> pack_img(pack_dir.generic_string() + "/pack.png", std::ios::binary);

	if (pack_img.is_open())
	{
		pack_img.seekg(0, std::ios::end);
		int length = pack_img.tellg();
		pack_img.seekg(0, std::ios::beg);

		unsigned char* buffer = new unsigned char[length];
		pack_img.read(&buffer[0], length);
		pack_img.close();

		HRESULT hr;
		hr = CreateWICTextureFromMemory(
			loader_get_d3d_device(),
			loader_get_d3d_device_context(),
			&buffer[0],
			(size_t)length,
			nullptr,
			&_pack->pack_img,
			NULL
		);
	}
	else
	{
		// TODO: placeholder pack img
	}

	_pack->sprite_path = pack_dir.generic_string() + "/spr/";
	_pack->shader_path = pack_dir.generic_string() + "/shader/";

	return _pack;
}