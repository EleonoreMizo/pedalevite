/*****************************************************************************

        BankMenu.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_BankMenu_HEADER_INCLUDED)
#define mfx_uitk_pg_BankMenu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalEditContext.h"
#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/uitk/PageMgrInterface.h"



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class BankMenu final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BankMenu (PageSwitcher &page_switcher, PedalEditContext &pedal_ctx);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;

	// mfx::ModelObserverInterface via mfx::uitk::PageInterface
	void           do_set_bank (int index, const doc::Bank &bank) final;
	void           do_select_bank (int index) final;
	void           do_set_bank_name (std::string name) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_WINDOW = 1000,
		Entry_ORGANIZE,
		Entry_IMPORT,
		Entry_EXPORT,
		Entry_LAYOUT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	void           update_display ();

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

	WinSPtr        _menu_sptr;    // Contains a few entries (selectable) + the bank list
	TxtSPtr        _orga_sptr;
	TxtSPtr        _import_sptr;
	TxtSPtr        _export_sptr;
	TxtSPtr        _layout_sptr;
	TxtArray       _bank_list;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BankMenu ()                               = delete;
	               BankMenu (const BankMenu &other)          = delete;
	               BankMenu (BankMenu &&other)               = delete;
	BankMenu &     operator = (const BankMenu &other)        = delete;
	BankMenu &     operator = (BankMenu &&other)             = delete;
	bool           operator == (const BankMenu &other) const = delete;
	bool           operator != (const BankMenu &other) const = delete;

}; // class BankMenu



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/BankMenu.hpp"



#endif   // mfx_uitk_pg_BankMenu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
