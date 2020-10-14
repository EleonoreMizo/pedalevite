/*****************************************************************************

        Testlal.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (Testlal_HEADER_INCLUDED)
#define Testlal_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace lal
{
	template <typename T> class MatInterface;
	template <typename T> class MatResizableInterface;
};

class Testlal
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <typename T>
	static void    generate_rnd_mat (lal::MatResizableInterface <T> &a, int n);
	template <typename T>
	static void    generate_rnd_vec (std::vector <T> &v, int n);

	template <typename T>
	static void    print_mat (const lal::MatInterface <T> &a);
	template <typename T>
	static void    print_vec (const std::vector <T> &v);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Testlal ()                               = delete;
	               Testlal (const Testlal &other)           = delete;
	               Testlal (Testlal &&other)                = delete;
	Testlal &      operator = (const Testlal &other)        = delete;
	Testlal &      operator = (Testlal &&other)             = delete;
	bool           operator == (const Testlal &other) const = delete;
	bool           operator != (const Testlal &other) const = delete;

}; // class Testlal



//#include "test/Testlal.hpp"



#endif   // Testlal_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
