/*****************************************************************************

        FileOp.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "FileOp.h"

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	FileOp::load_wav (const char *filename_0, std::vector <std::vector <float> > &chn_arr, double &sample_freq)
{
	int            ret_val = 0;

	chn_arr.clear ();
	sample_freq = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Loads the file into memory

	FILE *         f_ptr = fopen (filename_0, "rb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	std::vector <uint8_t>   content;
	if (ret_val == 0)
	{
		fseek (f_ptr, 0, SEEK_END);
		const long     file_size = ftell (f_ptr);
		fseek (f_ptr, 0, SEEK_SET);
		content.resize (file_size);
		if (fread (&content [0], file_size, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Converts the WAV file

	// Checks the header
	const WavRiff *   riff_ptr = 0;
	if (ret_val == 0)
	{
		riff_ptr = reinterpret_cast <const WavRiff *> (&content [0]);
		if (   riff_ptr->_chunk_id [0] != 'R'
		    || riff_ptr->_chunk_id [1] != 'I'
		    || riff_ptr->_chunk_id [2] != 'F'
		    || riff_ptr->_chunk_id [3] != 'F'
		    || riff_ptr->_wave_id [0] != 'W'
		    || riff_ptr->_wave_id [1] != 'A'
		    || riff_ptr->_wave_id [2] != 'V'
		    || riff_ptr->_wave_id [3] != 'E')
		{
			ret_val = -1;
		}
	}

	// Finds the significant chunks
	size_t         pos_fmt  = 0;
	size_t         pos_data = 0;
	if (ret_val == 0)
	{
		size_t         pos = 12;
		while (pos + 8 < content.size () && (pos_fmt == 0 || pos_data == 0))
		{
			if (   content [pos    ] == 'f'
			    && content [pos + 1] == 'm'
			    && content [pos + 2] == 't'
			    && content [pos + 3] == ' ')
			{
				pos_fmt = pos;
			}
			if (   content [pos    ] == 'd'
			    && content [pos + 1] == 'a'
			    && content [pos + 2] == 't'
			    && content [pos + 3] == 'a')
			{
				pos_data = pos;
			}

			const size_t   jmp = 
				   size_t (content [pos + 4])
				+ (size_t (content [pos + 5]) <<  8)
				+ (size_t (content [pos + 6]) << 16)
				+ (size_t (content [pos + 7]) << 24);
			pos += 8 + jmp;
		}
		if (pos_fmt == 0 || pos_data == 0)
		{
			ret_val = -1;
		}
	}

	// Checks the format
	const WavFmt * fmt_ptr = 0;
	int            format  = 0;
	if (ret_val == 0)
	{
		fmt_ptr = reinterpret_cast <const WavFmt *> (&content [pos_fmt]);
		format  = fmt_ptr->_format_tag;
		if (format == WavFormat_EXTENSIBLE)
		{
			format = fmt_ptr->_subformat [0] + (fmt_ptr->_subformat [1] << 8);
		}
		if (   format != WavFormat_PCM
		    && format != WavFormat_IEEE_FLOAT)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fmt_ptr->_bits_per_sample > 32)
		{
			ret_val = -1;
		}
	}

	// Loads and converts the sample data
	const WavData *   data_ptr = 0;
	if (ret_val == 0)
	{
		const int      nbr_chn = fmt_ptr->_channels;
		chn_arr.resize (nbr_chn);
		sample_freq = fmt_ptr->_samples_per_sec;
		const int      bytes_per_frame = fmt_ptr->_block_align;
		const int      bitdepth = fmt_ptr->_bits_per_sample;
		const int      bytes_per_spl   = (bitdepth + 7) / 8;
		const bool     float_flag = (fmt_ptr->_format_tag == WavFormat_IEEE_FLOAT);

		data_ptr = reinterpret_cast <const WavData *> (&content [pos_data]);
		const size_t   nbr_spl = data_ptr->_chunk_size / bytes_per_frame;
		const uint8_t* spl_ptr = &content [pos_data + 8];
		for (auto &chn_data : chn_arr)
		{
			chn_data.resize (nbr_spl);
		}

		for (size_t pos = 0; pos < nbr_spl; ++pos)
		{
			for (int chn = 0; chn < nbr_chn; ++chn)
			{
				const uint8_t* ptr =
					  spl_ptr
					+ pos * bytes_per_frame
					+ chn * bytes_per_spl;
				float				val = 0;
				if (float_flag)
				{
					val = *reinterpret_cast <const float *> (ptr);
				}
				else
				{
					for (int b = 0; b < bytes_per_spl; ++b)
					{
						val += ptr [b];
						val *= 1.0f / 256;
					}
					if (val >= 0.5f)
					{
						val -= 1;
					}
					val *= 2;
				}
				chn_arr [chn] [pos] = val;
			}
		}
	}

	return ret_val;
}



int	FileOp::save_wav (const char *filename_0, const std::vector <float> &chn, double sample_freq, float scale)
{
	const std::vector <std::vector <float> >  chn_arr (1, chn);

	return save_wav (filename_0, chn_arr, sample_freq, scale);
}



int	FileOp::save_wav (const char *filename_0, const std::vector <std::vector <float> > &chn_arr, double sample_freq, float scale)
{
	assert (! chn_arr.empty ());
	assert (sample_freq > 0);

	int            ret_val = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Prepares the chunks

	const size_t   nbr_spl = chn_arr [0].size ();
	const int      nbr_chn = int (chn_arr.size ());
#if ! defined (NDEBUG)
	for (auto &chn : chn_arr)
	{
		assert (chn.size () == nbr_spl);
	}
#endif // NDEBUG
	const int      header_len = 8;

	WavFmt         fmt { { 'f', 'm', 't', ' ' } };
	fmt._chunk_size        = offsetof (WavFmt, _size) - header_len;
	fmt._format_tag        = WavFormat_IEEE_FLOAT;
	fmt._channels          = uint16_t (chn_arr.size ());
	fmt._samples_per_sec   = uint32_t (floor (sample_freq + 0.5f));
	fmt._block_align       = sizeof (float) * nbr_chn;
	fmt._avg_bytes_per_sec = fmt._block_align * fmt._samples_per_sec;
	fmt._bits_per_sample   = sizeof (float) * CHAR_BIT;

	WavData        data { { 'd', 'a', 't', 'a' } };
	data._chunk_size = fmt._block_align * nbr_spl;
	assert ((data._chunk_size & 1) == 0);

	WavRiff        riff { { 'R', 'I', 'F', 'F' }, 0, { 'W', 'A', 'V', 'E' } };
	riff._chunk_size = 4 + header_len * 2 + fmt._chunk_size + data._chunk_size;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Writes the file

	FILE *         f_ptr = fopen (filename_0, "wb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		if (fwrite (&riff, header_len + 4, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fwrite (&fmt, header_len + fmt._chunk_size, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	if (ret_val == 0)
	{
		if (fwrite (&data, header_len, 1, f_ptr) != 1)
		{
			ret_val = -1;
		}
	}
	std::vector <float> tmp (nbr_chn);
	for (size_t pos = 0; pos < nbr_spl && ret_val == 0; ++pos)
	{
		for (int chn = 0; chn < nbr_chn; ++chn)
		{
			tmp [chn] = chn_arr [chn] [pos] * scale;
		}
		if (fwrite (&tmp [0], sizeof (tmp [0]), nbr_chn, f_ptr) != size_t (nbr_chn))
		{
			ret_val = -1;
		}
	}

	if (f_ptr != 0)
	{
		fclose (f_ptr);
		f_ptr = 0;
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
