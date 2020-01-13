/*****************************************************************************

        MenuMain.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_MenuMain_HEADER_INCLUDED)
#define mfx_uitk_pg_MenuMain_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <memory>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class MenuMain final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       MenuMain (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NORMAL = 0,
		State_REBOOT
	};

	enum RestartMenu
	{
		RestartMenu_CANCEL = 0,
		RestartMenu_RESTART,
		RestartMenu_REBOOT,
		RestartMenu_SHUTDOWN,

		RestartMenu_NBR_ELT
	};

	enum Entry
	{
		Entry_PROG  = 0,
		Entry_BANKS ,
		Entry_LAYOUT,
		Entry_LEVELS,
		Entry_SETTINGS,
		Entry_TUNER,
		Entry_REBOOT,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	PageSwitcher & _page_switcher;
	PedalEditContext &
	               _pedal_ctx;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	State          _state;

	TxtSPtr        _edit_prog_sptr;
	TxtSPtr        _edit_bank_sptr;
	TxtSPtr        _edit_layout_sptr;
	TxtSPtr        _edit_levels_sptr;
	TxtSPtr        _settings_sptr;
	TxtSPtr        _tuner_sptr;
	TxtSPtr        _reboot_sptr;

	Question::QArg _reboot_arg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MenuMain ()                               = delete;
	               MenuMain (const MenuMain &other)          = delete;
	               MenuMain (MenuMain &&other)               = delete;
	MenuMain &     operator = (const MenuMain &other)        = delete;
	MenuMain &     operator = (MenuMain &&other)             = delete;
	bool           operator == (const MenuMain &other) const = delete;
	bool           operator != (const MenuMain &other) const = delete;

}; // class MenuMain



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/MenuMain.hpp"



#endif   // mfx_uitk_pg_MenuMain_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
