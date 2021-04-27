/*****************************************************************************

        Page.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_Page_HEADER_INCLUDED)
#define mfx_uitk_Page_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/UserInputInterface.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/Rect.h"

#include <list>
#include <set>



namespace mfx
{

class Model;
class View;

namespace ui
{
	class DisplayInterface;
	class Font;
}

namespace uitk
{



class Page
:	public PageMgrInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _disp_node_id = 666999;

	explicit       Page (Model &model, View &view, ui::DisplayInterface &display, ui::UserInputInterface::MsgQueue &queue_input_to_gui, ui::UserInputInterface &input_device, const ui::Font &fnt_t, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
	               ~Page ();

	void           set_page_content (PageInterface &content, void *usr_ptr);
	const PageInterface *
	               get_page_content () const;
	int            get_cursor_node () const;
	void           clear (bool evt_flag = true);

	bool           process_messages ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageMgrInterface
	void           do_set_nav_layout (const NavLocList &nav_list) override;
	void           do_jump_to (int node_id) override;
	void           do_set_timer (int node_id, bool enable_flag) override;
	bool           do_get_shift (Shift key) const override;
	void           do_set_page_step (int step) override;
	void           do_reset_display () override;

	// mfx::uitk::ParentInterface via mfx::uitk::PageMgrInterface
	Vec2d          do_get_coord_abs () const override;
	void           do_invalidate (const Rect &zone) override;

	// mfx::uitk::ContainerInterface via mfx::uitk::PageMgrInterface
	void           do_push_back (NodeSPtr node_sptr) override;
	void           do_set_node (int pos, NodeSPtr node_sptr) override;
	void           do_insert (int pos, NodeSPtr node_sptr) override;
	void           do_erase (int pos) override;
	int            do_get_nbr_nodes () const override;
	NodeSPtr       do_use_node (int pos) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class SetPageContent
	{
	public:
		PageInterface* _content_ptr;
		void *         _usr_ptr;
	};

	bool           process_input ();
	void           handle_redraw ();
	void           send_button (int node_id, Button but);
	void           send_event (const NodeEvt &evt);
	int            find_nav_node (int node_id) const;
	bool           check_curs (bool send_msg_flag);
	bool           process_nav (Button but);
	int            compute_jump_pos (int cur_pos, int dir);
	bool           is_page_step () const;
	int            get_default_page_step () const;

	Model &        _model;
	View &         _view;
	ui::DisplayInterface &
	               _display;
	ui::UserInputInterface::MsgQueue &
	               _queue_input_to_gui;
	ui::UserInputInterface &            // To return message cells
	               _input_device;
	const PageInterface::FontSet
	               _fnt_set;
	const Vec2d    _disp_size;
	NWindow        _screen;
	Rect           _zone_inval;
	PageInterface* _content_ptr;        // 0 = not set

	NavLocList     _nav_list;
	int            _curs_pos;           // -1 = not shown
	int            _curs_id;            // -1 = not shown
	std::set <int> _timer_set;
	bool           _timer_mod_flag;     // Indicates we modified _timer_set while scanning it.

	Button         _but_hold;
	std::chrono::microseconds           // Microseconds, beginning of the hold position
	               _but_hold_date;
	int            _but_hold_count;     // Number of elapsed repetitions
	std::array <bool, _nbr_shift>       // Current state of the shift buttons
	               _shift_arr {};
	int            _page_step;

	std::list <SetPageContent>
	               _rec_spc;
	bool           _recursive_flag;

	std::chrono::microseconds
	               _first_refresh_date;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Page ()                               = delete;
	               Page (const Page &other)              = delete;
	               Page (Page &&other)                   = delete;
	Page &         operator = (const Page &other)        = delete;
	Page &         operator = (Page &&other)             = delete;
	bool           operator == (const Page &other) const = delete;
	bool           operator != (const Page &other) const = delete;

}; // class Page



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/Page.hpp"



#endif   // mfx_uitk_Page_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
