#pragma once

#include "pack.h"
#include <iterator>

#define SPRITE_DIRECTORY "custom_sprites/"

struct pack_queue
{
	std::vector<std::shared_ptr<pack>> data;
	using pack_ptr = std::shared_ptr<pack>;
	void emplace(const pack_ptr& _new_data)
	{
		data.emplace_back(_new_data);
	}

	void raise(pack_ptr _pack)
	{
		for (int i = data.size() - 1; i > 0; i--)
			if (data[i] == _pack)
			{
				std::swap(data[i], data[i - 1]);
				sanitize();
				break;
			}
	}

	void lower(pack_ptr _pack)
	{
		for (int i = 0; i < data.size() - 1; i++)
			if (data[i] == _pack)
			{
				std::swap(data[i], data[i + 1]);
				sanitize();
				break;
			}
	}

	void sanitize()
	{
		for (int i = 0; i < data.size() - 1; i++)
		{
			data[i]->priority = i;
		}
	}

	inline auto begin() { return data.begin(); }
	inline auto rbegin() { return data.rbegin(); }
	inline auto end() { return data.end(); }
	inline auto rend() { return data.rend(); }
	inline void clear() { data.clear(); }
};

void update_pack_queue();
pack_queue* get_loaded_packs();
void apply_packs(bool play_apply_sound = false);