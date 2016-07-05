/*****************************************************************************

        EndMsg.cpp
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

#include "mfx/uitk/pg/EndMsg.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/View.h"

#include <vector>

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EndMsg::EndMsg ()
:	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_line_list ()
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EndMsg::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	assert (usr_ptr != 0);
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;

	const std::string txt (reinterpret_cast <const char *> (usr_ptr));

	size_t         pos = 0;
	std::vector <std::string> line_arr;
	while (pos < txt.length ())
	{
		size_t         pos_nxt = txt.find ('\n', pos);
		if (pos_nxt == std::string::npos)
		{
			line_arr.push_back (txt.substr (pos));
			pos_nxt = txt.length ();
		}
		else
		{
			line_arr.push_back (txt.substr (pos, pos_nxt - pos));
			++ pos_nxt;
		}
		pos = pos_nxt;
	}

	const int      x_mid =  _page_size [0] >> 1;
	const int      h_l   = fnt_l.get_char_h ();

	_line_list.clear ();
	const int      nbr_lines = int (line_arr.size ());
	const int      y_base    = (_page_size [1] - nbr_lines * h_l) >> 1;
	for (int l_cnt = 0; l_cnt < nbr_lines; ++l_cnt)
	{
		TxtSPtr        line_sptr (new NText (l_cnt));
		line_sptr->set_font (fnt_l);
		line_sptr->set_justification (0.5f, 0, false);
		line_sptr->set_frame (Vec2d (_page_size [0], 0), Vec2d ());
		line_sptr->set_coord (Vec2d (x_mid, y_base + h_l * l_cnt));
		line_sptr->set_text (line_arr [l_cnt]);
		_line_list.push_back (line_sptr);
		_page_ptr->push_back (line_sptr);
	}
}



void	EndMsg::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	EndMsg::do_handle_evt (const NodeEvt &evt)
{
	// No exit
	return (EvtProp_PASS);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
