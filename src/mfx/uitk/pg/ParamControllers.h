/*****************************************************************************

        ParamControllers.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ParamControllers_HEADER_INCLUDED)
#define mfx_uitk_pg_ParamControllers_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/PluginModel.h"
#include "mfx/uitk/pg/CtrlSrcNamed.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>
#include <vector>



namespace mfx
{

class LocEdit;

namespace doc
{
	class CtrlLinkSet;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class ParamControllers
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ParamControllers (PageSwitcher &page_switcher, LocEdit &loc_edit, const std::vector <CtrlSrcNamed> &csn_list);
	virtual        ~ParamControllers () = default;



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
	virtual void   do_remove_plugin (int slot_index);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1000,
		Entry_LINK_VALUE,
		Entry_LINK_TITLE,
		Entry_MOD_TITLE
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           set_controller_info ();
	void           update_loc_edit (int node_id);
	int            conv_loc_edit_to_node_id () const;

	const std::vector <CtrlSrcNamed> &
	               _csn_list;
	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains 3 entries (1 of them is selectable) + the modulation list
	TxtSPtr        _link_value_sptr;
	TxtSPtr        _link_title_sptr;
	TxtSPtr        _mod_title_sptr;
	TxtArray       _mod_list;     // Shows N lines: controllers 1 to N-1, and the last line being <Empty>.

	// Cached stuff
	const doc::CtrlLinkSet *      // 0 if not set. Updated on connect();
	               _cls_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ParamControllers ()                                = delete;
	               ParamControllers (const ParamControllers &other)   = delete;
	ParamControllers &     operator = (const ParamControllers &other) = delete;
	bool           operator == (const ParamControllers &other) const  = delete;
	bool           operator != (const ParamControllers &other) const  = delete;

}; // class ParamControllers



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ParamControllers.hpp"



#endif   // mfx_uitk_pg_ParamControllers_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
