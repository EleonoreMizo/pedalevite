/*****************************************************************************

        PresetMenu.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PresetMenu_HEADER_INCLUDED)
#define mfx_uitk_pg_PresetMenu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PresetList.h"
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



class PresetMenu final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PresetMenu (PageSwitcher &page_switcher, LocEdit &loc_edit);



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



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1000,

		Entry_LOAD,
		Entry_BROWSE,
		Entry_STORE,
		Entry_SWAP,
		Entry_RENAME,
		Entry_MORPH,
		Entry_DELETE,
		Entry_ORGANIZE
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;

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
	TxtSPtr        _load_sptr;
	TxtSPtr        _brow_sptr;
	TxtSPtr        _stor_sptr;
	TxtSPtr        _swap_sptr;
	TxtSPtr        _renm_sptr;
	TxtSPtr        _mrph_sptr;
	TxtSPtr        _dele_sptr;
	TxtSPtr        _orga_sptr;

	PresetList::Param
	               _lp_param;
	int            _node_id_save;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PresetMenu ()                               = delete;
	               PresetMenu (const PresetMenu &other)        = delete;
	               PresetMenu (PresetMenu &&other)             = delete;
	PresetMenu &   operator = (const PresetMenu &other)        = delete;
	PresetMenu &   operator = (PresetMenu &&other)             = delete;
	bool           operator == (const PresetMenu &other) const = delete;
	bool           operator != (const PresetMenu &other) const = delete;

}; // class PresetMenu



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PresetMenu.hpp"



#endif   // mfx_uitk_pg_PresetMenu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
