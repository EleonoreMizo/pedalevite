/*****************************************************************************

        ProgCatalog.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_ProgCatalog_HEADER_INCLUDED)
#define mfx_uitk_pg_ProgCatalog_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NText.h"
#include "mfx/uitk/NWindow.h"
#include "mfx/uitk/PageInterface.h"
#include "mfx/UniqueProgList.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace uitk
{

class PageSwitcher;

namespace pg
{



class ProgCatalog final
:	public PageInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ProgCatalog (PageSwitcher &page_switcher);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::uitk::PageInterface
	void           do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const FontSet &fnt) final;
	void           do_disconnect () final;

	// mfx::uitk::MsgHandlerInterface via mfx::uitk::PageInterface
	EvtProp        do_handle_evt (const NodeEvt &evt) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Entry
	{
		Entry_BASE   = 0,
		Entry_WINDOW = 1'000'000,

		Entry_NBR_ELT
	};

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::shared_ptr <NWindow> WinSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	bool           is_valid_prog (int node_id) const;
	int            conv_node_id_to_index (int node_id) const;
	int            conv_index_to_node_id (int index) const;

	PageSwitcher & _page_switcher;
	Model *        _model_ptr;    // 0 = not connected
	const View *   _view_ptr;     // 0 = not connected
	PageMgrInterface *            // 0 = not connected
	               _page_ptr;
	Vec2d          _page_size;
	const ui::Font *              // 0 = not connected
	               _fnt_ptr;

	WinSPtr        _menu_sptr;    // Contains only the program list at the moment
	TxtArray       _prog_list;
	UniqueProgList::ProgList
	               _prog_coord_list;
	int            _active_node_id;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProgCatalog ()                               = delete;
	               ProgCatalog (const ProgCatalog &other)       = delete;
	               ProgCatalog (ProgCatalog &&other)            = delete;
	ProgCatalog &  operator = (const ProgCatalog &other)        = delete;
	ProgCatalog &  operator = (ProgCatalog &&other)             = delete;
	bool           operator == (const ProgCatalog &other) const = delete;
	bool           operator != (const ProgCatalog &other) const = delete;

}; // class ProgCatalog



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/ProgCatalog.hpp"



#endif   // mfx_uitk_pg_ProgCatalog_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
