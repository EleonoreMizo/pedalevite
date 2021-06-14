/*****************************************************************************

        TestSpeed.hpp
        By Laurent de Soras

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (ffft_test_TestSpeed_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestSpeed code header.
#endif
#define	ffft_test_TestSpeed_CURRENT_CODEHEADER

#if ! defined (ffft_test_TestSpeed_CODEHEADER_INCLUDED)
#define	ffft_test_TestSpeed_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"ffft/test/fnc.h"
#include "ffft/test/Demangler.h"
#include	"ffft/test/TestWhiteNoiseGen.h"
#include	"ffft/test/TimerAccurate.h"

#include <algorithm>
#include	<typeinfo>

#include	<cstdio>



namespace ffft
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class FO>
int	TestSpeed <FO>::perform_test_single_object (FO &fft)
{
   int            ret_val = 0;

	const std::type_info &	ti = typeid (fft);
	Demangler      dem (ti.name ());

	if (ret_val == 0)
	{
		perform_test_d (fft, dem.get_name ());
	}
	if (ret_val == 0)
	{
		perform_test_i (fft, dem.get_name ());
	}
	if (ret_val == 0)
	{
		perform_test_di (fft, dem.get_name ());
	}

   if (ret_val == 0)
   {
      printf ("\n");
   }

   return (ret_val);
}



template <class FO>
int	TestSpeed <FO>::perform_test_d (FO &fft, const char *class_name_0)
{
   assert (class_name_0 != 0);

	const long		len = fft.get_length ();
	const int      nbr_loops = std::max (int (_lap_len / len), 1);
	const long     nbr_tests = limit (
		static_cast <long> (_nbr_spd_tests / len / nbr_loops),
		4L,
		static_cast <long> (_nbr_tests_max)
	);

	TestWhiteNoiseGen <DataType>	noise;
	std::vector <DataType>	x (len, 0);
	std::vector <DataType>	s (len);
	noise.generate (&x [0], len);

   printf (
		"%s::do_fft [%7ld samples]...                 ",
		class_name_0,
		len
	);
	fflush (stdout);


	TimerAccurate tim;
	tim.start ();
	for (long test = 0; test < nbr_tests; ++ test)
	{
		for (int k = 0; k < nbr_loops; ++k)
		{
			fft.do_fft (&s [0], &x [0]);
		}
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len * nbr_loops);
	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / double (_sample_freq);
	printf ("%12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	return (0);
}



template <class FO>
int	TestSpeed <FO>::perform_test_i (FO &fft, const char *class_name_0)
{
   assert (class_name_0 != 0);

	const long		len = fft.get_length ();
	const int      nbr_loops = std::max (int (_lap_len / len), 1);
	const long     nbr_tests = limit (
		static_cast <long> (_nbr_spd_tests / len / nbr_loops),
		4L,
		static_cast <long> (_nbr_tests_max)
	);

	TestWhiteNoiseGen <DataType>	noise;
	std::vector <DataType>	x (len);
	std::vector <DataType>	s (len, 0);
	noise.generate (&s [0], len);

   printf (
		"%s::do_ifft [%7ld samples]...                ",
		class_name_0,
		len
	);
	fflush (stdout);

	TimerAccurate tim;
	tim.start ();
	for (long test = 0; test < nbr_tests; ++ test)
	{
		for (int k = 0; k < nbr_loops; ++k)
		{
			fft.do_ifft (&s [0], &x [0]);
		}
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len * nbr_loops);
	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / double (_sample_freq);
	printf ("%12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	return (0);
}



template <class FO>
int	TestSpeed <FO>::perform_test_di (FO &fft, const char *class_name_0)
{
   assert (class_name_0 != 0);

	const long		len = fft.get_length ();
	const int      nbr_loops = std::max (int (_lap_len / len), 1);
	const long     nbr_tests = limit (
		static_cast <long> (_nbr_spd_tests / len / nbr_loops),
		4L,
		static_cast <long> (_nbr_tests_max)
	);

	TestWhiteNoiseGen <DataType>	noise;
	std::vector <DataType>	x (len, 0);
	std::vector <DataType>	s (len);
	std::vector <DataType>	y (len);
	noise.generate (&x [0], len);

   printf (
		"%s::do_fft+do_ifft+rescale [%7ld samples]... ",
		class_name_0,
		len
	);
	fflush (stdout);

	TimerAccurate tim;
	tim.start ();
	for (long test = 0; test < nbr_tests; ++ test)
	{
		for (int k = 0; k < nbr_loops; ++k)
		{
			fft.do_fft (&s [0], &x [0]);
			fft.do_ifft (&s [0], &y [0]);
			fft.rescale (&y [0]);
		}
		tim.stop_lap ();
	}

	const double   spl_per_s = tim.get_best_rate (len * nbr_loops);
	const double   mega_sps  = spl_per_s / 1e6;
	const double   rt_mul    = spl_per_s / double (_sample_freq);
	printf ("%12.3f Mspl/s (x%.3f real-time).\n", mega_sps, rt_mul);

	return (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace test
}	// namespace ffft



#endif	// ffft_test_TestSpeed_CODEHEADER_INCLUDED

#undef ffft_test_TestSpeed_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
