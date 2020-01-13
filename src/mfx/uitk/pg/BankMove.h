/*****************************************************************************

        BankMove.h
        Author: Laurent de Soras, 2018

Use call_page() to access this page

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_BankMove_HEADER_INCLUDED)
#define mfx_uitk_pg_BankMove_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"



namespace mfx
{
namespace uitk
{

class PageMgrInterface;
class PageSwitcher;

namespace pg
{



class BankMove final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BankMove (PageSwitcher &page_switcher);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_bank (int index, const doc::Bank &bank) final;
	void           do_set_bank_name (std::string name) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum State
	{
		State_SEL = 0,
		State_MOV,

		State_NBR_ELT
	};

	enum Entry
	{
		Entry_WINDOW = 1000,
		Entry_TITLE
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           update_display ();
	EvtProp        move_bank (int index_new);
	void           save_if_req ();

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains a few entries (selectable) + the bank list
	TxtSPtr        _title_sptr;
	TxtArray       _bank_list;
	State          _state;        // Bank selection or move
	bool           _moving_flag;  // Set during a move operation
	bool           _moved_flag;   // The bank has been moved, the document needs to be saved to disk at exit
	int            _bank_index;   // Previous bank location



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BankMove ()                               = delete;
	               BankMove (const BankMove &other)          = delete;
	               BankMove (BankMove &&other)               = delete;
	BankMove &     operator = (const BankMove &other)        = delete;
	BankMove &     operator = (BankMove &&other)             = delete;
	bool           operator == (const BankMove &other) const = delete;
	bool           operator != (const BankMove &other) const = delete;

}; // class BankMove



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/BankMove.hpp"



#endif   // mfx_uitk_pg_BankMove_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
