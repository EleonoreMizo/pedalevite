/*****************************************************************************

        AllPassCascade.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_AllPassCascade_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_AllPassCascade_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_coefs
Description:
	Sets the filter coefficients. They are arranged in this order:
	- For each 2-pole sections, first comes b0 then b1, b0 and b1 being
		the z-equation numerator coefficients for the corresponding z^-N orders.
	- Then all the coefficients of the 1-pole sections
Input parameters:
	- b_arr: pointer on a list of the 2-pole and 1-pole coefficients.
		Each coefficient should be in [-1 ; 1].
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::set_coefs (const T b_arr [_order]) noexcept
{
	assert (b_arr != nullptr);

	int            coef_pos = 0;
	for (auto &filter : _f2p_coef_arr)
	{
		const auto     b0 = b_arr [coef_pos    ];
		const auto     b1 = b_arr [coef_pos + 1];
		filter._r0 = b0;
		filter._r1 = b1;
		coef_pos += 2;
	}
	for (auto &filter : _f1p_coef_arr)
	{
		filter._r0 = b_arr [coef_pos];
		++ coef_pos;
	}
}



/*
==============================================================================
Name: get_coefs
Description:
	Retrives the current filter coefficients. See set_coefs() for layout.
Output parameters:
	- b_arr: pointer on a list receiving the coefficients of 2-pole and 1-pole
		sections. The list should be N2P * 2 + N1P long.
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::get_coefs (T b_arr [_order]) const noexcept
{
	assert (b_arr != nullptr);

	int            coef_pos = 0;
	for (auto &filter : _f2p_coef_arr)
	{
		b_arr [coef_pos    ] = filter._r0;
		b_arr [coef_pos + 1] = filter._r1;
		coef_pos += 2;
	}
	for (auto &filter : _f1p_coef_arr)
	{
		b_arr [coef_pos] = filter._r0;
		++ coef_pos;
	}
}



/*
==============================================================================
Name: copy_z_eq
Description:
	Copies the filter coefficient from another filter cascade
Input parameters:
	- other: the filter whose coefficients should be copied from.
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::copy_z_eq (const ThisType &other) noexcept
{
	_f2p_coef_arr = other._f2p_coef_arr;
	_f1p_coef_arr = other._f1p_coef_arr;
}



/*
==============================================================================
Name: process_sample
Description:
	Filters a single sample.
Input parameters:
	- x: input sample
Returns: the filtered sample
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
float	AllPassCascade <T, N2P, N1P>::process_sample (T x) noexcept
{
	// 2nd-order sections
	if (N2P > 0)
	{
		for (int pos = 0; pos < N2P; ++pos)
		{
			x = proc_spl_ap2 (x, pos, [](const Data2p &, T, T) noexcept {});
		}
		auto &         s_y = _f2p_mem_arr [N2P];
		s_y._r1 = s_y._r0;
		s_y._r0 = x;
	}

	// 1st-order sections
	if (N1P > 0)
	{
		for (int pos = 0; pos < N1P; ++pos)
		{
			x = proc_spl_ap1 (x, pos, [](const Data1p &, T) noexcept {});
		}
		_f1p_mem_arr [N1P]._r0 = x;
	}

	return x;
}



/*
==============================================================================
Name: process_sample
Description:
	Filters a single sample and updates the coefficient list according to
	per-sample coefficient increments.
Input parameters:
	- x: input sample to be filtered
	- inc_b: increments for each coefficient. See set_coefs() for layout.
Returns: filtered sample
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
float	AllPassCascade <T, N2P, N1P>::process_sample (T x, const T inc_b [_order]) noexcept
{
	assert (inc_b != nullptr);

	// 2nd-order sections
	if (N2P > 0)
	{
		for (int pos = 0; pos < N2P; ++pos)
		{
			x = proc_spl_ap2 (
				x, pos,
				[pos, inc_b] (Data2p &coef, T b0, T b1) noexcept {
					coef._r0 = b0 + inc_b [pos * 2    ];
					coef._r1 = b1 + inc_b [pos * 2 + 1];
				}
			);
		}
		auto &         s_y = _f2p_mem_arr [N2P];
		s_y._r1 = s_y._r0;
		s_y._r0 = x;
	}

	// 1st-order sections
	if (N1P > 0)
	{
		for (int pos = 0; pos < N1P; ++pos)
		{
			x = proc_spl_ap1 (
				x, pos,
				[pos, inc_b] (Data1p &coef, T b0) noexcept {
					coef._r0 = b0 + inc_b [N2P * 2 + pos];
				}
			);
		}
		_f1p_mem_arr [N1P]._r0 = x;
	}

	return x;
}



/*
==============================================================================
Name: process_block
Description:
	Filters a block of samples. Can work in-place.
Input parameters:
	- dst_ptr: pointer on a buffer receiving the output samples.
	- src_ptr: pointer on a buffer containing the input samples.
	- nbr_spl: Number of samples to process.
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	// 1 1st-order section only
	if (N2P == 0 && N1P == 1)
	{
		const auto     b0 = _f1p_coef_arr [0]._r0;
		auto           x0 = _f1p_mem_arr [0]._r0;
		auto           y0 = _f1p_mem_arr [1]._r0;

		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const auto     x = src_ptr [pos];
			y0 = b0 * (x - y0) + x0;
			dst_ptr [pos] = y0;
			x0 = x;
		}

		_f1p_mem_arr [0]._r0 = x0;
		_f1p_mem_arr [1]._r0 = y0;
	}

	// 1 2nd-order section only
	if (N2P == 1 && N1P == 0)
	{
		const auto     b0 = _f2p_coef_arr [0]._r0;
		const auto     b1 = _f2p_coef_arr [0]._r1;
		auto           x0 = _f2p_mem_arr [0]._r0;
		auto           x1 = _f2p_mem_arr [0]._r1;
		auto           y0 = _f2p_mem_arr [1]._r0;
		auto           y1 = _f2p_mem_arr [1]._r1;

		const int      m2 = nbr_spl & ~(2-1);
		for (int pos = 0; pos < m2; pos += 2)
		{
			const auto     xa = src_ptr [pos    ];
			const auto     xb = src_ptr [pos + 1];

			y1 = b0 * (xa - y1) + b1 * (x0 - y0) + x1;
			y0 = b0 * (xb - y0) + b1 * (xa - y1) + x0;

			dst_ptr [pos    ] = y1;
			dst_ptr [pos + 1] = y0;
			x0 = xb;
			x1 = xa;
		}

		_f2p_mem_arr [0]._r0 = x0;
		_f2p_mem_arr [0]._r1 = x1;
		_f2p_mem_arr [1]._r0 = y0;
		_f2p_mem_arr [1]._r1 = y1;

		if (nbr_spl != m2)
		{
			dst_ptr [m2] = process_sample (src_ptr [m2]);
		}
	}

	// 1 2nd-order + 1 1st-order section only
	else if (N2P == 1 && N1P == 1)
	{
		const auto     b0 = _f2p_coef_arr [0]._r0;
		const auto     b1 = _f2p_coef_arr [0]._r1;
		const auto     b2 = _f1p_coef_arr [0]._r0;
		auto           x0 = _f2p_mem_arr [0]._r0;
		auto           x1 = _f2p_mem_arr [0]._r1;
		auto           y0 = _f2p_mem_arr [1]._r0;
		auto           y1 = _f2p_mem_arr [1]._r1;
		auto           x2 = _f1p_mem_arr [0]._r0;
		auto           y2 = _f1p_mem_arr [1]._r0;

		const int      m2 = nbr_spl & ~(2-1);
		for (int pos = 0; pos < m2; pos += 2)
		{
			const auto     xa = src_ptr [pos    ];
			const auto     xb = src_ptr [pos + 1];

			y1 = b0 * (xa - y1) + b1 * (x0 - y0) + x1;
			const float    y3 = b2 * (y1 - y2) + x2;

			y0 = b0 * (xb - y0) + b1 * (xa - y1) + x0;
			y2 = b2 * (y0 - y3) + y1;

			dst_ptr [pos    ] = y3;
			dst_ptr [pos + 1] = y2;
			x0 = xb;
			x1 = xa;
			x2 = y0;
		}

		_f2p_mem_arr [0]._r0 = x0;
		_f2p_mem_arr [0]._r1 = x1;
		_f2p_mem_arr [1]._r0 = y0;
		_f2p_mem_arr [1]._r1 = y1;
		_f1p_mem_arr [0]._r0 = x2;
		_f1p_mem_arr [1]._r0 = y2;

		if (nbr_spl != m2)
		{
			dst_ptr [m2] = process_sample (src_ptr [m2]);
		}
	}

	else
	{
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			dst_ptr [pos] = process_sample (src_ptr [pos]);
		}
	}
}



/*
==============================================================================
Name: process_block
Description:
Input parameters:
	- dst_ptr: pointer on a buffer receiving the output samples.
	- src_ptr: pointer on a buffer containing the input samples.
	- nbr_spl: Number of samples to process.
	- inc_b: pointer on an array with sample increments for each coefficient.
		Layout is the same as set_coefs()
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::process_block (T dst_ptr [], const T src_ptr [], int nbr_spl, const T inc_b [_order]) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (inc_b   != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos], inc_b);
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Resets the internal state to 0.
Throws: Nothing
==============================================================================
*/

template <typename T, int N2P, int N1P>
void	AllPassCascade <T, N2P, N1P>::clear_buffers () noexcept
{
	for (auto &mem : _f2p_mem_arr)
	{
		mem = Data2p {};
	}
	for (auto &mem : _f1p_mem_arr)
	{
		mem = Data1p {};
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int N2P, int N1P>
template <typename F>
float	AllPassCascade <T, N2P, N1P>::proc_spl_ap2 (float x, int pos, F fnc_inc) noexcept
{
	auto &         coef = _f2p_coef_arr [pos];
	const auto     b0   = coef._r0;
	const auto     b1   = coef._r1;
	auto &         s_x  = _f2p_mem_arr [pos];
	auto &         s_y  = _f2p_mem_arr [pos + 1];
	const auto     y    =
		  b0 * (x       - s_y._r1)
		+ b1 * (s_x._r0 - s_y._r0)
		+       s_x._r1;
	s_x._r1 = s_x._r0;
	s_x._r0 = x;
	fnc_inc (coef, b0, b1);

	return y;
}



template <typename T, int N2P, int N1P>
template <typename F>
float	AllPassCascade <T, N2P, N1P>::proc_spl_ap1 (float x, int pos, F fnc_inc) noexcept
{
	auto &         coef = _f1p_coef_arr [pos];
	const auto     b0   = coef._r0;
	auto &         s_x  = _f1p_mem_arr [pos];
	auto &         s_y  = _f1p_mem_arr [pos + 1];
	const auto     y    = b0 * (x - s_y._r0) + s_x._r0;
	s_x._r0 = x;
	fnc_inc (coef, b0);

	return y;
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_AllPassCascade_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
