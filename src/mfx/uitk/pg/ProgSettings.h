/*****************************************************************************

        ProgSettings.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ProgSettings_HEADER_INCLUDED)
#define mfx_uitk_pg_ProgSettings_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"



namespace mfx
{

namespace doc
{
	class Routing;
}

namespace uitk
{

class PageSwitcher;

namespace pg
{



class ProgSettings final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ProgSettings (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_prog_switch_mode (doc::ProgSwitchMode mode) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NONE = 0,
		State_FXSTATE,
		State_ADDSLOTS,

		State_NBR_ELT
	};

	enum Entry
	{
		Entry_LAYOUT = 1000,
		Entry_SWITCH,
		Entry_ADD_SLOTS
	};

	enum FxState
	{
		FxState_KEEP = 0,
		FxState_FRESH,
		FxState_CANCEL,

		FxState_NBR_ELT
	};

	enum AddSlot
	{
		AddSlot_CANCEL = 0,
		AddSlot_1
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           update_display ();
	EvtProp        change_switch (int dir);
	void           change_all_plugin_state_modes ();
	void           change_all_plugin_state_modes (bool force_reset_flag);
	void           add_slots ();
	void           remove_direct_io_cnx (doc::Routing &routing);

	PageSwitcher & _page_switcher;
	PedalEditContext &
	               _pedal_ctx;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	TxtSPtr        _layout_sptr;
	TxtSPtr        _switch_sptr;
	TxtSPtr        _add_slots_sptr;

	State          _state;
	Question::QArg _arg_fx_state_modes;
	Question::QArg _arg_add_slots;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProgSettings ()                               = delete;
	               ProgSettings (const ProgSettings &other)      = delete;
	               ProgSettings (ProgSettings &&other)           = delete;
	ProgSettings & operator = (const ProgSettings &other)        = delete;
	ProgSettings & operator = (ProgSettings &&other)             = delete;
	bool           operator == (const ProgSettings &other) const = delete;
	bool           operator != (const ProgSettings &other) const = delete;

}; // class ProgSettings



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ProgSettings.hpp"



#endif   // mfx_uitk_pg_ProgSettings_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
