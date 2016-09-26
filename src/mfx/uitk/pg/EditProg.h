/*****************************************************************************

        EditProg.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EditProg_HEADER_INCLUDED)
#define mfx_uitk_pg_EditProg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>
#include <string>
#include <vector>



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class EditProg
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       EditProg (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <std::string> &fx_list);
	virtual        ~EditProg () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_activate_preset (int index);
	virtual void   do_set_preset_name (std::string name);
	virtual void   do_set_nbr_slots (int nbr_slots);
	virtual void   do_insert_slot (int slot_index);
	virtual void   do_erase_slot (int slot_index);
	virtual void   do_set_plugin (int slot_index, const PluginInitData &pi_data);
	virtual void   do_remove_plugin (int slot_index);
	virtual void   do_set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NORMAL = 0,
		State_EDIT_NAME
	};

	enum Entry
	{
		Entry_WINDOW    = 1000,
		Entry_FX_LIST,
		Entry_PROG_NAME,
		Entry_CONTROLLERS,
		Entry_SAVE
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           set_preset_info ();
	void           set_slot (PageMgrInterface::NavLocList &nav_list, int slot_index, std::string multilabel, bool bold_flag);
	EvtProp        change_effect (int node_id, int dir);
	void           update_loc_edit (int node_id);
	void           update_rotenc_mapping ();
	int            conv_loc_edit_to_node_id () const;

	const std::vector <std::string> &
	               _fx_list;
	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains 3 entries (2 of them are selectable) + the slot list
	TxtSPtr        _fx_list_sptr;
	TxtSPtr        _prog_name_sptr;
	TxtSPtr        _controllers_sptr;
	TxtSPtr        _save_sptr;
	TxtArray       _slot_list;    // Shows N+1 slots, the last one being the <Empty> line.

	State          _state;
	int            _save_bank_index;
	int            _save_preset_index;
	EditText::Param
	               _name_param;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EditProg ()                               = delete;
	               EditProg (const EditProg &other)          = delete;
	EditProg &     operator = (const EditProg &other)        = delete;
	bool           operator == (const EditProg &other) const = delete;
	bool           operator != (const EditProg &other) const = delete;

}; // class EditProg



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EditProg.hpp"



#endif   // mfx_uitk_pg_EditProg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
