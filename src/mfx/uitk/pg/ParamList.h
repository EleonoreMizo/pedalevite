/*****************************************************************************

        ParamList.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ParamList_HEADER_INCLUDED)
#define mfx_uitk_pg_ParamList_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PageType.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>
#include <vector>



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class ParamList final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ParamList (PageSwitcher &page_switcher, LocEdit &loc_edit);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_activate_prog (int index) final;
	void           do_set_param (int slot_id, int index, float val, PiType type) final;
	void           do_remove_plugin (int slot_id) final;
	void           do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1000000,
		Entry_FX_SETUP,
		Entry_GUI
	};

	enum Col
	{
		Col_NAME = 0,
		Col_VAL,
		Col_UNIT,

		Col_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	typedef std::array <TxtArray, Col_NBR_ELT> ColArray;
	typedef std::array <int, Col_NBR_ELT> NodeIdRow;

	void           set_param_info ();
	void           check_gui (const std::string &pi_model);
	void           update_param_txt (PiType type, int index);
	void           update_loc_edit (int node_id);
	int            get_param_list_length () const;
	int            conv_loc_edit_to_node_id () const;
	bool           conv_param_to_node_id (NodeIdRow &nid_row, PiType type, int index) const;
	void           conv_node_id_to_param (PiType &type, int &index, int node_id);
	int            conv_param_to_list_pos (PiType type, int index) const;
	bool           is_nid_in_param_list (int node_id) const;
	EvtProp        change_param (int node_id, int dir);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains 1 or 2 entries + the parameter list
	TxtSPtr        _fx_setup_sptr;
	TxtSPtr        _gui_sptr;
	ColArray       _param_list;   // First the mixer parameters, then the plug-in parameters.
	bool           _mixer_flag;   // Indicates that we should display the mixer parameter (not needed for signal generators)
	bool           _gui_flag;     // We have a GUI to edit this effect
	PageType       _gui_page;
	bool           _unit_flag;    // UI is wide enough to display the parameter unit



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ParamList ()                               = delete;
	               ParamList (const ParamList &other)         = delete;
	               ParamList (ParamList &&other)              = delete;
	ParamList &    operator = (const ParamList &other)        = delete;
	ParamList &    operator = (ParamList &&other)             = delete;
	bool           operator == (const ParamList &other) const = delete;
	bool           operator != (const ParamList &other) const = delete;

}; // class ParamList



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ParamList.hpp"



#endif   // mfx_uitk_pg_ParamList_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
