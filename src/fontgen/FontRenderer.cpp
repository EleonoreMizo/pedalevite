/*****************************************************************************

        FontRenderer.cpp
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

#include	"FontRenderer.h"

#include <algorithm>
#include	<stdexcept>

#include	<cassert>
#include <cmath>
#include	<cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FontRenderer::FontRenderer (std::string filename, double size_pt, double scale_h)
:	_glyph_list ()
,	_font_filename (filename)
,	_size_pt (size_pt)
,	_rendered_flag (false)
,	_library ()
,	_face ()
,	_max_width (0)
,	_max_top (0)
,	_max_bot (0)
{
	assert (! filename.empty ());
	assert (size_pt > 0);
	assert (scale_h > 0);

	// Loads FreeType
	const int      err_lib = FT_Init_FreeType (&_library);
	if (err_lib != 0)
	{
		throw std::runtime_error (
			"FontRenderer::FontRenderer(): Cannot initialise FreeType library."
		);
	}

	// Loads font
	const int      err_face = FT_New_Face (
		_library,
		_font_filename.c_str (),
		0,
		&_face
	);
	if (err_face == FT_Err_Unknown_File_Format)
	{
		FT_Done_FreeType (_library);
		throw std::runtime_error (
			"FontRenderer::FontRenderer(): Cannot load font, unknown file format."
		);
	}
	else if (err_face != 0)
	{
		FT_Done_FreeType (_library);
		throw std::runtime_error (
			"FontRenderer::FontRenderer(): Cannot load font."
		);	
	}

	// Configures char size info
	const int      size_pt_26_6 =
		std::max (int (round (_size_pt * _size_scale)), 1);
	const int      dpi_v        = 72;
	const int      dpi_h        = std::max (int (round (dpi_v * scale_h)), 1);
	const int      err_size     = FT_Set_Char_Size (
		_face,         // handle to face object
		0,             // char_width in 1/64th of points
		size_pt_26_6,  // char_height in 1/64th of points
		dpi_h,         // Horizontal device resolution
		dpi_v          // Vertical device resolution
	);
	if (err_size != 0)
	{
		FT_Done_FreeType (_library);
		throw std::runtime_error (
			"FontRenderer::FontRenderer(): Cannot set character size."
		);
	}
}



FontRenderer::~FontRenderer ()
{
	FT_Done_FreeType (_library);
}



int	FontRenderer::export_to_raw (std::string filename)
{
	assert (! filename.empty ());

	PicData        pic_data;
	int            pic_w;
	int            pic_h;
	int            ret_val = render_font_to_pic_data (pic_data, pic_w, pic_h);
	if (ret_val != 0)
	{
		printf ("*** Error: cannot render the font.\n");
	}

	FILE *         file_ptr = 0;
	if (ret_val == 0)
	{
		file_ptr = fopen (filename.c_str (), "wb");
		if (file_ptr == 0)
		{
			printf ("*** Error: cannot open file \"%s\"\n", filename.c_str ());
			ret_val = -1;
		}
	}

	if (ret_val == 0)
	{
		const size_t   len     = size_t (pic_w) * size_t (pic_h);
		const size_t   len_wrt = fwrite (
			pic_data.data (),
			sizeof (pic_data [0]),
			len,
			file_ptr
		);
		if (len_wrt != len)
		{
			printf ("*** Error: cannot write file \"%s\"\n", filename.c_str ());
			ret_val = -1;
		}
	}

	if (file_ptr != 0)
	{
		fclose (file_ptr);
		file_ptr = 0;
	}

	if (ret_val == 0)
	{
		printf ("Font successfully written.\n");
		printf ("Max width : %d\n", _max_width);
		printf ("Max height: %d\n", _max_top + _max_bot);
		printf ("Baseline  : %d\n", _max_top);
		printf ("Picture   : %dx%d\n", pic_w, pic_h);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	FontRenderer::render_font_to_pic_data (PicData &pic_data, int &pic_w, int &pic_h)
{
	int            ret_val = 0;

	if (ret_val == 0 && ! _rendered_flag)
	{
		ret_val = render_font ();
	}

	if (ret_val == 0)
	{
		const int      height = _max_top + _max_bot;
		pic_w = compute_total_width ();
		pic_h = height + 1;

		const int      len = pic_w * pic_h;
		pic_data.clear ();
		pic_data.resize (len, 0);

		int            glyph_x_pos = 0;
		for (int c_index = 0; c_index < _nbr_char; ++c_index)
		{
			Glyph &        glyph = _glyph_list [c_index];
			const int      width = glyph.get_width ();

			// Copies glyph
			for (int y = 0; y < height; ++y)
			{
				const int      pix_pos = y * pic_w + glyph_x_pos;
				for (int x = 0; x < width; ++x)
				{
					const int      c = glyph.get_pix (x, y);
					pic_data [pix_pos + x] = uint8_t (c);
				}
			}

			// Delimiter
			const uint8_t  delim   = uint8_t ((c_index & 1) * 255);
			const int      pix_pos = height * pic_w + glyph_x_pos;
			for (int x = 0; x < width; ++x)
			{
				pic_data [pix_pos + x] = delim;
			}

			glyph_x_pos += width;
		}

		assert (glyph_x_pos == pic_w);
	}

	return ret_val;
}



int	FontRenderer::render_font ()
{
	int            ret_val = find_dimensions ();
	const int      height  = _max_top + _max_bot;

	if (ret_val == 0)
	{
		_max_width = 0;
		_glyph_list.resize (_nbr_char);
	}

	for (int c_index = 0; c_index < _nbr_char && ret_val == 0; ++c_index)
	{
		const FT_ULong c = static_cast <FT_ULong> (_unicode_list [c_index]);

		// Gets glyph
		const int      glyph_index = FT_Get_Char_Index (_face, c);
		ret_val = FT_Load_Glyph (
			_face,
			glyph_index,
			FT_LOAD_DEFAULT
		);
		if (ret_val == 0)
		{
			ret_val = FT_Render_Glyph (
				_face->glyph,
				FT_RENDER_MODE_NORMAL
			);
		}

		if (ret_val == 0)
		{
			FT_GlyphSlot & slot_ptr      = _face->glyph;
			FT_Bitmap &    bitmap        = slot_ptr->bitmap;
			const int      advance       = int (round (
				static_cast <double> (slot_ptr->linearHoriAdvance) / 0x10000
			));
			const int      width         = bitmap.width;
			const int      dest_width    = std::max (width, advance);
			const int      bitmap_height = bitmap.rows;
			const int      bitmap_top    = slot_ptr->bitmap_top;
			const int      scanline_w    = bitmap.pitch;
			const uint8_t* data_ptr      = bitmap.buffer;

			Glyph &        glyph         = _glyph_list [c_index];
			glyph.set_size (dest_width, height);
			_max_width = std::max (_max_width, dest_width);

			// Copies bitmap
			const int      offset = _max_top - bitmap_top;
			for (int y = 0; y < bitmap_height; ++y)
			{
				const int      dest_y = offset + y;
				if (dest_y >= 0 && dest_y < height)
				{
					for (int x = 0; x < width; ++x)
					{
						const int      pos = y * scanline_w + x;
						const int      v   = data_ptr [pos];
						glyph.set_pix (x, dest_y, v);
					}
				}
			}
		}
	}

	if (ret_val == 0)
	{
		_rendered_flag = true;
	}

	return (ret_val);
}



int	FontRenderer::find_dimensions ()
{
	int            ret_val = 0;

	for (int c_index = 32; c_index < 127 && ret_val == 0; ++c_index)
	{
		const FT_ULong c = static_cast <FT_ULong> (_unicode_list [c_index]);

		// Gets glyph
		const int      glyph_index = FT_Get_Char_Index (_face, c);
		ret_val = FT_Load_Glyph (
			_face,
			glyph_index,
			FT_LOAD_DEFAULT
		);
		if (ret_val == 0)
		{
			ret_val = FT_Render_Glyph (
				_face->glyph,
				FT_RENDER_MODE_NORMAL
			);
		}

		// Computes maximum character size
		if (ret_val == 0)
		{
			FT_GlyphSlot   slot_ptr = _face->glyph;
			const int      ext_top  = slot_ptr->bitmap_top;
			const int      ext_bot  = slot_ptr->bitmap.rows - slot_ptr->bitmap_top;
			_max_top = std::max (_max_top, ext_top);
			_max_bot = std::max (_max_bot, ext_bot);
		}
	}

	return (ret_val);
}



int	FontRenderer::compute_total_width ()
{
	assert (_rendered_flag);

	int            width = 0;
	for (int c_index = 0; c_index < _nbr_char; ++c_index)
	{
		Glyph &        glyph = _glyph_list [c_index];
		width += glyph.get_width ();
	}

	return width;
}



// ISO 8859-15 + a few other characters
// http://ftp.unicode.org/Public/MAPPINGS/ISO8859/8859-15.TXT
// https://en.wikipedia.org/wiki/List_of_Unicode_characters#Arrows
// https://en.wikipedia.org/wiki/List_of_Unicode_characters#Miscellaneous_Symbols
// https://en.wikipedia.org/wiki/Dingbat#Unicode
const std::array <char32_t, FontRenderer::_nbr_char>	FontRenderer::_unicode_list =
{{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

	0x2191, 0x2193, 0x2190, 0x2192, 0x21E7, 0x21E9, 0x21E6, 0x21E8, 0x2713, 0x26AB, 0x26AC, 0x26AD, 0x2026, 0x221E, 0x008E, 0x008F,
	0x2018, 0x2019, 0x201C, 0x201D, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7, 0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7, 0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
}};



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
