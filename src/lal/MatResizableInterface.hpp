/*****************************************************************************

        MatResizableInterface.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_MatResizableInterface_CODEHEADER_INCLUDED)
#define lal_MatResizableInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace lal
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	MatResizableInterface <T>::reserve (int r, int c)
{
	assert (r >= 0);
	assert (c >= 0);

	do_reserve (r, c);
}



template <class T>
void	MatResizableInterface <T>::resize (int r, int c)
{
	assert (r >= 0);
	assert (c >= 0);

	do_resize (r, c);
}



// Makes a vector
template <class T>
void	MatResizableInterface <T>::resize (int n, Dir dir)
{
	assert (n >= 0);

	do_resize (n, dir);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lal



#endif   // lal_MatResizableInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
