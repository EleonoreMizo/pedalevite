/*****************************************************************************

        ProgEdit.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ProgEdit_HEADER_INCLUDED)
#define mfx_uitk_pg_ProgEdit_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/pg/Tools.h"
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



class ProgEdit final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ProgEdit (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_prog (int index) final;
	void           do_set_prog_name (std::string name) final;
	void           do_add_slot (int slot_id) final;
	void           do_remove_slot (int slot_id) final;
	void           do_set_routing (const doc::Routing &routing) final;
	void           do_set_plugin (int slot_id, const PluginInitData &pi_data) final;
	void           do_remove_plugin (int slot_id) final;
	void           do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls) final;
	void           do_enable_auto_rotenc_override (bool ovr_flag) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NORMAL = 0,
		State_EDIT_NAME
	};

	enum Entry
	{
		Entry_LINKS     = 1000,
		Entry_WINDOW    = 2000,
		Entry_FX_LIST,
		Entry_MS_LIST,
		Entry_PROG_NAME,
		Entry_SETTINGS,
		Entry_SAVE
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	enum Link
	{
		Link_NONE = 0,
		Link_CHAIN,
		Link_BRANCH,
		Link_BROKEN,   // This one could be an additional flag, not excluding other values.

		Link_NBR_ELT
	};

	class SlotDispParam
	{
	public:
		explicit       SlotDispParam (int pos_list, int chain_size, const ui::Font &fnt, const Vec2d &page_size);
		const int      _w_m;
		const int      _h_m;
		const int      _scr_w;
		const int      _skip;
		const int      _pos_menu;
		const int      _txt_x;
		const int      _margin;
	};

	void           set_prog_info ();
	void           update_display ();
	std::vector <Link>
	               find_chain_links (const std::vector <Tools::NodeEntry> &entry_list) const;
	void           find_chain_links_dir (std::vector <Link> &link_list, int slot_id, piapi::Dir dir, const std::vector <Tools::NodeEntry> &entry_list) const;
	void           set_link (std::vector <Link> &link_list, int slot_id, Link link, const std::vector <Tools::NodeEntry> &entry_list) const;
	void           find_broken_links (std::vector <Link> &link_list, const std::vector <Tools::NodeEntry> &entry_list) const;
	void           add_slot_x (PageMgrInterface::NavLocList &nav_list, std::string multilabel, int pos_list);
	void           update_slot (int pos_list, std::string link_txt, bool bold_flag);
	void           set_slot (PageMgrInterface::NavLocList &nav_list, int pos_list, std::string multilabel, std::string link_txt, bool bold_flag);
	EvtProp        change_effect (int node_id, int dir);
	void           update_loc_edit (int node_id);
	void           update_cached_pi_list ();
	void           update_rotenc_mapping ();
	int            conv_slot_index_to_pos_list (int slot_index) const;
	int            conv_node_id_to_slot_id (int node_id) const;
	int            conv_node_id_to_slot_id (int node_id, bool &chain_flag) const;
	int            conv_loc_edit_to_node_id () const;
	bool           is_node_id_from_slot_list (int node_id) const;

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
	TxtSPtr        _ms_list_sptr;
	TxtSPtr        _prog_name_sptr;
	TxtSPtr        _settings_sptr;
	TxtSPtr        _save_sptr;
	TxtArray       _slot_list;   // Shows N+1+M+1 slots, the last one of each list being the <End> line.
	TxtArray       _rout_list;   // Link circles in front of N+1+M+1 slots

	State          _state;
	int            _save_bank_index;
	int            _save_prog_index;
	EditText::Param
	               _name_param;
	std::vector <int>             // Ordered list of the edited slots
	               _slot_id_list;
	int            _audio_list_len;  // Cached size of the audio part of the list
	bool           _spi_flag;     // Indicates we're in set_prog_info(); avoids recursion.
	bool           _reset_end_curs_flag; // When switching to a new prog and the cursor is on an <End> entry, put it at the beginning of the list



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProgEdit ()                               = delete;
	               ProgEdit (const ProgEdit &other)          = delete;
	               ProgEdit (ProgEdit &&other)               = delete;
	ProgEdit &     operator = (const ProgEdit &other)        = delete;
	ProgEdit &     operator = (ProgEdit &&other)             = delete;
	bool           operator == (const ProgEdit &other) const = delete;
	bool           operator != (const ProgEdit &other) const = delete;

}; // class ProgEdit



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ProgEdit.hpp"



#endif   // mfx_uitk_pg_ProgEdit_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
