/*****************************************************************************

        Testlal.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/Mat.h"
#include "lal/op.h"
#include "test/Testlal.h"

#include <random>

#include <cassert>
#include <cstdio>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	Testlal::perform_test ()
{
	typedef double DataType;
	const int      n = 4;

	lal::Mat <DataType>  a;
	generate_rnd_mat (a, n);

	std::vector <DataType>  b;
	generate_rnd_vec (b, n);

	printf ("a =\n");
	print_mat (a);

	printf ("\nb =\n");
	print_vec (b);

#if 0
	std::vector <int> r_arr { 0, 1, 2, 3 };
	std::vector <int> c_arr { 0, 1, 2, 3 };
#elif 1
	// Different permutations should lead to the same result (with a difference
	// in the accuracy, at most)
	std::vector <int> r_arr { 2, 3, 0, 1 };
	std::vector <int> c_arr { 3, 2, 1, 0 };
#else
	std::vector <int> r_arr { 2, 0, 3, 1 };
	std::vector <int> c_arr { 1, 2, 3, 0 };
#endif
	lal::Mat <DataType>  lu;
	lal::copy (lu, a);
	lal::decompose_lu (lu, r_arr, c_arr);
	printf ("\nlu = \n");
	print_mat (lu);

	std::vector <DataType> x;
	std::vector <DataType> y;
	lal::traverse_lu (x, y, b, lu, r_arr, c_arr);
	printf ("\nx =\n");
	print_vec (x);

	lal::Mat <DataType>  at;
	lal::transpose (at, a);
	printf ("\ntranspose (a) =\n");
	print_mat (at);

	lal::Mat <DataType>  ai;
	lal::invert (ai, y, lu, r_arr, c_arr);
	printf ("\ninvert (a) =\n");
	print_mat (ai);

	lal::Mat <DataType>  ai_a;
	lal::mul (ai_a, ai, a);
	printf ("\ninvert (a) * a =\n");
	print_mat (ai_a);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	Testlal::generate_rnd_mat (lal::MatResizableInterface <T> &a, int n)
{
	assert (n >= 0);

	std::default_random_engine gen;
	std::uniform_int_distribution <int> dist (1, 99);

	a.resize (n, n);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			a (i, j) = T (dist (gen));
		}
	}
}



template <typename T>
void	Testlal::generate_rnd_vec (std::vector <T> &v, int n)
{
	assert (n >= 0);

	std::default_random_engine gen;
	std::uniform_int_distribution <int> dist (1, 99);

	v.resize (n);
	for (int i = 0; i < n; ++i)
	{
		v [i] = T (dist (gen));
	}
}



template <typename T>
void	Testlal::print_mat (const lal::MatInterface <T> &a)
{
	const int      rows = a.get_rows ();
	const int      cols = a.get_cols ();
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			printf ("%9.3lf%s", double (a (i, j)), (j == cols - 1) ? "\n": " ");
		}
	}
}



template <typename T>
void	Testlal::print_vec (const std::vector <T> &v)
{
	const int      n = int (v.size ());
	for (int i = 0; i < n; ++i)
	{
		printf ("%9.3lf%s", double (v [i]), (i == n - 1) ? "\n": " ");
	}
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
