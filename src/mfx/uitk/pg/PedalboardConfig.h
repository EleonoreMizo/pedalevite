/*****************************************************************************

        PedalboardConfig.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalboardConfig_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalboardConfig_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/Cst.h"

#include <array>
#include <memory>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class PedalboardConfig final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PedalboardConfig (PageSwitcher &page_switcher, PedalEditContext &ctx);



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
		Entry_WINDOW = 1000,
		Entry_PEDAL_LIST
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::array <TxtSPtr, Cst::_nbr_pedals> TxtArray;

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

	WinSPtr        _menu_sptr;
	TxtArray       _pedal_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PedalboardConfig ()                               = delete;
	               PedalboardConfig (const PedalboardConfig &other)  = delete;
	               PedalboardConfig (PedalboardConfig &&other)       = delete;
	PedalboardConfig &
	               operator = (const PedalboardConfig &other)        = delete;
	PedalboardConfig &
	               operator = (PedalboardConfig &&other)             = delete;
	bool           operator == (const PedalboardConfig &other) const = delete;
	bool           operator != (const PedalboardConfig &other) const = delete;

}; // class PedalboardConfig



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalboardConfig.hpp"



#endif   // mfx_uitk_pg_PedalboardConfig_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
