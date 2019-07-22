/*****************************************************************************

        EditDate.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EditDate_HEADER_INCLUDED)
#define mfx_uitk_pg_EditDate_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/PageInterface.h"

#include <memory>

#include <ctime>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class EditDate
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       EditDate (PageSwitcher &page_switcher);
	virtual        ~EditDate () = default;



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

	enum Entry
	{
		Entry_T = 0,
		Entry_Y,
		Entry_M,
		Entry_D,
		Entry_H,
		Entry_MIN,
		Entry_SEC,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;

	void           update_display ();
	void           update_field (NText &label, const char format_0 [], const tm &utc);
	void           change_entry (int node_id, int dir);

	PageSwitcher & _page_switcher;
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_l_ptr;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	TxtSPtr        _title_sptr;
	TxtSPtr        _year_sptr;
	TxtSPtr        _month_sptr;
	TxtSPtr        _day_sptr;
	TxtSPtr        _hour_sptr;
	TxtSPtr        _minute_sptr;
	tm             _utc;
	bool           _change_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EditDate ()                               = delete;
	               EditDate (const EditDate &other)          = delete;
	EditDate &     operator = (const EditDate &other)        = delete;
	bool           operator == (const EditDate &other) const = delete;
	bool           operator != (const EditDate &other) const = delete;

}; // class EditDate



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EditDate.hpp"



#endif   // mfx_uitk_pg_EditDate_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
