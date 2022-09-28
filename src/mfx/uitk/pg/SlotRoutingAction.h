/*****************************************************************************

        SlotRoutingAction.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_SlotRoutingAction_HEADER_INCLUDED)
#define mfx_uitk_pg_SlotRoutingAction_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Dir.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <array>
#include <vector>



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class SlotRoutingAction final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum EdType
	{
		EdType_INVALID = -1,

		EdType_CNX = 0,
		EdType_PIN,

		EdType_NBR_ELT
	};

	class Arg
	{
	public:
		EdType         _ed_type = EdType_INVALID;     // I/-, edit connection or pin
		piapi::Dir     _dir     = piapi::Dir_INVALID; // I/-, edited direction (target is the opposite)
		doc::Cnx       _cnx;          // I/-, edited connection
		int            _pin_idx = -1; // I/-, edited pin
	};

	explicit       SlotRoutingAction (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_prog (int index) final;
	void           do_remove_slot (int slot_id) final;
	void           do_set_routing (const doc::Routing &routing) final;
	void           do_set_plugin (int slot_id, const PluginInitData &pi_data) final;
	void           do_remove_plugin (int slot_id) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 0,
		Entry_TITLE,
		Entry_DEL,
		Entry_REPLACE,
		Entry_TARGET
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

	class CnxEndData
	{
	public:
		doc::CnxEnd   _cnx_end;
		TxtSPtr       _txt_sptr;
	};

	typedef std::vector <CnxEndData> CnxEndArray;

	void           update_display ();
	void           build_possible_cnx_set (PageMgrInterface::NavLocList &nav_list, int y_pos);
	void           list_target (PageMgrInterface::NavLocList &nav_list, int &nid, int &pos_y, const doc::CnxEnd &cnx_end, const std::set <doc::CnxEnd> &ce_excl_list, int nbr_pins, const std::vector <Tools::NodeEntry> &entry_list);

	void           del_cur_cnx ();
	void           add_or_replace_cnx (int node_id);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;
	const Arg *    _arg_ptr;

	WinSPtr        _menu_sptr;
	TxtSPtr        _tit_sptr;
	TxtSPtr        _del_sptr;
	TxtSPtr        _rep_sptr;

	CnxEndArray    _cnx_end_arr;

	static const char *
	               _indent_0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SlotRoutingAction ()                               = delete;
	               SlotRoutingAction (const SlotRoutingAction &other) = delete;
	SlotRoutingAction &
	               operator = (const SlotRoutingAction &other)        = delete;
	bool           operator == (const SlotRoutingAction &other) const = delete;
	bool           operator != (const SlotRoutingAction &other) const = delete;

}; // class SlotRoutingAction



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/SlotRoutingAction.hpp"



#endif   // mfx_uitk_pg_SlotRoutingAction_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
