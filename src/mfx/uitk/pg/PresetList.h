/*****************************************************************************

        PresetList.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PresetList_HEADER_INCLUDED)
#define mfx_uitk_pg_PresetList_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class PresetList final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Action
	{
		Action_INVALID = -1,
		Action_LOAD    = 0,
		Action_BROWSE,
		Action_STORE,
		Action_SWAP,
		Action_RENAME,
		Action_MORPH,
		Action_DELETE,

		Action_NBR_ELT
	};

	class Param
	{
	public:
		Action         _action = Action_INVALID;
	};

	enum State
	{
		State_NORMAL = 0,
		State_EDIT_NAME,

		State_NBR_ELT
	};

	explicit       PresetList (PageSwitcher &page_switcher, LocEdit &loc_edit);



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
	void           do_set_plugin (int slot_id, const PluginInitData &pi_data) final;
	void           do_remove_plugin (int slot_id) final;
	void           do_add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix) final;
	void           do_remove_settings (std::string model, int index) final;
	void           do_clear_all_settings () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1'000'000
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;
	typedef std::map <std::string, int> PosMap;  // [fx_type] = index in the list
	typedef std::array <doc::PluginSettings, PiType_NBR_ELT> FullSettings;

	void           update_display ();
	void           add_entry (int set_idx, std::string name, PageMgrInterface::NavLocList &nav_list, bool same_flag);
	bool           load (int set_idx);
	void           store_1 (int set_idx);
	void           store_2 ();
	void           swap (int set_idx);
	void           rename_1 (int set_idx);
	void           rename_2 ();
	void           del (int set_idx);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains the preset list
	TxtArray       _preset_list;
	PosMap         _preset_pos_map;
	Action         _action;
	State          _state;
	EditText::Param
	               _name_param;

	// Index of the selected preset. We store it before calling a sub-screen,
	// so when we are back from this sub-screen, we can do the operation on the
	// selected settings. < 0: no index stored.
	int            _state_set_idx;

	// When browsing, we save the original settings here.
	// When the pointer is empty, it means that the new settings are validated.
	// Otherwise, we restore them when leaving the screen.
	std::shared_ptr <FullSettings>
	               _saved_settings_sptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PresetList ()                               = delete;
	               PresetList (const PresetList &other)        = delete;
	               PresetList (PresetList &&other)             = delete;
	PresetList &   operator = (const PresetList &other)        = delete;
	PresetList &   operator = (PresetList &&other)             = delete;
	bool           operator == (const PresetList &other) const = delete;
	bool           operator != (const PresetList &other) const = delete;

}; // class PresetList



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PresetList.hpp"



#endif   // mfx_uitk_pg_PresetList_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
