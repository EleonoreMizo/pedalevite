/*****************************************************************************

        SlotRouting.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_SlotRouting_HEADER_INCLUDED)
#define mfx_uitk_pg_SlotRouting_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Dir.h"
#include "mfx/uitk/pg/SlotRoutingAction.h"
#include "mfx/uitk/pg/Tools.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/ToolsRouting.h"

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



class SlotRouting final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SlotRouting (PageSwitcher &page_switcher, LocEdit &loc_edit);



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

	static const int  _s_dir    = 12;
	static const int  _s_pin    =  8;
	static const int  _mask_cnx = ((1 <<           _s_pin)  - 1);
	static const int  _mask_pin = ((1 << (_s_dir - _s_pin)) - 1);
	static const int  _ofs_name = _mask_cnx;

	enum Entry
	{
		Entry_WINDOW = 0,
		Entry_MOVE,
		Entry_IO
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

	class Cnx
	{
	public:
		doc::Cnx       _cnx;
		TxtSPtr        _label_sptr;
	};
	class Pin
	{
	public:
		TxtSPtr        _name_sptr;
		std::vector <Cnx>
		               _cnx_arr;
	};
	typedef std::vector <Pin> Side;
	typedef std::array <Side, piapi::Dir_NBR_ELT> SideArray;

	enum IoType
	{
		IoType_INVALID = -1,
		IoType_NAME = 0,
		IoType_CNX,

		IoType_NBR_ELT
	};

	void           update_display ();
	void           list_pin (int &pos_y, PageMgrInterface::NavLocList &nav_list, Pin &pin, int pin_idx, int nbr_pins, int nbr_pins_gra, piapi::Dir dir, const std::vector <Tools::NodeEntry> &entry_list, bool exist_flag, bool node_flag, ToolsRouting::NodeMap::const_iterator it_node);
	void           list_pin_cnx (int &pos_y, PageMgrInterface::NavLocList &nav_list, Pin &pin, int pin_idx, piapi::Dir dir, const std::vector <Tools::NodeEntry> &entry_list, const ToolsRouting::CnxSet &cnx_set);
	EvtProp        sel_pin_cnx (int node_id);

	static int     conv_cnx_to_node_id (IoType type, piapi::Dir dir, int pin_idx, int cnx_idx);
	static IoType  conv_node_id_to_cnx (piapi::Dir &dir, int &pin_idx, int &cnx_idx, int node_id);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;
	TxtSPtr        _mov_sptr;
	SideArray      _side_arr;

	SlotRoutingAction::Arg
	               _action_arg;

	static const char *
	               _indent_0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SlotRouting ()                               = delete;
	               SlotRouting (const SlotRouting &other)       = delete;
	SlotRouting &  operator = (const SlotRouting &other)        = delete;
	bool           operator == (const SlotRouting &other) const = delete;
	bool           operator != (const SlotRouting &other) const = delete;

}; // class SlotRouting



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/SlotRouting.hpp"



#endif   // mfx_uitk_pg_SlotRouting_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

