/*****************************************************************************

        ProgMove.h
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
#if ! defined (mfx_uitk_pg_ProgMove_HEADER_INCLUDED)
#define mfx_uitk_pg_ProgMove_HEADER_INCLUDED

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



class ProgMove final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ProgMove (PageSwitcher &page_switcher);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_select_bank (int index) final;
	void           do_set_prog_name (std::string name) final;
	void           do_set_prog (int bank_index, int prog_index, const doc::Program &prog) final;
	void           do_store_prog (int prog_index, int bank_index) final;



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
	EvtProp        move_prog (int index_new);
	void           save_if_req ();

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains a few entries (selectable) + the program list
	TxtSPtr        _title_sptr;
	TxtArray       _prog_list;
	State          _state;        // Prog selection or move
	bool           _moving_flag;  // Set during a move operation
	bool           _moved_flag;   // The program has been moved, the document needs to be saved to disk at exit
	int            _prog_index;   // Previous program location



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProgMove ()                               = delete;
	               ProgMove (const ProgMove &other)          = delete;
	               ProgMove (ProgMove &&other)               = delete;
	ProgMove &     operator = (const ProgMove &other)        = delete;
	ProgMove &     operator = (ProgMove &&other)             = delete;
	bool           operator == (const ProgMove &other) const = delete;
	bool           operator != (const ProgMove &other) const = delete;

}; // class ProgMove



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ProgMove.hpp"



#endif   // mfx_uitk_pg_ProgMove_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
