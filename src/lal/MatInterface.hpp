/*****************************************************************************

        MatInterface.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_MatInterface_CODEHEADER_INCLUDED)
#define lal_MatInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace lal
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
const T &	MatInterface <T>::operator () (int r, int c) const
{
	return Inherited::operator () (r, c);
}



template <typename T>
T &	MatInterface <T>::operator () (int r, int c)
{
	assert (r >= 0);
	assert (c >= 0);

	return do_at (r, c);
}



template <typename T>
const T *	MatInterface <T>::get_data () const
{
	return Inherited::get_data ();
}



template <typename T>
T *	MatInterface <T>::get_data ()
{
	T *            ptr = do_get_data ();
	assert (ptr != nullptr);

	return ptr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lal



#endif   // lal_MatInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
