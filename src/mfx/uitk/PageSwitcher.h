/*****************************************************************************

        PageSwitcher.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_PageSwitcher_HEADER_INCLUDED)
#define mfx_uitk_PageSwitcher_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PageType.h"

#include <map>
#include <vector>



namespace mfx
{
namespace uitk
{



class Page;
class PageInterface;

class PageSwitcher
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PageSwitcher (Page &page_mgr);
	virtual        ~PageSwitcher () = default;

	void           add_page (pg::PageType page_id, PageInterface &page);

	void           switch_to (pg::PageType page_id);
	void           call_page (pg::PageType page_id);
	void           return_page ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::map <pg::PageType, PageInterface *> PageMap;
	typedef std::vector <pg::PageType> PageStack;

	Page &         _page_mgr;
	PageMap        _page_map;
	PageStack      _call_stack;
	pg::PageType   _cur_page;     // pg::PageType_INVALID: not set



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PageSwitcher ()                               = delete;
	               PageSwitcher (const PageSwitcher &other)      = delete;
	PageSwitcher & operator = (const PageSwitcher &other)        = delete;
	bool           operator == (const PageSwitcher &other) const = delete;
	bool           operator != (const PageSwitcher &other) const = delete;

}; // class PageSwitcher



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/PageSwitcher.hpp"



#endif   // mfx_uitk_PageSwitcher_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
