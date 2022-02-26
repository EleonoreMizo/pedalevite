/*****************************************************************************

        TestR128.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestR128_HEADER_INCLUDED)
#define TestR128_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



class TestR128
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class SpecSineSeg
	{
	public:
		// Sine level for each channel, linear: 1 = 0 dBFS
		std::vector <double>
		               _lvl_arr;

		// Duration, s
		double         _duration  = 20.f;

		// Frequency in Hz
		double         _freq      = 1000.0f;
	};

	class SpecGen
	{
	public:
		// Multiple concatenated segments
		std::vector <SpecSineSeg>
		               _seg_arr;

		// 10 ms fade at the beginning and the end of the whole signal
		bool           _fade_flag = false;

		// Initial phase in turns, [0 ; 1].
		double         _phase     = 0;

		// Phase shift in samples, [0 ; 1[.
		double         _shift     = 0;
	};

	enum class Win
	{
		M = 0, // Momentary
		S,     // Short-term
		I,     // Integrated
		LRA,   // Loudness range
		TP     // True peak
	};

	class Result
	{
	public:
		// Type of checked result
		Win            _win      = Win::I;

		// Value in LU (FS or range)
		double         _val      = -23;

		// Tolerance, LU, below the value, >= 0
		double         _tol_n    = 0.1;

		// Tolerance, LU, above the value, >= 0
		double         _tol_p    = _tol_n;

		// Indicates we have to check the maximum value across the specified
		// time range, instead of the value at every moment on this range.
		// Only for Win::M and Win::S.
		bool           _max_flag = false;
	};

	class SpecResult
	{
	public:
		// Duration of the test (including _delay), in s. 0 = whole test
		double         _duration = 0;

		// Delay in seconds before the actual check
		double         _delay    = 0;

		// List of the expected results for this time range
		std::vector <Result>
		               _res_arr;
	};

	class SpecAllResults
	{
	public:
		std::vector <SpecResult>
		               _spec_arr;
	};

	typedef std::vector <float> Buffer;
	typedef std::vector <Buffer> BufMulti;

	static double  dB (double db) noexcept;
	static double  deg (double deg) noexcept;
	static void    perform_single_test (int &ret_val, std::string name, double sample_freq, const SpecGen &gen, const SpecAllResults &res_all);
	static void    generate_signal (BufMulti &buf_arr, double sample_freq, const SpecGen &gen);
	static int     check_signal (const BufMulti &buf_arr, double sample_freq, const SpecAllResults &res_all);
	static int     check_value (const Result &res, float val_cur, float val_max, bool end_flag);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestR128 ()                               = delete;
	               TestR128 (const TestR128 &other)          = delete;
	               TestR128 (TestR128 &&other)               = delete;
	               ~TestR128 ()                              = delete;
	TestR128 &     operator = (const TestR128 &other)        = delete;
	TestR128 &     operator = (TestR128 &&other)             = delete;
	bool           operator == (const TestR128 &other) const = delete;
	bool           operator != (const TestR128 &other) const = delete;

}; // class TestR128



//#include "test/TestR128.hpp"



#endif   // TestR128_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
