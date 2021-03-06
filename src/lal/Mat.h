/*****************************************************************************

        Mat.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_Mat_HEADER_INCLUDED)
#define lal_Mat_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/MatResizableInterface.h"

#include <vector>



namespace lal
{



template <typename T> class MatView;
template <typename T> class MatViewConst;

template <typename T>
class Mat
:	public MatResizableInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	MatView <T>    make_sub (int r, int c, int h, int w) noexcept;
	MatViewConst <T>
	               make_sub (int r, int c, int h, int w) const noexcept;

	void           set_zero () noexcept;
	void           set_id () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// lal::MatConstInterface
	int            do_get_rows () const noexcept override;
	int            do_get_cols () const noexcept override;
	const T &      do_at (int r, int c) const noexcept override;
	const T *      do_get_data () const noexcept override;
	int            do_get_stride () const noexcept override;

	// lal::MatInterface
	T &            do_at (int r, int c) noexcept override;
	T *            do_get_data () noexcept override;

	// lal::MatResizableInterface
	void           do_reserve (int r, int c) override;
	void           do_resize (int r, int c) override;
	void           do_resize (int n, Dir dir) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            conv_coord_to_pos (int r, int c) const noexcept;

	int            _rows = 0;  // >= 0
	int            _cols = 0;  // >= 0
	std::vector <T>
	               _data;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Mat &other) const = delete;
	bool           operator != (const Mat &other) const = delete;

}; // class Mat



}  // namespace lal



#include "lal/Mat.hpp"



#endif   // lal_Mat_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
