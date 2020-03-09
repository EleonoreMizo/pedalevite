/*****************************************************************************

        SlotMenu.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_SlotMenu_HEADER_INCLUDED)
#define mfx_uitk_pg_SlotMenu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditLabel.h"
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



class SlotMenu final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SlotMenu (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_preset (int index) final;
	void           do_remove_slot (int slot_id) final;
	void           do_set_routing (const doc::Routing &routing) final;
	void           do_set_slot_label (int slot_id, std::string name) final;
	void           do_set_plugin (int slot_id, const PluginInitData &pi_data) final;
	void           do_remove_plugin (int slot_id) final;
	void           do_set_plugin_mono (int slot_id, bool mono_flag) final;
	void           do_set_plugin_reset (int slot_id, bool reset_flag) final;



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
		Entry_INS_BFR,
		Entry_INS_AFT,
		Entry_DELETE,
		Entry_ROUTING,
		Entry_PRESETS,
		Entry_RESET,
		Entry_CHN,
		Entry_FRESH,
		Entry_LABEL
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

	void           update_display ();
	EvtProp        change_type (int dir);
	void           reset_plugin ();
	void           insert_slot_before ();
	void           insert_slot_after ();
	void           delete_slot ();
	void           change_chn_pref ();
	void           change_state_mode ();
	void           set_label (int node_id);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
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
	TxtSPtr        _inb_sptr;
	TxtSPtr        _ina_sptr;
	TxtSPtr        _del_sptr;
	TxtSPtr        _rtn_sptr;
	TxtSPtr        _prs_sptr;
	TxtSPtr        _rst_sptr;
	TxtSPtr        _chn_sptr;
	TxtSPtr        _frs_sptr;
	TxtSPtr        _lbl_sptr;

	EditLabel::Param
	               _label_param;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SlotMenu ()                               = delete;
	               SlotMenu (const SlotMenu &other)          = delete;
	               SlotMenu (SlotMenu &&other)               = delete;
	SlotMenu &     operator = (const SlotMenu &other)        = delete;
	SlotMenu &     operator = (SlotMenu &&other)             = delete;
	bool           operator == (const SlotMenu &other) const = delete;
	bool           operator != (const SlotMenu &other) const = delete;

}; // class SlotMenu



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/SlotMenu.hpp"



#endif   // mfx_uitk_pg_SlotMenu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
