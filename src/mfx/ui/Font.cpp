/*****************************************************************************

        Font.cpp
        Author: Laurent de Soras, 2016

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

#include "fstb/fnc.h"
#include "mfx/ui/Font.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace ui
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Font::init (int nbr_char, int char_w, int char_h, int char_per_row, int stride, const uint8_t pic_arr [], const char32_t unicode_arr [], int baseline, int max_val, bool copy_data_flag, int zoom_h, int zoom_v)
{
	assert (nbr_char > 0);
	assert (nbr_char <= 0x7FFF);
	assert (char_w > 0);
	assert (char_h > 0);
	assert (char_per_row > 0);
	assert (stride > 0);
	assert (pic_arr != 0);
	assert (unicode_arr != 0);
	assert (baseline > 0);
	assert (baseline <= char_h);
	assert (max_val > 0);
	assert (max_val <= 255);
	assert (zoom_h > 0);
	assert (zoom_v > 0);

	_nbr_char   = nbr_char;
	_baseline   = zoom_v * baseline;
	_bold_shift = zoom_h;
	_prop_flag  = false;
	if (copy_data_flag || max_val != 255 || zoom_h != 1 || zoom_v != 1)
	{
		_char_h       = char_h * zoom_v;
		_stride       = char_w * zoom_h;
		_data_arr.resize (_nbr_char * _stride * _char_h);
		int            pos_dst = 0;

		for (int c = 0; c < nbr_char; ++c)
		{
			const int      row     = c / char_per_row;
			const int      col     = c - row * char_per_row;
			int            pos_src = row * char_h * stride + col * char_w;
			for (int cy = 0; cy < char_h; ++cy)
			{
				if (zoom_h == 1)
				{
					for (int cx = 0; cx < char_w; ++cx)
					{
						_data_arr [pos_dst + cx] =
							uint8_t (pic_arr [pos_src + cx] * 255 / max_val);
					}
				}
				else
				{
					for (int cx = 0; cx < char_w; ++cx)
					{
						const uint8_t  v =
							uint8_t (pic_arr [pos_src + cx] * 255 / max_val);
						for (int zx = 0; zx < zoom_h; ++zx)
						{
							_data_arr [pos_dst + cx * zoom_h + zx] = v;
						}
					}
				}

				pos_dst += _stride;
				pos_src +=  stride;

				for (int zy = 1; zy < zoom_v; ++zy)
				{
					memcpy (
						&_data_arr [pos_dst          ],
						&_data_arr [pos_dst - _stride],
						_stride * sizeof (_data_arr [0])
					);
					pos_dst += _stride;
				}
			}
		}

		_data_ptr    = _data_arr.data ();
		char_per_row = 1;
	}
	else
	{
		_char_h      = char_h;
		_stride      = stride;
		PicData ().swap (_data_arr);
		_data_ptr    = pic_arr;
	}

	char_w *= zoom_h;
	char_h *= zoom_v;

	// Fills glyph information
	GlyphInfo      gi;
	gi._width = char_w;
	_glyph_arr.assign (nbr_char, gi);
	for (int c = 0; c < nbr_char; ++c)
	{
		const int      row     = c / char_per_row;
		const int      col     = c - row * char_per_row;
		const int      pos_src = row * char_h * _stride + col * char_w;
		_glyph_arr [c]._data_index = pos_src;
	}

	// Character mapping
	ZoneArray ().swap (_zone_arr);
	for (int c = 0; c < nbr_char; ++c)
	{
		add_char (unicode_arr [c], c);
	}
}



/*
This function takes a raw uncompressed picture file (no header) as input.
The picture is made of nbr_char characters aligned from left to right.
There is an extra line at the bottom to indicate the character width.
This line alternates between 0 and 255 at each character.
pic_w is the total width of the picture (sum of all character widths), and
pic_h its height. Therefore the font height is pic_h - 1.
Pixels are unsigned 8-bit greyscale (0 = transparent, 255 = 100% opaque).
*/

int	Font::init (std::string filename, int nbr_char, int pic_w, int pic_h, const char32_t unicode_arr [], int baseline)
{
	assert (! filename.empty ());
	assert (nbr_char > 0);
	assert (nbr_char <= 0x7FFF);
	assert (pic_w >= nbr_char);
	assert (pic_h > 1);
	assert (unicode_arr != 0);
	assert (baseline > 0);
	assert (baseline <= pic_h - 1);

	int            ret_val = 0;

	_nbr_char   = nbr_char;
	_baseline   = baseline;
	_prop_flag  = true;

	_char_h     = pic_h - 1;
	_stride     = pic_w;
	_data_arr.resize (_stride * _char_h);
	PicData        width_code (_stride);

	// Loads file
	FILE *         file_ptr = fstb::fopen_utf8 (filename.c_str (), "rb");
	if (file_ptr == 0)
	{
		ret_val = -1;
	}

	// Main data
	if (ret_val == 0)
	{
		const size_t   len_read = fread (
			_data_arr.data (),
			sizeof (_data_arr [0]),
			_data_arr.size (),
			file_ptr
		);
		if (len_read != _data_arr.size ())
		{
			ret_val = -1;
		}
	}

	// Additional line indicating the width of each character
	if (ret_val == 0)
	{
		const size_t   len_read = fread (
			width_code.data (),
			sizeof (width_code [0]),
			width_code.size (),
			file_ptr
		);
		if (len_read != width_code.size ())
		{
			ret_val = -1;
		}
	}

	if (file_ptr != 0)
	{
		fclose (file_ptr);
		file_ptr = 0;
	}

	// Builds data
	if (ret_val == 0)
	{
		GlyphInfoArray (nbr_char).swap (_glyph_arr);

		int            pos_dst = 0;
		for (int c = 0; c < nbr_char && pos_dst < _stride; ++c)
		{
			// Finds the character length
			uint8_t        color = width_code [pos_dst];
			int            width = 0;
			while (   pos_dst + width < _stride
			       && width_code [pos_dst + width] == color)
			{
				++ width;
			}

			GlyphInfo &    gi = _glyph_arr [c];
			gi._width      = width;
			gi._data_index = pos_dst;

			pos_dst += width;
		}

		if (pos_dst != _stride)
		{
			ret_val = -1;
		}
	}

	if (ret_val == 0)
	{
		_bold_shift = std::max (pic_w / (nbr_char * 8), 1);
		_data_ptr   = _data_arr.data ();

		// Character mapping
		ZoneArray ().swap (_zone_arr);
		for (int c = 0; c < nbr_char; ++c)
		{
			add_char (unicode_arr [c], c);
		}
	}

	return ret_val;
}



void	Font::add_char (char32_t ucs4, int index)
{
	assert (is_ready ());
	assert (index >= 0);
	assert (index < _nbr_char);

	const size_t   zone_idx = ucs4 >> _zone_bits;
	if (zone_idx >= _zone_arr.size ())
	{
		_zone_arr.resize (zone_idx + 1);
	}
	Zone *         zone_ptr = _zone_arr [zone_idx].get ();
	if (zone_ptr == 0)
	{
		zone_ptr = new Zone;
		_zone_arr [zone_idx] = ZoneUPtr (zone_ptr);
		for (size_t i = 0; i < zone_ptr->size (); ++i)
		{
			(*zone_ptr) [i] = _not_found;
		}
	}

	const int      loc = ucs4 & _zone_mask;
	(*zone_ptr) [loc] = int16_t (index);
}



bool	Font::is_ready () const
{
	return (_data_ptr != 0);
}



bool	Font::is_existing (char32_t ucs4) const
{
	return (get_char_pos (ucs4) >= 0);
}



int	Font::get_baseline () const
{
	return _baseline;
}



int	Font::get_char_h () const
{
	return _char_h;
}



int	Font::get_char_w (char32_t ucs4) const
{
	const int      g_idx = get_char_pos_no_fail (ucs4);
	const GlyphInfo & gl_info = _glyph_arr [g_idx];
	const int      w     = gl_info._width;
	assert (w > 0);

	return w;
}



int	Font::get_bold_shift () const
{
	return _bold_shift;
}



// You can use std::codecvt_utf8 <char32_t> to convert from UTF-8.
void	Font::render_char (uint8_t *buf_ptr, char32_t ucs4, int dst_stride) const
{
	assert (is_ready ());
	assert (buf_ptr != 0);

	const int      c       = get_char_pos_no_fail (ucs4);
	const GlyphInfo & gi   = _glyph_arr [c];
	const int      char_w  = gi._width;
	assert (dst_stride >= char_w);
	const uint8_t* src_ptr = _data_ptr + gi._data_index;

	for (int y = 0; y < _char_h; ++y)
	{
		for (int x = 0; x < char_w; ++x)
		{
			buf_ptr [x] = src_ptr [x];
		}
		buf_ptr += dst_stride;
		src_ptr += _stride;
	}
}



void	Font::render_char (uint8_t *buf_ptr, char32_t ucs4, int dst_stride, int mag_x, int mag_y) const
{
	assert (is_ready ());
	assert (buf_ptr != 0);
	assert (mag_x > 0);
	assert (mag_y > 0);

	if (mag_x == 1 && mag_y == 1)
	{
		render_char (buf_ptr, ucs4, dst_stride);
	}
	else
	{
		const int      c       = get_char_pos_no_fail (ucs4);
		const GlyphInfo & gi   = _glyph_arr [c];
		const int      char_w  = gi._width;
		assert (dst_stride >= char_w);
		const uint8_t* src_ptr = _data_ptr + gi._data_index;

		for (int y = 0; y < _char_h; ++y)
		{
			for (int x = 0; x < char_w; ++x)
			{
				const uint8_t  v      = src_ptr [x];
				const int      x_base = x * mag_x;
				for (int x2 = 0; x2 < mag_x; ++x2)
				{
					buf_ptr [x_base + x2] = v;
				}
			}
			buf_ptr += dst_stride;
			src_ptr += _stride;
			for (int y2 = 1; y2 < mag_y; ++y2)
			{
				for (int x = 0; x < char_w * mag_x; ++x)
				{
					buf_ptr [x] = buf_ptr [x - dst_stride];
				}
				buf_ptr += dst_stride;
			}
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Returns -1 if not found
int	Font::get_char_pos (char32_t ucs4) const
{
	int            pos = _not_found;

	const size_t   zone_idx = ucs4 >> _zone_bits;
	if (zone_idx < _zone_arr.size ())
	{
		const Zone &   zone = *(_zone_arr [zone_idx]);
		const int      loc  = ucs4 & _zone_mask;
		pos = zone [loc];
	}

	return pos;
}



// Replaces unmapped characters with the one at position 0
int	Font::get_char_pos_no_fail (char32_t ucs4) const
{
	return std::max (get_char_pos (ucs4), 0);
}



}  // namespace ui
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
