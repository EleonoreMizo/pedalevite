/*****************************************************************************

        SettingsOther.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_SettingsOther_HEADER_INCLUDED)
#define mfx_uitk_pg_SettingsOther_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/Cst.h"
#include "mfx/Dir.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class SettingsOther final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SettingsOther (PageSwitcher &page_switcher);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_tempo (double bpm) final;
	void           do_set_click (bool click_flag) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_TEMPO_I = 1000,
		Entry_TEMPO_F,
		Entry_CLICK,
		Entry_SAVE,
		Entry_BACKUP,
		Entry_RECORD
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           refresh_display ();
	EvtProp        change_tempo (double delta);

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;

	TxtSPtr        _tempo_i_sptr;
	TxtSPtr        _tempo_f_sptr;
	TxtSPtr        _click_sptr;
	TxtSPtr        _save_sptr;
	TxtSPtr        _backup_sptr;
	TxtSPtr        _record_sptr;

	Question::QArg _msg_arg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SettingsOther ()                               = delete;
	               SettingsOther (const SettingsOther &other)     = delete;
	               SettingsOther (SettingsOther &&other)          = delete;
	SettingsOther& operator = (const SettingsOther &other)        = delete;
	SettingsOther& operator = (SettingsOther &&other)             = delete;
	bool           operator == (const SettingsOther &other) const = delete;
	bool           operator != (const SettingsOther &other) const = delete;

}; // class SettingsOther



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/SettingsOther.hpp"



#endif   // mfx_uitk_pg_SettingsOther_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
