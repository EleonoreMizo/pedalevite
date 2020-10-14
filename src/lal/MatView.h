/*****************************************************************************

        MatView.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_MatView_HEADER_INCLUDED)
#define lal_MatView_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/MatInterface.h"



namespace lal
{



template <typename T>
class MatView
:	public MatInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           setup (int rows, int cols, bool const_flag, T *data_ptr, int stride);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// lal::MatInterface
	int            do_get_rows () const override;
	int            do_get_cols () const override;
	T &            do_at (int r, int c) override;
	const T &      do_at (int r, int c) const override;

	T *            do_get_data () override;
	const T *      do_get_data () const override;
	int            do_get_stride () const override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int           conv_coord_to_pos (int r, int c) const;

	int            _rows       = 0;
	int            _cols       = 0;
	int            _stride     = 0;
	T *            _data_ptr   = &this->_dummy_scalar;
	bool           _const_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MatView <T> &other) const = delete;
	bool           operator != (const MatView <T> &other) const = delete;

}; // class MatView



}  // namespace lal



#include "lal/MatView.hpp"



#endif   // lal_MatView_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
