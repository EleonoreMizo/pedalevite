/*****************************************************************************

        MatViewConst.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_MatViewConst_CODEHEADER_INCLUDED)
#define lal_MatViewConst_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace lal
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	MatViewConst <T>::setup (int rows, int cols, T *data_ptr, int stride)
{
	assert (rows >= 0);
	assert (cols >= 0);
	assert (rows == 0 || cols == 0 || data_ptr != nullptr);

	_rows       = rows;
	_cols       = cols;
	_stride     = (rows == 0 || cols == 0) ? 0 : stride;
	_data_ptr   = (rows == 0 || cols == 0) ? &MatViewConst <T>::_dummy_scalar : data_ptr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
int	MatViewConst <T>::do_get_rows () const
{
	return _rows;
}



template <typename T>
int	MatViewConst <T>::do_get_cols () const
{
	return _cols;
}



template <typename T>
const T &	MatViewConst <T>::do_at (int r, int c) const
{
	assert (r < _rows);
	assert (c < _cols);

	const int      pos = conv_coord_to_pos (r, c);

	return _data_ptr [pos];
}



template <typename T>
const T *	MatViewConst <T>::do_get_data () const
{
	return _data_ptr;
}



template <typename T>
int	MatViewConst <T>::do_get_stride () const
{
	return _stride;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
int	MatViewConst <T>::conv_coord_to_pos (int r, int c) const
{
	assert (r >= 0);
	assert (r < _rows);
	assert (c >= 0);
	assert (c < _cols);

	return r * _stride + c;
}



}  // namespace lal



#endif   // lal_MatViewConst_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
