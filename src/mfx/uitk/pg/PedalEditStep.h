/*****************************************************************************

        PedalEditStep.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalEditStep_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalEditStep_HEADER_INCLUDED

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

class PedalEditStep final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalEditStep (PageSwitcher &page_switcher, PedalEditContext &ctx);



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
		Entry_ACTION_LIST =    0,

		Entry_WINDOW      = 1000,
		Entry_TITLE,
		Entry_ADD,
		Entry_UP,
		Entry_DOWN,
		Entry_DELETE,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> ActionList;
	typedef std::shared_ptr <NWindow> WinSPtr;

	const doc::PedalboardLayout &
	               use_layout () const;
	void           check_ctx ();
	void           update_model ();
	void           update_display ();

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
	TxtSPtr        _add_sptr;
	TxtSPtr        _up_sptr;
	TxtSPtr        _down_sptr;
	TxtSPtr        _del_sptr;
	WinSPtr        _menu_sptr;
	ActionList     _action_sptr_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalEditStep ()                               = delete;
	               PedalEditStep (const PedalEditStep &other)     = delete;
	               PedalEditStep (PedalEditStep &&other)          = delete;
	PedalEditStep& operator = (const PedalEditStep &other)        = delete;
	PedalEditStep& operator = (PedalEditStep &&other)             = delete;
	bool           operator == (const PedalEditStep &other) const = delete;
	bool           operator != (const PedalEditStep &other) const = delete;

}; // class PedalEditStep



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalEditStep.hpp"



#endif   // mfx_uitk_pg_PedalEditStep_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
