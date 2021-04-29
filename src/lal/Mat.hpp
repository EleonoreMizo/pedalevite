/*****************************************************************************

        Mat.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_Mat_CODEHEADER_INCLUDED)
#define lal_Mat_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/MatView.h"

#include <algorithm>

#include <cassert>



namespace lal
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
MatView <T>	Mat <T>::make_sub (int r, int c, int h, int w) noexcept
{
	assert (r >= 0);
	assert (c >= 0);
	assert (h >= 0);
	assert (w >= 0);
	assert (r + h <= _rows);
	assert (c + w <= _cols);

	MatView <T>    view;
	T *            data_ptr = this->get_data ();
	const int      stride   = this->get_stride ();
	if (_rows > 0 && _cols > 0)
	{
		assert (data_ptr != nullptr);
		data_ptr += stride * r + c;
	}
	view.setup (h, w, data_ptr, stride);

	return view;
}



template <typename T>
MatViewConst <T>	Mat <T>::make_sub (int r, int c, int h, int w) const noexcept
{
	assert (r >= 0);
	assert (c >= 0);
	assert (h >= 0);
	assert (w >= 0);
	assert (r + h <= _rows);
	assert (c + w <= _cols);

	MatViewConst <T>  view;
	T *            data_ptr = this->get_data ();
	const int      stride   = this->get_stride ();
	if (_rows > 0 && _cols > 0)
	{
		assert (data_ptr != nullptr);
		data_ptr += stride * r + c;
	}
	view.setup (h, w, data_ptr, stride);

	return view;
}



template <typename T>
void	Mat <T>::set_zero () noexcept
{
	std::fill (_data.begin (), _data.end (), T (0));
}



template <typename T>
void	Mat <T>::set_id () noexcept
{
	set_zero ();
	const int      len      = std::min (_rows, _cols);
	T *            data_ptr = this->get_data ();
	const int      stride   = this->get_stride ();
	const int      step     = stride + 1;
	const int      len_s    = len * step;
	for (int pos = 0; pos < len_s; pos += step)
	{
		data_ptr [pos] = T (1);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
int	Mat <T>::do_get_rows () const noexcept
{
	return _rows;
}



template <typename T>
int	Mat <T>::do_get_cols () const noexcept
{
	return _cols;
}



template <typename T>
const T &	Mat <T>::do_at (int r, int c) const noexcept
{
	assert (r < _rows);
	assert (c < _cols);

	const int      pos = conv_coord_to_pos (r, c);

	return _data [pos];
}



template <typename T>
const T *	Mat <T>::do_get_data () const noexcept
{
	return _data.data ();
}



template <typename T>
int	Mat <T>::do_get_stride () const noexcept
{
	return _cols;
}



template <typename T>
T &	Mat <T>::do_at (int r, int c) noexcept
{
	assert (r < _rows);
	assert (c < _cols);

	const int      pos = conv_coord_to_pos (r, c);

	return _data [pos];
}



template <typename T>
T *	Mat <T>::do_get_data () noexcept
{
	return _data.data ();
}



template <typename T>
void	Mat <T>::do_reserve (int r, int c)
{
	_data.reserve (r * c);
}



// Data is lost, it is assumed that the structure contains garbage
template <typename T>
void	Mat <T>::do_resize (int r, int c)
{
	_data.resize (r * c);
	_rows = r;
	_cols = c;
}



// Data is lost, it is assumed that the structure contains garbage
// Diagonal construct fills the structure with 0s.
template <typename T>
void	Mat <T>::do_resize (int n, Dir dir)
{
	int            r = 0;
	int            c = 0;
	switch (dir)
	{
	case Dir::V: r = n; c = 1; break;
	case Dir::H: r = 1; c = n; break;
	case Dir::D: r = n; c = n; break;
	}
	if (r != _rows || c != _cols)
	{
		_data.resize (r * c);
		_rows = r;
		_cols = c;
		if (dir == Dir::D)
		{
			std::fill (_data.begin (), _data.end (), T (0));
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
int	Mat <T>::conv_coord_to_pos (int r, int c) const noexcept
{
	assert (r >= 0);
	assert (c >= 0);
	assert (r < _rows);
	assert (c < _cols);

	return r * _cols + c;
}



}  // namespace lal



#endif   // lal_Mat_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
