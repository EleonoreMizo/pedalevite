/*****************************************************************************

        PedalActionType.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalActionType_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalActionType_HEADER_INCLUDED

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
namespace uitk
{

class PageSwitcher;

namespace pg
{



class PedalActionType
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalActionType (PageSwitcher &page_switcher, PedalEditContext &ctx);
	virtual        ~PedalActionType () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
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
		Entry_TITLE = 1000,
		Entry_TITLE2,
		Entry_FULL,
		Entry_CLEAR,
		Entry_PUSH,
		Entry_MOMENTARY,
		Entry_TOGGLE,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	const doc::PedalboardLayout &
	               use_layout () const;
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
	TxtSPtr        _title2_sptr;
	TxtSPtr        _full_sptr;
	TxtSPtr        _clear_sptr;
	TxtSPtr        _push_sptr;
	TxtSPtr        _momentary_sptr;
	TxtSPtr        _toggle_sptr;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalActionType ()                               = delete;
	               PedalActionType (const PedalActionType &other)   = delete;
	PedalActionType &
	               operator = (const PedalActionType &other)        = delete;
	bool           operator == (const PedalActionType &other) const = delete;
	bool           operator != (const PedalActionType &other) const = delete;

}; // class PedalActionType



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalActionType.hpp"



#endif   // mfx_uitk_pg_PedalActionType_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
