/*****************************************************************************

        NavLoc.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NavLoc_HEADER_INCLUDED)
#define mfx_uitk_NavLoc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Button.h"

#include <array>



namespace mfx
{
namespace uitk
{



class NavLoc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum OvrAction
	{
		OvrAction_DEFAULT = -1,
		OvrAction_STOP    = -2,
		OvrAction_PASS    = -3
	};

	               NavLoc ();
	               NavLoc (const NavLoc &other)     = default;
	               NavLoc (NavLoc &&other)          = default;

	               ~NavLoc ()                       = default;

	NavLoc &       operator = (const NavLoc &other) = default;
	NavLoc &       operator = (NavLoc &&other)      = default;

	bool           is_valid () const;

	int            _node_id;
	std::array <int, Button_NBR_ELT>  // Node where to jump to or OvrAction
	               _action_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const NavLoc &other) const = delete;
	bool           operator != (const NavLoc &other) const = delete;

}; // class NavLoc



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NavLoc.hpp"



#endif   // mfx_uitk_NavLoc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
