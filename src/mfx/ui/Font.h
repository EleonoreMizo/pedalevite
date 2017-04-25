/*****************************************************************************

        Font.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_Font_HEADER_INCLUDED)
#define mfx_ui_Font_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>
#include <array>
#include <memory>

#include <cstdint>



namespace mfx
{
namespace ui
{



class Font
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Font ()  = default;
	virtual        ~Font () = default;

	void           init (int nbr_char, int char_w, int char_h, int char_per_row, int stride, const uint8_t pic_arr [], const char32_t unicode_arr [], int baseline, int max_val, bool copy_data_flag = true);
	void           add_char (char32_t ucs4, int index);

	bool           is_ready () const;
	bool           is_existing (char32_t ucs4) const;
	int            get_baseline () const;
	int            get_char_h () const;
	int            get_char_w (char32_t ucs4 = 32) const;

	void           render_char (uint8_t *buf_ptr, char32_t ucs4, int dst_stride) const;
	void           render_char (uint8_t *buf_ptr, char32_t ucs4, int dst_stride, int mag_x, int mag_y) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int                    // 256 char per zone
	               _zone_bits    = 8;
	static const int
						_zone_mask    = (1 << _zone_bits) - 1;
	static const int
	               _not_found    = -1;

	typedef std::array <int16_t, 1 << _zone_bits> Zone;   // _not_found: not assigned
	typedef std::unique_ptr <Zone> ZoneUPtr;
	typedef std::vector <ZoneUPtr> ZoneArray;
	typedef std::vector <uint8_t>  PicData;

	int            get_char_pos (char32_t ucs4) const;

	int            _nbr_char     = 0;
	int            _char_w       = 0;
	int            _char_h       = 0;
	int            _char_per_row = 0;
	int            _stride       = 0;
	int            _baseline     = 0;
	ZoneArray      _zone_arr;
	PicData        _data_arr;
	const uint8_t *_data_ptr     = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Font (const Font &other)              = delete;
	Font &         operator = (const Font &other)        = delete;
	bool           operator == (const Font &other) const = delete;
	bool           operator != (const Font &other) const = delete;

}; // class Font



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/Font.hpp"



#endif   // mfx_ui_Font_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
