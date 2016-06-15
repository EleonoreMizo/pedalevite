/*****************************************************************************

        PageSwitcher.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Page.h"
#include "mfx/uitk/PageSwitcher.h"

#include <cassert>



namespace mfx
{
namespace uitk
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PageSwitcher::PageSwitcher (Page &page_mgr)
:	_page_mgr (page_mgr)
,	_page_map ()
,	_call_stack ()
,	_cur_page (pg::PageType_INVALID)
,	_usr_ptr (0)
,	_prev_node_id (-1)
{
	// Nothing
}



void	PageSwitcher::add_page (pg::PageType page_id, PageInterface &page)
{
	assert (page_id >= 0);
	assert (_page_map.find (page_id) == _page_map.end ());

	_page_map [page_id] = &page;
}



void	PageSwitcher::switch_to (pg::PageType page_id, void *usr_ptr)
{
	switch_to (page_id, usr_ptr, -1);
}



// node_id = cursor location in the initial page
void	PageSwitcher::call_page (pg::PageType page_id, void *usr_ptr, int node_id)
{
	assert (_cur_page >= 0);

	_call_stack.push_back (PagePos { _cur_page, _usr_ptr, node_id });
	switch_to (page_id, usr_ptr);
}



// Returns the original node
void	PageSwitcher::return_page ()
{
	assert (! _call_stack.empty ());

	const PagePos        page_pos = _call_stack.back ();
	_call_stack.pop_back ();
	switch_to (page_pos._page_id, page_pos._usr_ptr, page_pos._node_id);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PageSwitcher::switch_to (pg::PageType page_id, void *usr_ptr, int node_id)
{
	assert (page_id >= 0);
	assert (_page_map.find (page_id) != _page_map.end ());

	PageInterface *   page_ptr = _page_map [page_id];
	_cur_page = page_id;
	_usr_ptr  = usr_ptr;
	_page_mgr.set_page_content (*page_ptr, usr_ptr);
	if (node_id >= 0)
	{
		_page_mgr.jump_to (node_id);
	}
}



}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
