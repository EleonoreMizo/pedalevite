/*****************************************************************************

        ParamEdit.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ParamEdit_HEADER_INCLUDED)
#define mfx_uitk_pg_ParamEdit_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"



namespace mfx
{

class LocEdit;

namespace uitk
{

class PageSwitcher;

namespace pg
{



class ParamEdit
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ParamEdit (PageSwitcher &page_switcher, LocEdit &loc_edit);
	virtual        ~ParamEdit () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	virtual void   do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt);
	virtual void   do_disconnect ();

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	virtual EvtProp
	               do_handle_evt (const NodeEvt &evt);

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	virtual void   do_activate_preset (int index);
	virtual void   do_set_param (int slot_id, int index, float val, PiType type);
	virtual void   do_remove_plugin (int slot_id);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_steps = 4;

	enum Entry
	{
		Entry_NAME = 0,
		Entry_VALUNIT,
		Entry_STEP,
		Entry_CTRL = Entry_STEP + _nbr_steps,
		Entry_FOLLOW
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           update_display ();
	void           update_param_txt ();
	EvtProp        change_param (int dir);

	PageSwitcher & _page_switcher;
	LocEdit &      _loc_edit;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	TxtSPtr        _name_sptr;
	TxtSPtr        _val_unit_sptr;
	std::array <TxtSPtr, _nbr_steps>
	               _step_sptr_arr;
	TxtSPtr        _controllers_sptr;
	TxtSPtr        _follow_sptr;

	int            _step_index;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ParamEdit ()                               = delete;
	               ParamEdit (const ParamEdit &other)         = delete;
	ParamEdit &    operator = (const ParamEdit &other)        = delete;
	bool           operator == (const ParamEdit &other) const = delete;
	bool           operator != (const ParamEdit &other) const = delete;

}; // class ParamEdit



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ParamEdit.hpp"



#endif   // mfx_uitk_pg_ParamEdit_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
