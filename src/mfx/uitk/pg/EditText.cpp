/*****************************************************************************

        EditText.cpp
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

#include "fstb/txt/unicode/unicode.h"
#include "fstb/txt/utf8/Codec8.h"
#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EditText::EditText (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_curs_sptr (new NBitmap (Entry_CURS))
,	_title_sptr ( new NText (Entry_TITLE ))
,	_text_sptr (  new NText (Entry_TEXT  ))
,	_ok_sptr (    new NText (Entry_OK    ))
,	_cancel_sptr (new NText (Entry_CANCEL))
,	_space_sptr ( new NText (Entry_SPACE ))
,	_del_sptr (   new NText (Entry_DEL   ))
,	_left_sptr (  new NText (Entry_LEFT  ))
,	_right_sptr ( new NText (Entry_RIGHT ))
,	_char_list ()
,	_curs_pos (0)
,	_curs_blink_flag (true)
,	_txt ()
,	_char_map ()
{
	_ok_sptr    ->set_text ("OK");
	_cancel_sptr->set_text ("CANCEL");
	_space_sptr ->set_text ("SPC");
	_del_sptr   ->set_text ("DEL");
	_left_sptr  ->set_text ("\xE2\x86\x90");  // U+2190
	_right_sptr ->set_text ("\xE2\x86\x92");  // U+2192

	_curs_sptr->set_blend_mode (ui::DisplayInterface::BlendMode_XOR);

	_menu_sptr->set_autoscroll (true);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditText::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	assert (usr_ptr != 0);

	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt_m;
	_param_ptr = reinterpret_cast <Param *> (usr_ptr);

	_param_ptr->_ok_flag = false;

	const int      scr_w = _page_size [0];
	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      w_m   = _fnt_ptr->get_char_w (' ');
	const int      w_m2  = _fnt_ptr->get_char_w ('M');

	_curs_sptr->set_size (Vec2d (1, h_m));
	uint8_t *      buf_ptr = _curs_sptr->use_buffer ();
	memset (buf_ptr, 255, h_m);
	fstb::txt::unicode::conv_utf8_to_unicode (_txt, _param_ptr->_text.c_str ());
	_curs_pos = int (_txt.length ());

	_title_sptr ->set_font (*_fnt_ptr);
	_text_sptr  ->set_font (*_fnt_ptr);
	_ok_sptr    ->set_font (*_fnt_ptr);
	_cancel_sptr->set_font (*_fnt_ptr);
	_space_sptr ->set_font (*_fnt_ptr);
	_del_sptr   ->set_font (*_fnt_ptr);
	_left_sptr  ->set_font (*_fnt_ptr);
	_right_sptr ->set_font (*_fnt_ptr);

	_title_sptr ->set_coord (Vec2d (0, 0 * h_m));
	_text_sptr  ->set_coord (Vec2d (0, 1 * h_m));

	_title_sptr ->set_text (_param_ptr->_title);

	int            x = 0;
	_ok_sptr    ->set_coord (Vec2d (x, 0));
	x += _ok_sptr    ->get_bounding_box ().get_size () [0] + w_m;
	_cancel_sptr->set_coord (Vec2d (x, 0));
	x += _cancel_sptr->get_bounding_box ().get_size () [0] + w_m;
	_space_sptr ->set_coord (Vec2d (x, 0));
	x += _space_sptr ->get_bounding_box ().get_size () [0] + w_m;
	_del_sptr   ->set_coord (Vec2d (x, 0));
	x += _del_sptr   ->get_bounding_box ().get_size () [0] + w_m;
	_left_sptr  ->set_coord (Vec2d (x, 0));
	x += _left_sptr  ->get_bounding_box ().get_size () [0] + w_m;
	_right_sptr ->set_coord (Vec2d (x, 0));

	const Vec2d    pos_menu (0, 2 * h_m);
	_menu_sptr->set_size (_page_size - pos_menu, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	_menu_sptr->set_coord (pos_menu);

	_menu_sptr->clear_all_nodes ();
	_menu_sptr->push_back (_ok_sptr    );
	_menu_sptr->push_back (_cancel_sptr);
	_menu_sptr->push_back (_space_sptr );
	_menu_sptr->push_back (_del_sptr   );
	_menu_sptr->push_back (_left_sptr  );
	_menu_sptr->push_back (_right_sptr );

	NodeIdTable    id_table;
	_char_list.clear ();
	_char_map.clear ();
	int            node_id = Entry_CHAR_BEG;
	int            py      = 0;
	for (const auto &str : _char_table)
	{
		int            px = 0;
		std::vector <int> row;
		for (const char32_t c : str)
		{
			if (c == ' ')
			{
				row.push_back (-1);
			}
			else if (c == Span_R || c == Span_L)
			{
				row.push_back (-int (c));
			}
			else if (c < Entry_CHAR_BEG)
			{
				row.push_back (c);
			}
			else
			{
				row.push_back (node_id);
				TxtSPtr        char_sptr (new NText (node_id));
				char_sptr->set_font (*_fnt_ptr);
				char_sptr->set_coord (Vec2d (px, py));
				std::string    str;
				fstb::txt::utf8::Codec8::encode_char (str, c);
				char_sptr->set_text (str);
				_char_map [node_id] = c;
				_char_list.push_back (char_sptr);
				_menu_sptr->push_back (char_sptr);
				++ node_id;
			}
			px += w_m2;
		}

		id_table.push_back (row);
		py += h_m;
	}
	const int         node_id_end = node_id;

	PageMgrInterface::NavLocList  nav_list (node_id_end - Entry_NAV_BEG);
	for (int nit_y = 0; nit_y < int (id_table.size ()); ++ nit_y)
	{
		const NodeIdRow & row = id_table [nit_y];
		for (int nit_x = 0; nit_x < int (row.size ()); ++ nit_x)
		{
			fill_nav (nav_list, id_table, nit_x, nit_y);
		}
	}

	_page_ptr->push_back (_title_sptr);
	_page_ptr->push_back (_text_sptr );
	_page_ptr->push_back (_curs_sptr );
	_page_ptr->push_back (_menu_sptr );

	_page_ptr->set_nav_layout (nav_list);

	_page_ptr->set_timer (Entry_CURS, true);

	update_display ();
}



void	EditText::do_disconnect ()
{
	_page_ptr->set_timer (Entry_CURS, false);
}



MsgHandlerInterface::EvtProp	EditText::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (evt.is_timer ())
	{
		/*** To do: slow down blinking ***/
		_curs_blink_flag = ! _curs_blink_flag;
		update_curs ();
		ret_val = EvtProp_CATCH;
	}

	else if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
			ret_val = EvtProp_CATCH;
			switch (node_id)
			{
			case Entry_OK:
				_param_ptr->_ok_flag = true;
				_page_switcher.return_page ();
				break;
			case Entry_CANCEL:
				_param_ptr->_ok_flag = false;
				_page_switcher.return_page ();
				break;
			case Entry_SPACE:
				_txt.insert (_txt.begin () + _curs_pos, ' ');
				++ _curs_pos;
				update_display ();
				break;
			case Entry_DEL:
				if (_curs_pos > 0)
				{
					_txt.erase (_curs_pos - 1, 1);
					-- _curs_pos;
					update_display ();
				}
				break;
			case Entry_LEFT:
				_curs_pos = std::max (_curs_pos - 1, 0);
				_curs_blink_flag = true;
				update_curs ();
				break;
			case Entry_RIGHT:
				_curs_pos = std::min (_curs_pos + 1, int (_txt.length ()));
				_curs_blink_flag = true;
				update_curs ();
				break;
			default:
				{
					auto           it = _char_map.find (node_id);
					if (it == _char_map.end ())
					{
						ret_val = EvtProp_PASS;
					}
					else
					{
						_txt.insert (_txt.begin () + _curs_pos, it->second);
						++ _curs_pos;
						update_display ();
					}
				}
				break;
			}
			break;
		case Button_E:
			_param_ptr->_ok_flag = false;
			_page_switcher.return_page ();
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	EditText::update_display ()
{
	fstb::txt::unicode::conv_unicode_to_utf8 (_param_ptr->_text, _txt.c_str ());
	_text_sptr->set_text (_param_ptr->_text);
	_curs_blink_flag = true;
	update_curs ();

	_menu_sptr->invalidate_all ();
}



void	EditText::update_curs ()
{
	int            x = 0;
	for (int pos = 0; pos < _curs_pos && pos < int (_txt.length ()); ++ pos)
	{
		const char32_t c = _txt [pos];
		x += _fnt_ptr->get_char_w (c);
	}

	const int      h_m   = _fnt_ptr->get_char_h ();
	_curs_sptr->set_coord (Vec2d (x, h_m));
	_curs_sptr->show (_curs_blink_flag);
}



void	EditText::fill_nav (PageMgrInterface::NavLocList &nav_list, const NodeIdTable &nit, int x, int y) const
{
	assert (x >= 0);
	assert (y >= 0);
	const int      sy = int (nit.size ());
	assert (y < sy);
	assert (x < int (nit [y].size ()));

	class Dir
	{
	public:
		int         _dx;
		int         _dy;
		Button      _but;
	};

	static const std::array <Dir, 4> dir_arr =
	{{
		{  1,  0, Button_R },
		{  0, -1, Button_U },
		{ -1,  0, Button_L },
		{  0,  1, Button_D }
	}};

	const int      node_base = nit [y] [x];
	if (node_base >= 0)
	{
		NavLoc &       nav = nav_list [node_base - Entry_NAV_BEG];
		nav._node_id = node_base;
		for (const auto &dir : dir_arr)
		{
			int            nx = x;
			int            ny = y;
			do
			{
				if (     dir._dx == 0 && nit [ny] [nx] == -int (Span_L))
				{
					++ nx;
				}
				else if (dir._dx == 0 && nit [ny] [nx] == -int (Span_R))
				{
					-- nx;
				}
				else
				{
					ny = (ny + dir._dy + sy) % sy;
					const int      sx = int (nit [ny].size ());
					nx = (nx + dir._dx + sx) % sx;
				}
			}
			while (nit [ny] [nx] < 0);
			
			if (nx != x || ny != y)
			{
				nav._action_arr [dir._but] = nit [ny] [nx];
			}
			else
			{
				nav._action_arr [dir._but] = NavLoc::OvrAction_STOP;
			}
		}
	}
}



// ' ' = empty cell
const std::array <std::u32string, 1+7>	EditText::_char_table =
{{
	{ Entry_OK,
	 Span_R,Span_R,Span_L,Span_L, Entry_CANCEL,
	                            Span_R,Span_R,Span_R,Span_L,Span_L, Entry_SPACE,
	                                                                   Span_R,Span_R,Span_L, Entry_DEL,
	                                                                                      Span_R,Span_L, Entry_LEFT,
	                                                                                                           Span_L, Entry_RIGHT },
	{ 'a', 'b', 'c' , 'd' , 'e' , 'f' , 'g' , 'A', 'B', 'C', 'D'  , 'E'  , 'F' , 'G' , 0xE1, 0xE0, 0xE2, 0xE3, 0xE4  , 0xC1, 0xC0, 0xC2, 0xC3, 0xC4 , 0xFE  },
	{ 'h', 'i', 'j' , 'k' , 'l' , 'm' , 'n' , 'H', 'I', 'K', 'L'  , 'M'  , 'N' , 'J' , 0xE9, 0xE8, 0xEA, 0xEB, 0xFD  , 0xC9, 0xC8, 0xCA, 0xCB, 0x161, 0xDE  },
	{ 'o', 'p', 'q' , 'r' , 's' , 't' , 'u' , 'O', 'P', 'Q', 'R'  , 'S'  , 'T' , 'U' , 0xED, 0xEC, 0xEE, 0xEF, 0xDD  , 0xCD, 0xCC, 0xCE, 0xCF, 0x160, 0xF8  },
	{ 'v', 'w', 'x' , 'y' , 'z' , 0xE7, 0xC7, 'V', 'W', 'X', 'Y'  , 'Z'  , 0xF1, 0xD1, 0xF3, 0xF2, 0xF4, 0xF5, 0xF6  , 0xD3, 0xD2, 0xD4, 0xD5, 0xD6 , 0xD8  },
	{ '0', '1', '2' , '3' , '4' , '5' , '6' , '7', '8', '9', 0x153, 0x152, 0xE6, 0xC6, 0xFA, 0xF9, 0xFB, 0xFC, 0xDF  , 0xDA, 0xD9, 0xDB, 0xDC, 0x17E, 0x17D },
	{ '-', '_', '\'', '\"', 0xAB, 0xBB, ',' , '.', ':', ';', '!'  , 0xA1 ,'\?' , 0xBF, '*' , '+' , 0xB7, '#' , '%'   , '&' , '|' , 0xE5, 0xC5               },
	{ '=', '@', '/' , '\\', '(' , ')' , '[' , ']', '{', '}', '<'  , '>'  , 0xD7, 0xF7, 0xB1, 0xAF, 0xA2, '$' , 0x20AC, 0xA3, 0xA5, 0xB0                     }
}};



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
