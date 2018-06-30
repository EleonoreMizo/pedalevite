/*****************************************************************************

        NText.cpp
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

#include "fstb/txt/utf8/Codec8.h"
#include "fstb/Err.h"
#include "fstb/fnc.h"
#include "mfx/ui/Font.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/ParentInterface.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NText::NText ()
:	_txt ()
,	_font_ptr (0)
,	_frame_size ()
,	_margin ()
,	_mag_arr ({{ 1, 1 }})
,	_justification ({{ 0, 0 }})
,	_baseline_flag (false)
,	_bold_flag (false)
,	_space_flag (false)
,	_vid_inv_flag (false)
,	_underline_flag (false)
,	_origin ()
,	_txt_ucs4 ()
{
	// Nothing
}



NText::NText (int node_id)
:	Inherited (node_id)
,	_txt ()
,	_font_ptr (0)
,	_frame_size ()
,	_margin ()
,	_mag_arr ({{ 1, 1 }})
,	_justification ({{ 0, 0 }})
,	_baseline_flag (false)
,	_bold_flag (false)
,	_space_flag (false)
,	_vid_inv_flag (false)
,	_underline_flag (false)
,	_origin ()
,	_txt_ucs4 ()
{
	// Nothing
}



void	NText::set_frame (Vec2d size_max, Vec2d margin)
{
	const Rect     zone_old (get_bounding_box () + get_coord ());

	assert (size_max [0] >= 0);
	assert (size_max [1] >= 0);
	assert (margin [0] >= 0);
	assert (margin [1] >= 0);

	_frame_size = size_max;
	_margin     = margin;
	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



void	NText::set_text (std::string txt)
{
	const Rect     zone_old (get_bounding_box () + get_coord ());

	_txt = txt;
	update_txt_ucs4 ();
	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



std::string	NText::get_text () const
{
	return _txt;
}



void	NText::set_font (const ui::Font &fnt)
{
	const Rect     zone_old (get_bounding_box () + get_coord ());

	_font_ptr = &fnt;
	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



void	NText::set_mag (int x, int y)
{
	assert (x > 0);
	assert (y > 0);

	const Rect     zone_old (get_bounding_box () + get_coord ());

	_mag_arr [0] = x;
	_mag_arr [1] = y;
	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



void	NText::set_justification (float x, float y, bool baseline_flag)
{
	assert (x >= 0);
	assert (x <= 1);
	assert (y >= 0);
	assert (y <= 1);

	const Rect     zone_old (get_bounding_box () + get_coord ());

	_justification [0] = x;
	_justification [1] = y;
	_baseline_flag     = baseline_flag;
	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



void	NText::set_bold (bool bold_flag, bool space_flag)
{
	const Rect     zone_old (get_bounding_box () + get_coord ());

	_bold_flag  = bold_flag;
	_space_flag = space_flag;

	update_content ();

	ParentInterface * parent_ptr = get_parent ();
	if (parent_ptr != 0)
	{
		parent_ptr->invalidate (zone_old);
	}
}



void	NText::set_vid_inv (bool vid_inv_flag)
{
	_vid_inv_flag = vid_inv_flag;

	update_content ();
}



void	NText::set_underline (bool underline_flag)
{
	_underline_flag = underline_flag;

	update_content ();
}



// In displayed pixels
int	NText::get_char_width (char32_t c) const
{
	assert (_font_ptr != 0);

	int            len = _font_ptr->get_char_w (c);
	if (_bold_flag && _space_flag)
	{
		++ len;
	}
	len *= _mag_arr [0];

	return len;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Rect	NText::do_get_bounding_box () const
{
	return Inherited::do_get_bounding_box () + _origin;
}



void	NText::do_redraw (ui::DisplayInterface &disp, Rect clipbox, Vec2d node_coord)
{
	Inherited::do_redraw (disp, clipbox, node_coord + _origin);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NText::update_txt_ucs4 ()
{
	_txt_ucs4.clear ();
	size_t         pos_utf8 = 0;
	int            ret_val  = fstb::Err_OK;
	while (pos_utf8 < _txt.size () && ret_val == fstb::Err_OK)
	{
		int            len_utf8 = 1;
		ret_val = fstb::txt::utf8::Codec8::get_char_seq_len_utf (
			len_utf8, _txt [pos_utf8]
		);

		if (ret_val == fstb::Err_OK)
		{
			char32_t       c_ucs4;
			ret_val = fstb::txt::utf8::Codec8::decode_char (
				c_ucs4, _txt.c_str () + pos_utf8
			);

			_txt_ucs4 += c_ucs4;
			pos_utf8  += len_utf8;
		}
	}

	assert (ret_val == fstb::Err_OK);
}



void	NText::update_content ()
{
	if (_font_ptr == 0 || _txt.empty ())
	{
		set_size (Vec2d ());
	}

	else
	{
		// First, finds the string size in pixels
		const int      w_pix = compute_width_pix ();
		const int      h_pix = _font_ptr->get_char_h () * _mag_arr [1];

		// The frame size
		int            fw_pix = std::max (w_pix + _margin [0], _frame_size [0]);
		int            fh_pix = std::max (h_pix + _margin [1], _frame_size [1]);
		set_size (Vec2d (fw_pix, fh_pix));

		// Sets the origin
		_origin = Vec2d (
			-fstb::round_int (fw_pix * _justification [0]),
			-fstb::round_int (fh_pix * _justification [1])
		);
		if (_baseline_flag)
		{
			_origin [1] = -_font_ptr->get_baseline ();
		}

		// Margins from the top-left corner
		const int      margin_x = fstb::round_int (
			  _margin [0]      * (1 - _justification [0])
			+ (fw_pix - w_pix) *      _justification [0]
		);
		const int      margin_y = fstb::round_int (
			  _margin [1]      * (1 - _justification [1])
			+ (fh_pix - h_pix) *      _justification [1]
		);

		uint8_t *      buf_ptr = use_buffer ();
		const int      stride  = get_stride ();

		// Cleans the buffer if required
		if (   (_bold_flag && _space_flag)
		    || fw_pix > w_pix
		    || fh_pix > h_pix)
		{
			memset (buf_ptr, 0, stride * fh_pix);
		}

		// Renders the string
		{
			uint8_t *      buf2_ptr = buf_ptr + stride * margin_y;
			int            x = margin_x;
			for (auto c : _txt_ucs4)
			{
				_font_ptr->render_char (
					buf2_ptr + x, c, stride, _mag_arr [0], _mag_arr [1]
				);
				x += get_char_width (c);
			}
		}

		// Bold
		if (_bold_flag)
		{
			const int      mag_x    = _mag_arr [0];
			uint8_t *      buf2_ptr = buf_ptr + stride * margin_y;
			for (int y = 0; y < h_pix; ++y)
			{
				for (int x = margin_x + w_pix - 1; x >= margin_x + mag_x; -- x)
				{
					buf2_ptr [x] = std::max (buf2_ptr [x], buf2_ptr [x - mag_x]);
				}
				buf2_ptr += stride;
			}
		}

		// Underline
		if (_underline_flag)
		{
			const int      mag_y     = _mag_arr [1];
			const int      thickness = (mag_y + 1) >> 1;
			const int      y = margin_y + h_pix - thickness;
			memset (buf_ptr + y * w_pix + margin_x, 255, stride * thickness);
		}

		// Video inverse
		if (_vid_inv_flag)
		{
			if (has_cursor () && _vid_inv_flag && fw_pix >= 2 && fh_pix >= 2)
			{
				NodeBase::invert_zone (
					buf_ptr + fw_pix + 1,
					fw_pix - 2,
					fh_pix - 2,
					stride
				);
			}
			else
			{
				NodeBase::invert_zone (buf_ptr, fw_pix, fh_pix, stride);
			}
		}

		invalidate_all ();
	}
}



int	NText::compute_width_pix () const
{
	int            w_pix = 0;
	for (auto c : _txt_ucs4)
	{
		w_pix += get_char_width (c);
	}

	return w_pix;
}



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
