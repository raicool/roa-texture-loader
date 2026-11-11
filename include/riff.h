#pragma once

#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct MasterHeader
{
	char magic[4];
	uint32_t file_size;
	uint32_t file_format_id;
};
static_assert(sizeof(MasterHeader) == 0x0c, "invalid master_header struct size");

struct FormatChunkHeader
{
	char block_id[4]; //< identifier (fmt#) with # being the id
	uint32_t block_size;
	uint16_t audio_format; //< 1: PCM, 3: ieee_float
	uint16_t channel_count;
	uint32_t sample_speed; //< sampling in Hz
	uint32_t byte_rate; //<
	uint16_t byte_per_block; //< sample_speed * bit_depth
	uint16_t bit_depth; //< bits per sample
};
static_assert(sizeof(FormatChunkHeader) == 0x18, "invalid FormatChunkHeader struct size");


struct GenericHeader
{
	char block_id[4];
	uint32_t block_size;
};
static_assert(sizeof(GenericHeader) == 0x08, "invalid GenericHeader struct size");
#pragma pack(pop)

struct Riff
{
	MasterHeader master_block;
	FormatChunkHeader format_block;
	GenericHeader data_subblock;
	
	size_t file_size;
	size_t data_offset;
	void* data;

	void free();
};

Riff* read_riff(const std::string path, bool load_data = true);