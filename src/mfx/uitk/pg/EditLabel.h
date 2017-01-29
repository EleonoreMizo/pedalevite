/*****************************************************************************

        EditLabel.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_EditLabel_HEADER_INCLUDED)
#define mfx_uitk_pg_EditLabel_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/EditText.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"

#include <memory>
#include <set>
#include <string>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class EditLabel
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Param
	{
	public:
		std::string    _label;                 // Input/output
		bool           _sep_cur_flag = false;  // Input: labels from the current preset are separated
		bool           _ok_flag      = false;  // Output: OK or cancel
	};

	explicit       EditLabel (PageSwitcher &page_switcher);
	virtual        ~EditLabel () = default;



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
		Entry_WINDOW = 0,
		Entry_NAME,
		Entry_EDIT,
		Entry_LIST,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> LabelList;
	typedef std::shared_ptr <NWindow> WinSPtr;

	void           add_label_list (PageMgrInterface::NavLocList &nav_list, const std::set <std::string> &labels, int node_id_base, int y);

	PageSwitcher & _page_switcher;
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;
	TxtSPtr        _name_sptr;
	TxtSPtr        _edit_sptr;
	LabelList      _label_sptr_arr;

	Param *        _arg_ptr;
	std::vector <std::string>
	               _label_arr;
	EditText::Param
	               _edit_text_arg;
	bool           _edit_text_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EditLabel ()                               = delete;
	               EditLabel (const EditLabel &other)         = delete;
	EditLabel &    operator = (const EditLabel &other)        = delete;
	bool           operator == (const EditLabel &other) const = delete;
	bool           operator != (const EditLabel &other) const = delete;

}; // class EditLabel



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/EditLabel.hpp"



#endif   // mfx_uitk_pg_EditLabel_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
