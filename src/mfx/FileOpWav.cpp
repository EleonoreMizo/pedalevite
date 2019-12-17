/*****************************************************************************

        FileOpWav.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/FileOpWav.h"

#include <algorithm>
#include <array>

#include <cassert>
#include <climits>
#include <cmath>
#include <cstddef>




namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FileOpWav::~FileOpWav ()
{
	if (_f_ptr != 0)
	{
		close_file ();
	}
}



// Creates a new empty wav file, to be populated with write_data ().
// Then call close_file () when you're done.
int	FileOpWav::create_save (const char *filename_0, int nbr_chn, double sample_freq, int64_t max_len)
{
	assert (! is_open ());

	int            ret_val = 0;

	_f_ptr = fstb::fopen_utf8 (filename_0, "wb");
	if (_f_ptr == 0)
	{
		ret_val = -1;
	}
	else
	{
		_max_len     = max_len;
		_nbr_chn     = nbr_chn;
		_sample_freq = sample_freq;
		_len         = 0;
		_pos         = 0;
		_max_nbr_frm = _tmp_buf_len / nbr_chn;

		ret_val = write_headers (_f_ptr, _nbr_chn, _len, _sample_freq);
		_data_beg = ftell (_f_ptr);
	}

	return ret_val;
}



int	FileOpWav::write_data (const float * const chn_arr [], int nbr_spl)
{
	assert (chn_arr != 0);
	assert (nbr_spl > 0);

	int            ret_val = 0;
	
	if (! is_open ())
	{
		ret_val = -1;
		assert (false);
	}

	else
	{
		std::array <float, _tmp_buf_len> tmp_buf;

		int            pos = 0;
		do
		{
			const int      work_len =
				std::min (nbr_spl - pos, _max_nbr_frm);
			if (_pos + work_len > _max_len)
			{
				ret_val = fseek (_f_ptr, _data_beg, SEEK_SET);
				_pos = 0;
			}

			if (ret_val == 0)
			{
				if (_nbr_chn == 2)
				{
					dsp::mix::Generic::copy_2_2i (
						&tmp_buf [0], chn_arr [0] + pos, chn_arr [1] + pos, work_len
					);
				}
				else
				{
					for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
					{
						assert (chn_arr [chn_cnt] != 0);
						dsp::mix::Generic::copy_1_ni1 (
							&tmp_buf [chn_cnt],
							chn_arr [chn_cnt] + pos,
							work_len,
							_nbr_chn
						);
					}
				}
		
				const size_t   write_len   = _nbr_chn * work_len;
				const size_t   written_len = fwrite (
					&tmp_buf [0], sizeof (tmp_buf [0]), write_len, _f_ptr
				);
				if (written_len != write_len)
				{
					ret_val = -1;
				}
				else
				{
					_pos += work_len;
					_len  = std::max (_len, _pos);
					pos  += work_len;
				}
			}
		}
		while (pos < nbr_spl && ret_val == 0);
	}

	return ret_val;
}



// Interleaved data. nbr_spl = number of multichannel frames
int	FileOpWav::write_data (const float frame_arr_ptr [], int nbr_spl)
{
	assert (frame_arr_ptr != 0);
	assert (nbr_spl > 0);

	int            ret_val = 0;

	if (! is_open ())
	{
		ret_val = -1;
		assert (false);
	}

	else
	{
		if (_pos + nbr_spl > _max_len)
		{
			ret_val = fseek (_f_ptr, _data_beg, SEEK_SET);
			_pos = 0;
		}

		if (ret_val == 0)
		{
			const size_t   write_len   = _nbr_chn * nbr_spl;
			const size_t   written_len = fwrite (
				&frame_arr_ptr [0], sizeof (frame_arr_ptr [0]), write_len, _f_ptr
			);
			if (written_len != write_len)
			{
				ret_val = -1;
			}
			else
			{
				_pos += nbr_spl;
				_len  = std::max (_len, _pos);
			}
		}
	}

	return ret_val;
}



int	FileOpWav::close_file ()
{
	int            ret_val = 0;

	if (! is_open ())
	{
		ret_val = -1;
		assert (false);
	}
	else
	{
		ret_val = write_headers (_f_ptr, _nbr_chn, _len, _sample_freq);
		fclose (_f_ptr);
		_f_ptr = 0;
	}

	return ret_val;
}



bool	FileOpWav::is_open () const
{
	return (_f_ptr != 0);
}



int64_t	FileOpWav::get_size_frames () const
{
	return _len;
}



int	FileOpWav::load (const char *filename_0, std::vector <std::vector <float> > &chn_arr, double &sample_freq)
{
	int            ret_val = 0;

	chn_arr.clear ();
	sample_freq = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Loads the file into memory

	FILE *         f_ptr = fstb::fopen_utf8 (filename_0, "rb");
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



int	FileOpWav::save (const char *filename_0, const std::vector <float> &chn, double sample_freq, float scale)
{
	const float * const chn_arr [1] = { chn.data () };

	return save (filename_0, chn_arr, chn.size (), 1, sample_freq, scale);
}



int	FileOpWav::save (const char *filename_0, const std::vector <std::vector <float> > &chn_arr, double sample_freq, float scale)
{
	assert (! chn_arr.empty ());
	assert (sample_freq > 0);

	const size_t   nbr_spl = chn_arr [0].size ();
	const int      nbr_chn = int (chn_arr.size ());

	std::vector <const float *> chn_ptr_arr;
	for (auto &chn : chn_arr)
	{
		assert (chn.size () == nbr_spl);
		chn_ptr_arr.push_back (chn.data ());
	}

	return save (
		filename_0, chn_ptr_arr.data (), nbr_spl, nbr_chn, sample_freq, scale
	);
}



int	FileOpWav::save (const char *filename_0, const float * const chn_arr [], size_t nbr_spl, int nbr_chn, double sample_freq, float scale)
{
	assert (filename_0 != 0);
	assert (chn_arr != 0);
	assert (nbr_chn > 0);
	assert (sample_freq > 0);

	int            ret_val = 0;

	FILE *         f_ptr = fstb::fopen_utf8 (filename_0, "wb");
	if (f_ptr == 0)
	{
		ret_val = -1;
	}

	if (ret_val == 0)
	{
		ret_val = write_headers (f_ptr, nbr_chn, nbr_spl, sample_freq);
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



int	FileOpWav::write_headers (FILE * f_ptr, int nbr_chn, int64_t nbr_spl, double sample_freq)
{
	assert (f_ptr != 0);
	assert (nbr_chn > 0);
	assert (nbr_spl >= 0);
	assert (sample_freq > 0);

	int            ret_val = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Prepares the chunks

	const int      header_len = 8;

	WavFmt         fmt;
	fmt._chunk_size        = offsetof (WavFmt, _size) - header_len;
	fmt._format_tag        = WavFormat_IEEE_FLOAT;
	fmt._channels          = uint16_t (nbr_chn);
	fmt._samples_per_sec   = uint32_t (floor (sample_freq + 0.5f));
	fmt._block_align       = sizeof (float) * nbr_chn;
	fmt._avg_bytes_per_sec = fmt._block_align * fmt._samples_per_sec;
	fmt._bits_per_sample   = sizeof (float) * CHAR_BIT;

	WavData        data;
	// Value can be wrong here if nbr_spl is very large, but this is a known
	// issue with WAV files. Most file readers have a flag to ignore the length
	// specified in the chunks and use the actual file length.
	data._chunk_size = uint32_t (fmt._block_align * nbr_spl);
	assert ((data._chunk_size & 1) == 0);

	WavRiff        riff;
	riff._chunk_size = 4 + header_len * 2 + fmt._chunk_size + data._chunk_size;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Writes the file

	if (ret_val == 0)
	{
		ret_val = fseek (f_ptr, 0, SEEK_SET);
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

	return ret_val;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
