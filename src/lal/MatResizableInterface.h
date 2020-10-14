/*****************************************************************************

        MatResizableInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_MatResizableInterface_HEADER_INCLUDED)
#define lal_MatResizableInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/Dir.h"
#include "lal/MatInterface.h"

namespace lal
{



template <typename T>
class MatResizableInterface
:	public MatInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           reserve (int r, int c);
	void           resize (int r, int c);
	void           resize (int n, Dir dir);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_reserve (int r, int c) = 0;
	virtual void   do_resize (int r, int c) = 0;
	virtual void   do_resize (int n, Dir dir) = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class MatResizableInterface



}  // namespace lal



#include "lal/MatResizableInterface.hpp"



#endif   // lal_MatResizableInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
