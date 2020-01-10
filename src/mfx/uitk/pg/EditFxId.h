/*****************************************************************************

        EditFxId.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EditFxId_HEADER_INCLUDED)
#define mfx_uitk_pg_EditFxId_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/FxId.h"
#include "mfx/uitk/pg/EditLabel.h"
#include "mfx/uitk/pg/Question.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <string>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class EditFxId
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Param
	{
	public:
		doc::FxId      _fx_id;           // Input/output
		bool           _cur_preset_flag = false; // Input: if the FX id is possibly related to the current preset
		bool           _ok_flag = false; // Output: OK or cancel
	};

	explicit       EditFxId (PageSwitcher &page_switcher);
	virtual        ~EditFxId () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_NONE = 0,
		State_MENU,
		State_FX,
		State_LABEL,

		State_NBR_ELT
	};

	enum EntryMenu
	{
		EntryMenu_FX_TYPE,
		EntryMenu_LABEL,

		EntryMenu_NBR_ELT
	};

	void           call_menu ();
	void           handle_menu ();
	void           handle_fx_type ();
	void           handle_label ();
	void           add_fx_list (const std::vector <std::string> &fx_list);

	PageSwitcher & _page_switcher;
	const Model *  _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;

	State          _state;

	Question::QArg _arg_menu;
	Question::QArg _arg_fx_type;
	EditLabel::Param
	               _arg_label;
	int            _chr_per_line; // Approximated

	Param *        _param_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EditFxId ()                               = delete;
	               EditFxId (const EditFxId &other)          = delete;
	EditFxId &     operator = (const EditFxId &other)        = delete;
	bool           operator == (const EditFxId &other) const = delete;
	bool           operator != (const EditFxId &other) const = delete;

}; // class EditFxId



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EditFxId.hpp"



#endif   // mfx_uitk_pg_EditFxId_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
