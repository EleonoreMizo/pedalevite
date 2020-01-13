/*****************************************************************************

        PedalEditCycle.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalEditCycle_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalEditCycle_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class PedalEditContext;

class PedalEditCycle final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalEditCycle (PageSwitcher &page_switcher, PedalEditContext &ctx);



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

	enum Entry
	{
		Entry_STEP_LIST =    0,

		Entry_WINDOW     = 1000,
		Entry_TITLE,
		Entry_INHERIT,
		Entry_OVERRIDABLE,
		Entry_RESET,
		Entry_ADD,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> StepList;
	typedef std::shared_ptr <NWindow> WinSPtr;

	const doc::PedalboardLayout &
	               use_layout () const;
	void           check_ctx ();
	void           update_model ();
	void           update_display ();
	void           show_flag (NText &ctrl, const char *name_0, int name_size, bool flag);

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

	TxtSPtr        _title_sptr;
	TxtSPtr        _inherit_sptr;
	TxtSPtr        _overridable_sptr;
	TxtSPtr        _reset_sptr;
	TxtSPtr        _add_sptr;
	WinSPtr        _menu_sptr;
	StepList       _step_sptr_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalEditCycle ()                               = delete;
	               PedalEditCycle (const PedalEditCycle &other)    = delete;
	               PedalEditCycle (PedalEditCycle &&other)         = delete;
	PedalEditCycle &
	               operator = (const PedalEditCycle &other)        = delete;
	PedalEditCycle &
	               operator = (PedalEditCycle &&other)             = delete;
	bool           operator == (const PedalEditCycle &other) const = delete;
	bool           operator != (const PedalEditCycle &other) const = delete;

}; // class PedalEditCycle



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalEditCycle.hpp"



#endif   // mfx_uitk_pg_PedalEditCycle_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
