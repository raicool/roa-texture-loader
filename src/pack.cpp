#include "pack.h"

#include "yaml_helper.h"

#include <loader/log.h>
#include <loader/d3d11_hook.h>

#include <yaml-cpp/yaml.h>

#include <WICTextureLoader.h>

#include <fstream>
#include <filesystem>
#include <memory>
#include <format>
#include <queue>

struct img_request
{
	unsigned char* buffer;
	size_t length;
	ID3D11ShaderResourceView** shader_res;
};

std::mutex img_queue_mutex;
std::queue<img_request> img_buffer_queue;

using namespace std::literals::chrono_literals;

void fetch_packimg()
{
	std::unique_lock<std::mutex> lock(img_queue_mutex);
	while (!img_buffer_queue.empty())
	{
		img_request img = img_buffer_queue.front();
		img_buffer_queue.pop();
			
		auto d3d_device = loader_get_d3d_device();
		auto d3d_device_context = loader_get_d3d_device_context();
		while (!d3d_device && !d3d_device_context)
		{
			// wait for d3d device/context
			std::this_thread::sleep_for(500ms);
			d3d_device = loader_get_d3d_device();
			d3d_device_context = loader_get_d3d_device_context();
		}

		HRESULT hr;
		hr = CreateWICTextureFromMemory(
			d3d_device,
			d3d_device_context,
			&img.buffer[0],
			img.length,
			nullptr,
			img.shader_res,
			NULL
		);
	}
}

std::shared_ptr<pack> open_texture_pack(const std::filesystem::path& pack_dir)
{
	if (std::filesystem::is_directory(pack_dir) == false)
	{
		loader_log_warn("texture pack directory \"{}\" cannot be found.");
		return nullptr;
	}

	std::shared_ptr<pack> _pack = std::make_shared<pack>();

	if (file_exists(pack_dir.generic_string() + "/pack.ini"))
	{
		const YAML::Node config = YAML::LoadFile(pack_dir.generic_string() + "/pack.ini");

		std::string err_string = std::format("unable to open texture pack: {}", pack_dir.generic_string());
		if (!config)
		{
			err_string.append("\n\t^~~~~~ pack.ini could not be opened properly. does it exist?");
			loader_log_error(err_string);
			return nullptr;
		}

		// texture pack name
		_pack->name = get_nodeleaf_safe(
			config,
			"pack-name",
			"???").as<std::string>();

		_pack->description = get_nodeleaf_safe(
			config,
			"pack-description",
			"missing description").as<std::string>();

		_pack->author = get_nodeleaf_safe(
			config,
			"author",
			"").as<std::string>();
	}
	else
	{
		_pack->name = "???";
		_pack->description = "missing description";
		_pack->author = "";
	}

	std::basic_ifstream<uint8_t> pack_img(pack_dir.generic_string() + "/pack.png", std::ios::binary);

	if (pack_img.is_open())
	{
		pack_img.seekg(0, std::ios::end);
		int length = pack_img.tellg();
		pack_img.seekg(0, std::ios::beg);

		unsigned char* buffer = new unsigned char[length];
		pack_img.read(&buffer[0], length);
		pack_img.close();

		img_buffer_queue.push(img_request{ buffer, (size_t)length, &_pack->pack_img });
	}
	else
	{
		// TODO: placeholder pack img
	}

	_pack->pack_path = pack_dir.generic_string();
	_pack->sprite_path = pack_dir.generic_string() + "/spr/";
	_pack->shader_path = pack_dir.generic_string() + "/shader/";

	return _pack;
}