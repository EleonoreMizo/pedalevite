/*****************************************************************************

        MenuSlot.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_MenuSlot_HEADER_INCLUDED)
#define mfx_uitk_pg_MenuSlot_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class MenuSlot
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       MenuSlot (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <std::string> &fx_list, const std::vector <std::string> &ms_list);
	virtual        ~MenuSlot () = default;



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
	virtual void   do_remove_slot (int slot_id);
	virtual void   do_insert_slot_in_chain (int index, int slot_id);
	virtual void   do_erase_slot_from_chain (int index);
	virtual void   do_set_slot_label (int slot_id, std::string name);
	virtual void   do_set_plugin (int slot_id, const PluginInitData &pi_data);
	virtual void   do_remove_plugin (int slot_id);
	virtual void   do_set_plugin_mono (int slot_id, bool mono_flag);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NORMAL = 0,
		State_EDIT_LABEL
	};

	enum Entry
	{
		Entry_WINDOW = 0,
		Entry_TYPE,
		Entry_INSERT,
		Entry_DELETE,
		Entry_MOVE,
		Entry_PRESETS,
		Entry_RESET,
		Entry_CHN,
		Entry_LABEL
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

	void           update_display ();
	EvtProp        change_type (int dir);
	EvtProp        reset_plugin ();
	void           fix_chain_flag ();

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	const std::vector <std::string> &
	               _fx_list;
	const std::vector <std::string> &
	               _ms_list;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	State          _state;
	int            _save_bank_index;
	int            _save_preset_index;
	int            _save_slot_id;

	WinSPtr        _menu_sptr;
	TxtSPtr        _typ_sptr;
	TxtSPtr        _ins_sptr;
	TxtSPtr        _del_sptr;
	TxtSPtr        _mov_sptr;
	TxtSPtr        _prs_sptr;
	TxtSPtr        _rst_sptr;
	TxtSPtr        _chn_sptr;
	TxtSPtr        _lbl_sptr;

	EditText::Param
	               _name_param;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MenuSlot ()                               = delete;
	               MenuSlot (const MenuSlot &other)          = delete;
	MenuSlot &     operator = (const MenuSlot &other)        = delete;
	bool           operator == (const MenuSlot &other) const = delete;
	bool           operator != (const MenuSlot &other) const = delete;

}; // class MenuSlot



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/MenuSlot.hpp"



#endif   // mfx_uitk_pg_MenuSlot_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
