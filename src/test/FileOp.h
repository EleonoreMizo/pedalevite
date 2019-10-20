/*****************************************************************************

        FileOp.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (FileOp_HEADER_INCLUDED)
#define FileOp_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>

#include <cstdint>



class FileOp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     load_wav (const char *filename_0, std::vector <std::vector <float> > &chn_arr, double &sample_freq);
	static int     save_wav (const char *filename_0, const std::vector <float> &chn, double sample_freq, float scale = 1);
	static int     save_wav (const char *filename_0, const std::vector <std::vector <float> > &chn_arr, double sample_freq, float scale = 1);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	struct WavRiff
	{
		char           _chunk_id [4] = { 'R', 'I', 'F', 'F' };  // "RIFF"
		uint32_t       _chunk_size;
		char           _wave_id [4] = { 'W', 'A', 'V', 'E' };   // "WAVE"
	};

	struct WavFmt
	{
		char           _chunk_id [4] = { 'f', 'm', 't', ' ' };  // "fmt "
		uint32_t       _chunk_size;
		int16_t        _format_tag;
		uint16_t       _channels;
		uint32_t       _samples_per_sec;
		uint32_t       _avg_bytes_per_sec;
		uint16_t       _block_align;
		uint16_t       _bits_per_sample;

		uint16_t       _size;
		uint16_t       _valid_bits_per_sample;
		uint32_t       _channel_mask;
		uint8_t        _subformat [16];
	};

	struct WavData
	{
		char           _chunk_id [4] = {'d', 'a', 't', 'a' };  // "data"
		uint32_t       _chunk_size;
	};

	enum WavFormat : uint16_t
	{
 		WavFormat_PCM        = 0x0001,
 		WavFormat_IEEE_FLOAT = 0x0003,
 		WavFormat_ALAW		   = 0x0006,
 		WavFormat_MULAW	   = 0x0007,
 		WavFormat_EXTENSIBLE = 0xFFFE
	};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FileOp ()                               = delete;
	               FileOp (const FileOp &other)            = delete;
	virtual        ~FileOp ()                              = delete;
	FileOp &       operator = (const FileOp &other)        = delete;
	bool           operator == (const FileOp &other) const = delete;
	bool           operator != (const FileOp &other) const = delete;

}; // class FileOp



//#include "FileOp.hpp"



#endif   // FileOp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
