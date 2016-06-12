/*****************************************************************************

        NText.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NText_HEADER_INCLUDED)
#define mfx_uitk_NText_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/unicode/StringU.h"
#include "mfx/uitk/NBitmap.h"

#include <array>



namespace mfx
{

namespace ui
{
	class Font;
}

namespace uitk
{



class NText
:	public NBitmap
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef NBitmap Inherited;

	               NText ();
	explicit       NText (int node_id);
	               NText (const NText &other)      = default;
	virtual        ~NText ()                       = default;

	NText &        operator = (const NText &other) = default;

	void           set_frame (Vec2d size_min, Vec2d margin);
	void           set_text (std::string txt);
	void           set_font (const ui::Font &fnt);
	void           set_mag (int x, int y);
	void           set_justification (float x, float y);
	void           set_bold (bool bold_flag, bool space_flag);
	void           set_vid_inv (bool vid_inv_flag);
	void           set_underline (bool underline_flag);

	int            get_char_width (char32_t c) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::NodeInterface via mfx::uitk::NBitmap
	virtual Rect   do_get_bounding_box () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_txt_ucs4 ();
	void           update_content ();
	int            compute_width_pix () const;

	std::string    _txt;
	const ui::Font *
	               _font_ptr;        // 0 = not set
	Vec2d          _frame_size;      // The minimum frame size (for video inverse). Includes the margin. >= 0
	Vec2d          _margin;          // Distributed on all sides depending on the justification. >= 0
	std::array <int, 2>              // Magnification in 2 directions. > 0
	               _mag_arr;
	std::array <float, 2>            // [0 ; 1]. 0 = left/top, 0.5 = center, 1.0 = right/bottom
	               _justification;
	bool           _bold_flag;
	bool           _space_flag;      // When bold, insert one column between all characters
	bool           _vid_inv_flag;    // In addition to the selection
	bool           _underline_flag;

	// Cached
	Vec2d          _origin;          // Top-left of the textbox relative to the node coord.
	fstb::txt::unicode::StringU
	               _txt_ucs4;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const NText &other) const = delete;
	bool           operator != (const NText &other) const = delete;

}; // class NText



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NText.hpp"



#endif   // mfx_uitk_NText_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
