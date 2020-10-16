/*****************************************************************************

        MatConstInterface.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_MatConstInterface_CODEHEADER_INCLUDED)
#define lal_MatConstInterface_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace lal
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
int	MatConstInterface <T>::get_rows () const
{
	const int      rows = do_get_rows ();
	assert (rows >= 0);

	return rows;
}



template <typename T>
int	MatConstInterface <T>::get_cols () const
{
	const int      cols = do_get_cols ();
	assert (cols >= 0);

	return cols;
}



template <typename T>
const T &	MatConstInterface <T>::operator () (int r, int c) const
{
	assert (r >= 0);
	assert (c >= 0);

	return do_at (r, c);
}



template <typename T>
const T *	MatConstInterface <T>::get_data () const
{
	const T *      ptr = do_get_data ();
	assert (do_get_rows () == 0 || do_get_cols () == 0 || ptr != nullptr);

	return ptr;
}



template <typename T>
int	MatConstInterface <T>::get_stride () const
{
	return do_get_stride ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
T	MatConstInterface <T>::_dummy_scalar { 0 };



}  // namespace lal



#endif   // lal_MatConstInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
