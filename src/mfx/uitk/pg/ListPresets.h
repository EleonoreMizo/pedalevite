/*****************************************************************************

        ListPresets.h
        Copyright (c) 2017 Ohm Force

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ListPresets_HEADER_INCLUDED)
#define mfx_uitk_pg_ListPresets_HEADER_INCLUDED

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



class ListPresets
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

	explicit       ListPresets (PageSwitcher &page_switcher, LocEdit &loc_edit);
	virtual        ~ListPresets () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_activate_preset (int index);
	virtual void   do_remove_slot (int slot_id);
	virtual void   do_set_plugin (int slot_id, const PluginInitData &pi_data);
	virtual void   do_remove_plugin (int slot_id);
	virtual void   do_add_settings (std::string model, int index, std::string name, const doc::PluginSettings &s_main, const doc::PluginSettings &s_mix);
	virtual void   do_remove_settings (std::string model, int index);
	virtual void   do_clear_all_settings ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1000000
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;
	typedef std::map <std::string, int> PosMap;  // [fx_type] = index in the list

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
	int            _state_set_idx;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ListPresets ()                               = delete;
	               ListPresets (const ListPresets &other)       = delete;
	ListPresets &  operator = (const ListPresets &other)        = delete;
	bool           operator == (const ListPresets &other) const = delete;
	bool           operator != (const ListPresets &other) const = delete;

}; // class ListPresets



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ListPresets.hpp"



#endif   // mfx_uitk_pg_ListPresets_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
