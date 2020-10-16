/*****************************************************************************

        MatViewConst.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_MatViewConst_HEADER_INCLUDED)
#define lal_MatViewConst_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/MatConstInterface.h"



namespace lal
{



template <typename T>
class MatViewConst
:	public MatConstInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           setup (int rows, int cols, T *data_ptr, int stride);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// lal::MatConstInterface
	int            do_get_rows () const override;
	int            do_get_cols () const override;
	const T &      do_at (int r, int c) const override;
	const T *      do_get_data () const override;
	int            do_get_stride () const override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int           conv_coord_to_pos (int r, int c) const;

	int            _rows       = 0;
	int            _cols       = 0;
	int            _stride     = 0;
	T *            _data_ptr   = &this->_dummy_scalar;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MatViewConst ()                               = delete;
	               MatViewConst (const MatViewConst &other)      = delete;
	               MatViewConst (MatViewConst &&other)           = delete;
	MatViewConst & operator = (const MatViewConst &other)        = delete;
	MatViewConst & operator = (MatViewConst &&other)             = delete;
	bool           operator == (const MatViewConst &other) const = delete;
	bool           operator != (const MatViewConst &other) const = delete;

}; // class MatViewConst



}  // namespace lal



#include "lal/MatViewConst.hpp"



#endif   // lal_MatViewConst_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
