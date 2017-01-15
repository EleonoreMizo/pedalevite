/*****************************************************************************

        PedalEditAction.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalEditAction_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalEditAction_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>



namespace mfx
{

namespace doc
{
	class ActionBank;
	class ActionParam;
	class ActionPreset;
	class ActionSettings;
	class ActionTempo;
	class ActionToggleFx;
	class ActionToggleTuner;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class PedalEditAction
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalEditAction (PageSwitcher &page_switcher, PedalEditContext &ctx);
	virtual        ~PedalEditAction () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout);
	virtual void   do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_TYPE = 1000,
		Entry_RELATIVE,
		Entry_INDEX,
		Entry_VALUE,
		Entry_LOC_TYPE,
		Entry_LABEL,
		Entry_UNIT,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	const doc::PedalboardLayout &
	               use_layout () const;
	void           check_ctx ();
	void           update_model ();
	void           update_display ();
	void           hide_all ();
	void           display_bank (PageMgrInterface::NavLocList &nav_list, const doc::ActionBank &action);
	void           display_preset (PageMgrInterface::NavLocList &nav_list, const doc::ActionPreset &action);
	void           display_tuner (PageMgrInterface::NavLocList &nav_list);
	void           display_fx (PageMgrInterface::NavLocList &nav_list, const doc::ActionToggleFx &action);
	void           display_loop_rec (PageMgrInterface::NavLocList &nav_list);
	void           display_loop_ps (PageMgrInterface::NavLocList &nav_list);
	void           display_loop_undo (PageMgrInterface::NavLocList &nav_list);
	void           display_param (PageMgrInterface::NavLocList &nav_list, const doc::ActionParam &action);
	void           display_tempo (PageMgrInterface::NavLocList &nav_list, const doc::ActionTempo &action);
	void           display_settings (PageMgrInterface::NavLocList &nav_list, const doc::ActionSettings &action);
	void           display_event (PageMgrInterface::NavLocList &nav_list);
	EvtProp        change_value (int node_id, int dir);

	PageSwitcher & _page_switcher;
	PedalEditContext &
	               _ctx;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	TxtSPtr        _type_sptr;
	TxtSPtr        _index_sptr;
	TxtSPtr        _value_sptr;
	TxtSPtr        _loc_type_sptr;
	TxtSPtr        _label_sptr;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalEditAction ()                               = delete;
	               PedalEditAction (const PedalEditAction &other)   = delete;
	PedalEditAction &
	               operator = (const PedalEditAction &other)        = delete;
	bool           operator == (const PedalEditAction &other) const = delete;
	bool           operator != (const PedalEditAction &other) const = delete;

}; // class PedalEditAction



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalEditAction.hpp"



#endif   // mfx_uitk_pg_PedalEditAction_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
