#pragma once

#include "pack.h"
#include <iterator>

#define SPRITE_DIRECTORY "custom_sprites/"

struct TexturePackList
{
	std::vector<std::shared_ptr<TexturePack>> data;

	void emplace(const std::shared_ptr<TexturePack>& _new_data)
	{
		data.emplace_back(_new_data);
	}

	void raise(std::shared_ptr<TexturePack> _pack)
	{
		for (int i = data.size() - 1; i > 0; i--)
		{
			if (data[i] == _pack)
			{
				std::swap(data[i], data[i - 1]);
				sanitize();
				break;
			}
		}
	}

	void lower(std::shared_ptr<TexturePack> _pack)
	{
		for (int i = 0; i < data.size() - 1; i++)
		{
			if (data[i] == _pack)
			{
				std::swap(data[i], data[i + 1]);
				sanitize();
				break;
			}
		}
	}

	inline auto begin() { return data.begin(); }
	inline auto rbegin() { return data.rbegin(); }
	inline auto end() { return data.end(); }
	inline auto rend() { return data.rend(); }

	void sort()
	{
		std::sort(begin(), end(), [&](
			const std::shared_ptr<TexturePack>& a, 
			const std::shared_ptr<TexturePack>& b) 
			{
			return a->priority > b->priority;
			}
		);
	}

	void sanitize()
	{
		int16_t idx = 0;
		for (int i = data.size() - 1; i >= 0; i--)
		{
			std::shared_ptr<TexturePack> r = data[i];
			if (r->priority != idx)
			{
				r->priority = idx;
				r->save();
			}
			idx++;
		}
	}

	
	inline void clear() { data.clear(); }
};

TexturePackList* get_loaded_packs();

void update_pack_queue();
void apply_packs(bool play_apply_sound = false);