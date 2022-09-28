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

#include "mfx/uitk/pg/EditFxId.h"
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
	class ActionClick;
	class ActionParam;
	class ActionProg;
	class ActionSettings;
	class ActionTempo;
	class ActionTempoSet;
	class ActionToggleFx;
	class ActionToggleTuner;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class PedalEditAction final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalEditAction (PageSwitcher &page_switcher, PedalEditContext &ctx);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_pedalboard_layout (const doc::PedalboardLayout &layout) final;
	void           do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NORMAL = 0,
		State_EDIT_FXID,

		State_NBR_ELT
	};

	enum Entry
	{
		Entry_TYPE = 1000,
		Entry_RELATIVE,
		Entry_INDEX,
		Entry_VALUE,
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
	void           display_prog (PageMgrInterface::NavLocList &nav_list, const doc::ActionProg &action);
	void           display_tuner ();
	void           display_fx (PageMgrInterface::NavLocList &nav_list, const doc::ActionToggleFx &action);
	void           display_loop_rec (PageMgrInterface::NavLocList &nav_list);
	void           display_loop_ps (PageMgrInterface::NavLocList &nav_list);
	void           display_loop_undo (PageMgrInterface::NavLocList &nav_list);
	void           display_param (PageMgrInterface::NavLocList &nav_list, const doc::ActionParam &action);
	void           display_tempo_tap ();
	void           display_settings (PageMgrInterface::NavLocList &nav_list, const doc::ActionSettings &action);
	void           display_event (PageMgrInterface::NavLocList &nav_list);
	void           display_tempo_set (PageMgrInterface::NavLocList &nav_list, const doc::ActionTempoSet &action);
	void           display_click (PageMgrInterface::NavLocList &nav_list, const doc::ActionClick &action);
	std::string    print_fx_id (const doc::FxId &fx_id) const;
	EvtProp        change_value (int node_id, int dir);
	EvtProp        change_type (int dir);
	EvtProp        change_bank (int node_id, int dir);
	EvtProp        change_prog (int node_id, int dir);
	EvtProp        change_fx (int node_id, int dir);
	EvtProp        change_param (int node_id, int dir);
	EvtProp        change_settings (int node_id, int dir);
	EvtProp        change_tempo_set (int dir);
	EvtProp        change_click (int node_id, int dir);

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

	State          _state;
	EditFxId::Param
	               _arg_edit_fxid;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalEditAction ()                               = delete;
	               PedalEditAction (const PedalEditAction &other)   = delete;
	               PedalEditAction (PedalEditAction &&other)        = delete;
	PedalEditAction &
	               operator = (const PedalEditAction &other)        = delete;
	PedalEditAction &
	               operator = (PedalEditAction &&other)             = delete;
	bool           operator == (const PedalEditAction &other) const = delete;
	bool           operator != (const PedalEditAction &other) const = delete;

}; // class PedalEditAction



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalEditAction.hpp"



#endif   // mfx_uitk_pg_PedalEditAction_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
