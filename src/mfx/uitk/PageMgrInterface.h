/*****************************************************************************

        PageMgrInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_PageMgrInterface_HEADER_INCLUDED)
#define mfx_uitk_PageMgrInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/ContainerInterface.h"
#include "mfx/uitk/NavLoc.h"

#include <vector>



namespace mfx
{
namespace uitk
{



class PageMgrInterface
:	public ContainerInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::vector <NavLoc> NavLocList;

	virtual        ~PageMgrInterface () = default;

	void           set_nav_layout (const NavLocList &nav_list);
	void           jump_to (int node_id);
	void           set_timer (int node_id, bool enable_flag);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_set_nav_layout (const NavLocList &nav_list) = 0;
	virtual void   do_jump_to (int node_id) = 0;
	virtual void   do_set_timer (int node_id, bool enable_flag) = 0;



}; // class PageMgrInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/PageMgrInterface.hpp"



#endif   // mfx_uitk_PageMgrInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
