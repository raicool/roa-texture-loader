#include "riff.h"

#include "loader/log.h"

riff* read_riff(const std::string path, bool load_data)
{
	FILE* waveform_file = fopen(path.c_str(), "rb");
	riff* riff_data = new riff();

	fseek(waveform_file, 0, SEEK_END);
	riff_data->file_size = ftell(waveform_file);
	fseek(waveform_file, 0, SEEK_SET);

	if (waveform_file == nullptr)
	{
		loader_log_warn("read_riff(): bad waveform file open (\"{}\")", path);
		return nullptr;
	}

	fread(&riff_data->master_block, 1, sizeof(master_header), waveform_file);
	fread(&riff_data->format_block, 1, sizeof(format_chunk_header), waveform_file);
	
	generic_header generic_subblock{};

	while (true)
	{
		fread(&generic_subblock, 1, sizeof(generic_header), waveform_file);

		if (*(unsigned int*)&generic_subblock.block_id == 0x61746164) // "data" sub block
		{
			riff_data->data_subblock = generic_subblock;
			riff_data->data_offset = ftell(waveform_file);

			if (load_data == false)
				break;

			char* sampledata = new char[riff_data->file_size];
			fseek(waveform_file, 0, SEEK_SET);
			fread(sampledata, 1, riff_data->file_size, waveform_file);

			riff_data->data = sampledata;
			break;
		}

		fseek(waveform_file, generic_subblock.block_size, SEEK_CUR);

		// lazy failsafe
		if (ftell(waveform_file) > 0xffff)
		{
			break;
		}
	}

	fclose(waveform_file);

	return riff_data;
}

void riff::free()
{
	if (data)
	{
		delete[] data;
		data = nullptr;
	}
}