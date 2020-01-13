/*****************************************************************************

        FileOpWav.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_FileOpWav_HEADER_INCLUDED)
#define mfx_FileOpWav_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>

#include <cstdint>
#include <cstdio>



namespace mfx
{



class FileOpWav
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FileOpWav () = default;
	virtual        ~FileOpWav ();

	int            create_save (const char *filename_0, int nbr_chn, double sample_freq, int64_t max_len = INT64_MAX);
	int            write_data (const float * const chn_arr [], int nbr_spl);
	int            write_data (const float frame_arr_ptr [], int nbr_spl);
	int            close_file ();
	bool           is_open () const;
	int64_t        get_size_frames () const;

	static int     load (const char *filename_0, std::vector <std::vector <float> > &chn_arr, double &sample_freq);
	static int     save (const char *filename_0, const std::vector <float> &chn, double sample_freq, float scale = 1);
	static int     save (const char *filename_0, const std::vector <std::vector <float> > &chn_arr, double sample_freq, float scale = 1);
	static int     save (const char *filename_0, const float * const chn_arr [], size_t nbr_spl, int nbr_chn, double sample_freq, float scale = 1);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _tmp_buf_len = 1024; // In single samples

	struct WavRiff
	{
		char           _chunk_id [4] = { 'R', 'I', 'F', 'F' };  // "RIFF"
		uint32_t       _chunk_size = 0;
		char           _wave_id [4] = { 'W', 'A', 'V', 'E' };   // "WAVE"
	};

	struct WavFmt
	{
		char           _chunk_id [4] = { 'f', 'm', 't', ' ' };  // "fmt "
		uint32_t       _chunk_size = 0;
		int16_t        _format_tag = 0;
		uint16_t       _channels = 0;
		uint32_t       _samples_per_sec = 0;
		uint32_t       _avg_bytes_per_sec = 0;
		uint16_t       _block_align = 0;
		uint16_t       _bits_per_sample = 0;

		uint16_t       _size = 0;
		uint16_t       _valid_bits_per_sample = 0;
		uint32_t       _channel_mask = 0;
		uint8_t        _subformat [16] = { };
	};

	struct WavData
	{
		char           _chunk_id [4] = {'d', 'a', 't', 'a' };  // "data"
		uint32_t       _chunk_size = 0;
	};

	enum WavFormat : uint16_t
	{
 		WavFormat_PCM        = 0x0001,
 		WavFormat_IEEE_FLOAT = 0x0003,
 		WavFormat_ALAW		   = 0x0006,
 		WavFormat_MULAW	   = 0x0007,
 		WavFormat_EXTENSIBLE = 0xFFFE
	};

	static int     write_headers (FILE * f_ptr, int nbr_chn, int64_t nbr_spl, double sample_freq);

	FILE *         _f_ptr         = nullptr; // 0 if not open
	int            _nbr_chn       = 0;
	double         _sample_freq   = 0;
	int64_t        _max_len       = 0; // Sample frames
	int64_t        _len           = 0; // Length of the file, sample frames
	int64_t        _pos           = 0; // Position within the file, sample frames
	int            _max_nbr_frm   = 0; // Sample frames, in the temporary buffer
	long           _data_beg      = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FileOpWav (const FileOpWav &other)         = delete;
	               FileOpWav (const FileOpWav &&other)        = delete;
	FileOpWav &    operator = (const FileOpWav &other)        = delete;
	FileOpWav &    operator = (const FileOpWav &&other)       = delete;
	bool           operator == (const FileOpWav &other) const = delete;
	bool           operator != (const FileOpWav &other) const = delete;

}; // class FileOpWav



}  // namespace mfx



//#include "mfx/FileOpWav.hpp"



#endif   // mfx_FileOpWav_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
