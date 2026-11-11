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

struct TextureThreadQueueData
{
	unsigned char* buffer;
	size_t length;
	ID3D11ShaderResourceView** shader_res;
};

std::mutex mlock;
std::queue<TextureThreadQueueData> texture_queue;
std::condition_variable cv;
bool terminating = false;

using namespace std::literals::chrono_literals;

// executed from texture fetch thread
void worker_thread_proc_image()
{
	std::unique_lock<std::mutex> lock(mlock);
	HRESULT hr;

	hr = CoInitialize(nullptr);

	if (hr != S_OK)
	{
		loader_log_warn("texture worker thread failed to call CoInitialize successfully");
		loader_log_warn("texture pack images will not render :((");
		return;
	}

	while (1)
	{
		cv.wait(lock, [] { return terminating == true || texture_queue.size() > 0; });

		if (terminating)
		{
			break;
		}

		auto d3d_device = loader_get_d3d_device();
		while (!d3d_device)
		{
			// wait for d3d device
			std::this_thread::sleep_for(500ms);
			d3d_device = loader_get_d3d_device();
		}

		TextureThreadQueueData img = texture_queue.front();
		texture_queue.pop();

		hr = CreateWICTextureFromMemory(
			d3d_device,
			nullptr,
			&img.buffer[0],
			img.length,
			nullptr,
			img.shader_res
		);

		if (hr != S_OK)
		{
			loader_log_warn("error while trying to create texture");
			loader_log_warn("error id: 0x{:08x}", (unsigned int)hr);
		}
	}
}
std::thread worker_thread(worker_thread_proc_image);

std::shared_ptr<TexturePack> open_texture_pack(const std::filesystem::path& pack_dir)
{
	if (std::filesystem::is_directory(pack_dir) == false)
	{
		loader_log_warn("texture pack directory \"{}\" cannot be found.");
		return nullptr;
	}

	std::shared_ptr<TexturePack> _pack = std::make_shared<TexturePack>();
	const std::string pack_ini_path = pack_dir.generic_string() + "/pack.ini";

	if (file_exists(pack_ini_path))
	{
		YAML::Node config;

		try
		{
			config = YAML::LoadFile(pack_ini_path);
		}
		catch (YAML::ParserException& _exc)
		{
			loader_log_error("overwrite_sprite_properties(): ParserException caught from \"{}\"", pack_ini_path);
			loader_log_error("\t^~~~~~~ {}", _exc.what());
			return nullptr;
		}

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
		return nullptr;
	}

	const std::string local_pref_path = pack_dir.generic_string() + "/local_pref.ini";
	if (file_exists(local_pref_path))
	{
		YAML::Node local_config;
		try
		{
			local_config = YAML::LoadFile(local_pref_path);
		}
		catch (YAML::ParserException& _exc)
		{
			loader_log_error("overwrite_sprite_properties(): ParserException caught from \"{}\"", local_pref_path);
			loader_log_error("\t^~~~~~~ {}", _exc.what());
			return nullptr;
		}

		std::string err_string = std::format("unable to open texture pack: {}", pack_dir.generic_string());
		if (!local_config)
		{
			err_string.append("\n\t^~~~~~ local_pref.ini could not be opened properly.");
			loader_log_error(err_string);
			return nullptr;
		}

		_pack->enabled = get_nodeleaf_safe(
			local_config,
			"enabled",
			false).as<bool>();
		
		_pack->priority = get_nodeleaf_safe(
			local_config,
			"priority",
			0).as<int16_t>();
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

		texture_queue.push(TextureThreadQueueData{ buffer, (size_t)length, &_pack->pack_img });
		cv.notify_all();
	}

	_pack->pack_path = pack_dir.generic_string();
	_pack->sound_path = pack_dir.generic_string() + "/sound/";
	_pack->sprite_path = pack_dir.generic_string() + "/spr/";
	_pack->shader_path = pack_dir.generic_string() + "/shader/";

	return _pack;
}

void TexturePack::save()
{
	const std::string path = pack_path.generic_string() + "/local_pref.ini";
	std::ofstream out_file(path, std::ios::out);

	YAML::Node config;

	config["enabled"] = this->enabled;
	config["priority"] = this->priority;

	out_file << config;
	out_file.close();
}

void terminate_texture_thread()
{
	terminating = true;
	cv.notify_all();

	if (worker_thread.joinable())
	{
		worker_thread.join();
	}
}