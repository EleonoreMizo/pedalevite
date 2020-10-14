/*****************************************************************************

        MatInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_MatInterface_HEADER_INCLUDED)
#define lal_MatInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace lal
{



template <typename T>
class MatInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T Scalar;

	               MatInterface ()  = default;
	virtual        ~MatInterface () = default;

	int            get_rows () const;
	int            get_cols () const;
	T &            operator () (int r, int c);
	const T &      operator () (int r, int c) const;

	T *            get_data ();
	const T *      get_data () const;
	int            get_stride () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_rows () const = 0;
	virtual int    do_get_cols () const = 0;
	virtual T &    do_at (int r, int c) = 0;
	virtual const T &
	               do_at (int r, int c) const = 0;

	virtual T *    do_get_data () = 0;
	virtual const T *
	               do_get_data () const = 0;
	virtual int    do_get_stride () const = 0;

	static T       _dummy_scalar;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class MatInterface



}  // namespace lal



#include "lal/MatInterface.hpp"



#endif   // lal_MatInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
