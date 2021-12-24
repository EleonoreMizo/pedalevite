/*****************************************************************************

        TestDelay.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_TestDelay_CODEHEADER_INCLUDED)
#define hiir_test_TestDelay_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/ResultCheck.h"
#include "hiir/test/SweepingSine.h"
#include "hiir/PolyphaseIir2Designer.h"

#include <vector>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
int	TestDelay <TO>::test_halfband (TO &filter, const double coef_arr [_nbr_coefs])
{
	const double   dly_ref =
		filter._delay + PolyphaseIir2Designer::compute_phase_delay (
			coef_arr, _nbr_coefs, _f_fs, false
		);

	std::vector <DataType>  src (_len * _nbr_chn);
	std::vector <DataType>  dst (_len * _nbr_chn);
	std::vector <DataType>  src_m (_len);
	std::vector <DataType>  dst_m (_len);
	SweepingSine   ss (1, _f_fs, _f_fs, _len);
	ss.generate (src_m.data ());
	for (long k = 0; k < _len; ++k)
	{
		for (int c = 0; c < _nbr_chn; ++c)
		{
			src [k * _nbr_chn + c] = src_m [k];
		}
	}
	filter.clear_buffers ();
	filter.process_block (dst.data (), src.data (), _len);
	for (long k = 0; k < _len; ++k)
	{
		dst_m [k] = dst [k * _nbr_chn];
	}

	int            ret_val = ResultCheck <DataType>::check_delay (
		dst_m.data (), src_m.data (), _len, dly_ref, _f_fs
	);

	return ret_val;
}



template <class TO>
int	TestDelay <TO>::test_downsampler (TO &filter, const double coef_arr [_nbr_coefs])
{
	constexpr long    len_src = _len * 2;

	const double   dly_ref =
		filter._delay + PolyphaseIir2Designer::compute_phase_delay (
			coef_arr, _nbr_coefs, _f_fs, false
		);

	std::vector <DataType>  src (len_src * _nbr_chn);
	std::vector <DataType>  dst (_len * _nbr_chn);
	std::vector <DataType>  src_m2 (len_src);
	std::vector <DataType>  dst_m (_len);
	std::vector <DataType>  src_m (_len);
	SweepingSine   ss (1, _f_fs, _f_fs, len_src);
	ss.generate (src_m2.data ());
	for (long k = 0; k < len_src; ++k)
	{
		for (int c = 0; c < _nbr_chn; ++c)
		{
			src [k * _nbr_chn + c] = src_m2 [k];
		}
	}
	filter.clear_buffers ();
	filter.process_block (dst.data (), src.data (), _len);
	for (long k = 0; k < _len; ++k)
	{
		dst_m [k] = dst [k * _nbr_chn];
		src_m [k] = src_m2 [k * 2]; // Simple decimation
	}

	int            ret_val = ResultCheck <DataType>::check_delay (
		dst_m.data (), src_m.data (), _len, dly_ref * 0.5, _f_fs * 2
	);

	return ret_val;
}



template <class TO>
int	TestDelay <TO>::test_upsampler (TO &filter, const double coef_arr [_nbr_coefs])
{
	constexpr long    len_src = _len / 2;

	const double   dly_ref =
		filter._delay + PolyphaseIir2Designer::compute_phase_delay (
			coef_arr, _nbr_coefs, _f_fs, false
		);

	std::vector <DataType>  src (len_src * _nbr_chn);
	std::vector <DataType>  dst (_len * _nbr_chn);
	std::vector <DataType>  dst_m (_len);
	std::vector <DataType>  src_m (_len);
	SweepingSine   ss (1, _f_fs, _f_fs, _len);
	ss.generate (src_m.data ());
	for (long k = 0; k < len_src; ++k)
	{
		for (int c = 0; c < _nbr_chn; ++c)
		{
			src [k * _nbr_chn + c] = src_m [k * 2];
		}
	}
	filter.clear_buffers ();
	filter.process_block (dst.data (), src.data (), len_src);
	for (long k = 0; k < _len; ++k)
	{
		dst_m [k] = dst [k * _nbr_chn];
	}

	int            ret_val = ResultCheck <DataType>::check_delay (
		dst_m.data (), src_m.data (), _len, dly_ref, _f_fs
	);

	return ret_val;
}



template <class TO>
int	TestDelay <TO>::test_phaser (TO &filter, const double coef_arr [_nbr_coefs])
{
	const double   dly_ref =
		filter._delay + PolyphaseIir2Designer::compute_phase_delay (
			coef_arr, _nbr_coefs, _f_fs, true
		);

	std::vector <DataType>  src (_len * _nbr_chn);
	std::vector <DataType>  dst_1 (_len * _nbr_chn);
	std::vector <DataType>  dst_2 (_len * _nbr_chn);
	std::vector <DataType>  src_m (_len);
	std::vector <DataType>  dst_m (_len);
	SweepingSine   ss (1, _f_fs, _f_fs, _len);
	ss.generate (src_m.data ());
	for (long k = 0; k < _len; ++k)
	{
		for (int c = 0; c < _nbr_chn; ++c)
		{
			src [k * _nbr_chn + c] = src_m [k];
		}
	}
	filter.clear_buffers ();
	filter.process_block (dst_1.data (), dst_2.data (), src.data (), _len);
	for (long k = 0; k < _len; ++k)
	{
		dst_m [k] = dst_1 [k * _nbr_chn];
	}

	int            ret_val = ResultCheck <DataType>::check_delay (
		dst_m.data (), src_m.data (), _len, dly_ref, _f_fs
	);

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
constexpr double	TestDelay <TO>::_f_fs;
template <class TO>
constexpr int	TestDelay <TO>::_len;



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TestDelay_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
