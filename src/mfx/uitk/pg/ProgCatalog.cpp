/*****************************************************************************

        ProgCatalog.cpp
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

#include "fstb/def.h"
#include "mfx/ui/Font.h"
#include "mfx/uitk/pg/ProgCatalog.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/Model.h"
#include "mfx/View.h"

#include <cassert>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ProgCatalog::ProgCatalog (PageSwitcher &page_switcher)
:	_page_switcher (page_switcher)
,	_model_ptr (0)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_fnt_ptr (0)
,	_menu_sptr (new NWindow (Entry_WINDOW))
,	_prog_list ()
,	_prog_coord_list ()
,	_active_node_id (-1)
{
	// Nothing
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgCatalog::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt)
{
	fstb::unused (usr_ptr);

	_model_ptr = &model;
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_ptr   = &fnt._m;

	const int      h_m   = _fnt_ptr->get_char_h ();
	const int      scr_w = _page_size [0];

	_menu_sptr->set_size (_page_size, Vec2d ());
	_menu_sptr->set_disp_pos (Vec2d ());
	_page_ptr->push_back (_menu_sptr);

	PageMgrInterface::NavLocList  nav_list;

	UniqueProgList upl;
	_prog_coord_list = upl.build (view);
	const doc::Setup &   setup = view.use_setup ();
	const size_t   nbr_prog = _prog_coord_list.size ();
	_prog_list.clear ();
	_prog_list.resize (nbr_prog);
	for (size_t pos = 0; pos < nbr_prog; ++pos)
	{
		const int      node_id = conv_index_to_node_id (int (pos));
		TxtSPtr        txt_sptr (new NText (node_id));
		const UniqueProgList::ProgCoord &   coord = _prog_coord_list [pos];
		const doc::Preset &   prog =
			setup._bank_arr [coord._bank]._preset_arr [coord._prog];

		txt_sptr->set_font (*_fnt_ptr);
		txt_sptr->set_coord (Vec2d (0, int (pos) * h_m));
		txt_sptr->set_frame (Vec2d (scr_w, 0), Vec2d ());
		txt_sptr->set_text (prog._name);

		_prog_list [pos] = txt_sptr;
		_menu_sptr->push_back (txt_sptr);
		PageMgrInterface::add_nav (nav_list, node_id);
	}
	_page_ptr->set_nav_layout (nav_list);
	_page_ptr->jump_to (Entry_BASE);
	_active_node_id = Entry_BASE;

	_menu_sptr->invalidate_all ();
}



void	ProgCatalog::do_disconnect ()
{
	// Nothing
}



MsgHandlerInterface::EvtProp	ProgCatalog::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	const int      node_id = evt.get_target ();

	if (node_id != _active_node_id && is_valid_prog (node_id))
	{
		const int      prog_pos = conv_node_id_to_index (node_id);
		const UniqueProgList::ProgCoord &   coord = _prog_coord_list [prog_pos];

		_model_ptr->select_bank (coord._bank);
		_model_ptr->activate_preset (coord._prog);
			
		_active_node_id = node_id;
	}

	if (evt.is_button_ex ())
	{
		const Button   but = evt.get_button_ex ();
		switch (but)
		{
		case Button_S:
		case Button_E:
			_page_switcher.return_page ();
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
	}

	return ret_val;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	ProgCatalog::is_valid_prog (int node_id) const
{
	return (
		   node_id >= Entry_BASE
		&& node_id - Entry_BASE < int (_prog_list.size ())
	);
}



int	ProgCatalog::conv_node_id_to_index (int node_id) const
{
	assert (node_id >= Entry_BASE);
	assert (node_id < Entry_WINDOW);

	return node_id - Entry_BASE;
}



int	ProgCatalog::conv_index_to_node_id (int index) const
{
	assert (index >= 0);
	assert (index < Entry_WINDOW - Entry_BASE);

	return Entry_BASE + index;
}



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
