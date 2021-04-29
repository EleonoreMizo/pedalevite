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

#include "lal/MatConstInterface.h"



namespace lal
{



template <typename T>
class MatInterface
:	public MatConstInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MatInterface ()  = default;
	virtual        ~MatInterface () = default;

	const T &      operator () (int r, int c) const noexcept;
	T &            operator () (int r, int c) noexcept;
	const T *      get_data () const noexcept;
	T *            get_data () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	const virtual T &
	               do_at (int r, int c) const noexcept = 0;
	virtual T &    do_at (int r, int c) noexcept = 0;
	const virtual T *
	               do_get_data () const noexcept = 0;
	virtual T *    do_get_data () noexcept = 0;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

	typedef MatConstInterface <T> Inherited;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class MatInterface



}  // namespace lal



#include "lal/MatInterface.hpp"



#endif   // lal_MatInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
