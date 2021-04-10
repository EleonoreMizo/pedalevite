/*****************************************************************************

        SplitMultibandLinBase.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_SplitMultibandLinBase_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_SplitMultibandLinBase_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransSZBilin.h"

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
Name: get_actual_xover_freq
Description:
	Retrieves the actual crossover frequency, after the splitter target
	frequency has been set.
Input parameters:
	- split_idx: Crossover index, >= 0.
Returns:
	The actual crossover frequency, in Hz.
Throws: Nothing
==============================================================================
*/

template <int O>
float	SplitMultibandLinBase <O>::get_actual_xover_freq (int split_idx) const noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));

	const Splitter &  split = _split_arr [split_idx];
	assert (split._freq_act > 0);

	return float (TransSZBilin::unwarp_freq (split._freq_act, _sample_freq));
}



/*
==============================================================================
Name: get_global_delay
Description:
	Retrieves the global filterbank delay. Effective only when all crossover
	have been set.
Returns: The delay, in samples. > 0
Throws: Nothing
==============================================================================
*/

template <int O>
int	SplitMultibandLinBase <O>::get_global_delay () const noexcept
{
	assert (_sample_freq > 0);
	assert (! _split_arr.empty ());

	return _max_delay;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// 100 ms should be enough for most uses.
template <int O>
const double	SplitMultibandLinBase <O>::_max_dly_time = 0.100;



template <int O>
template <int N>
int	SplitMultibandLinBase <O>::FilterEq <N>::fill_with (const float coef_ptr [SplitMultibandLinBase <O>::FilterEq <N>::_nbr_coef]) noexcept
{
	for (int k = 0; k < N; ++k)
	{
		_b [k] = coef_ptr [    k];
		_a [k] = coef_ptr [N + k];
	}
	assert (fstb::is_eq (_a [0], 1.f));

	return _nbr_coef;
}



template <int O>
bool	SplitMultibandLinBase <O>::set_sample_freq_internal (double sample_freq, int max_buf_size)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);

	const int      max_dly_time_spl =
		fstb::round_int (float (_max_dly_time * _sample_freq));
	_delay.setup (max_dly_time_spl, max_buf_size);
	
	return ! _split_arr.empty ();
}



template <int O>
void	SplitMultibandLinBase <O>::set_splitter_coef_internal (int split_idx, float freq, const float coef_arr [O], float dly_ofs) noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);
	assert (coef_arr != nullptr);
	assert (dly_ofs > -1);

	Splitter &     split = _split_arr [split_idx];

	split._freq_tgt = freq;
	split._dly_ofs  = dly_ofs;

	// Stores the coefficients at the right places
	int            coef_ofs = 0;
	float          a_n      = 1;
	for (auto &eq : split._eq_2p)
	{
		coef_ofs += eq.fill_with (coef_arr + coef_ofs);
		a_n *= eq._a [2];
	}
	for (auto &eq : split._eq_1p)
	{
		coef_ofs += eq.fill_with (coef_arr + coef_ofs);
		a_n *= eq._a [1];
	}
	assert (fstb::is_eq (a_n, 1.f));

	// Computes the 1st-order coefficient for the denominator of the whole
	// filter. This is b1 in eq. 21
	float          b1 = 0;
	static constexpr int nbr_flt = _nbr_2p + _nbr_1p;
	for (int k = 0; k < nbr_flt; ++k)
	{
		float          prod = 1;
		for (int j = 0; j < nbr_flt; ++j)
		{
			const int      order = (j == k) ? 1 : 0;
			const float    coef  =
				  (j < _nbr_2p)
				? split._eq_2p [j          ]._a [order]
				: split._eq_1p [j - _nbr_2p]._a [order];
			prod *= coef;
		}
		b1 += prod;
	}
	split._b1 = b1;
}



// update_post() must be called afterwards
template <int O>
bool	SplitMultibandLinBase <O>::update_single_splitter (int split_idx) noexcept
{
	assert (_sample_freq > 0);
	assert (split_idx >= 0);
	assert (split_idx < int (_split_arr.size ()));

	auto &         split = _split_arr [split_idx];
	const float    f     = split._freq_tgt; // Hz

	// Is the band already set?
	const bool     ok_flag = (f > 0);
	if (ok_flag)
	{
		// Group delay at DC
		// Eq. 22 with bilinear frequency prewarping
		split._freq_warp = float (TransSZBilin::prewarp_freq (f, _sample_freq));
		split._dly_comp  = split._b1 / (2 * split._freq_warp);
		split._dly_comp *= 1 + split._dly_ofs;

		split._dly_int   = fstb::round_int (float (split._dly_comp));

		// For the highest frequencies, the rounded delay may be 0.
		// Keep it greater than 0.
		split._dly_int   = std::max (split._dly_int, 1);

		// Evaluates the actual cutoff frequency corresponding to this
		// rounded delay time
		float          ratio = 1;
		ratio = split._dly_comp / split._dly_int;
		split._freq_act = split._freq_warp * ratio;
	}

	return ok_flag;
}



// We need to compute the maximum delay time for the whole filterbank,
// take all the actual delays into account and recompute the crossover
// frequencies according to these changes.
// Then we can compute the z-plane equations for all filters.
template <int O>
void	SplitMultibandLinBase <O>::update_post () noexcept
{
	assert (_sample_freq > 0);

	_max_delay = _split_arr [0]._dly_int;
	assert (_max_delay <= _delay.get_max_delay ());

	float          freq_tgt_old = 0;
	const int      nbr_split    = int (_split_arr.size ());
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		Splitter &     split = _split_arr [split_idx];
		if (split._freq_tgt <= freq_tgt_old)
		{
			// Splitter not set or invalid set of frequencies: stop updating
			break;
		}

		// The difference between the maximum delay and LPF DC group delay
		// on the upper band.
		split._dly_b = _max_delay;
		if (split_idx + 1 < nbr_split)
		{
			split._dly_b -= _split_arr [split_idx + 1]._dly_int;
		}

		freq_tgt_old = split._freq_tgt;
	}
}



// Simplified bilinear transforms:
// - No frequency prewarping
// - Assumes a0 == 1
// f0_pi_fs = f0 * pi / fs
template <int O>
void	SplitMultibandLinBase <O>::bilinear_2p (Eq2p &eq_z, const Eq2p &eq_s, double f0_pi_fs) noexcept
{
	assert (fstb::is_eq (eq_s._a [0], 1.f));

	const double   k  = 1 / f0_pi_fs;
	const double   kk = k*k;

	const double   b1k  = eq_s._b [1] * k;
	const double   b2kk = eq_s._b [2] * kk;
	const double   b2kk_plus_b0 = b2kk + eq_s._b [0];
	const double   b0z = b2kk_plus_b0 + b1k;
	const double   b2z = b2kk_plus_b0 - b1k;
	const double   b1z = 2 * (eq_s._b [0] - b2kk);

	const double   a1k  = eq_s._a [1] * k;
	const double   a2kk = eq_s._a [2] * kk;
	const double   a2kk_plus_a0 = a2kk + 1;
	const double   a0z = a2kk_plus_a0 + a1k;
	const double   a2z = a2kk_plus_a0 - a1k;
	const double   a1z = 2 * (1 - a2kk);

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double	mult = 1 / a0z;

	eq_z._b [0] = float (b0z * mult);
	eq_z._b [1] = float (b1z * mult);
	eq_z._b [2] = float (b2z * mult);

	eq_z._a [0] = 1.f;
	eq_z._a [1] = float (a1z * mult);
	eq_z._a [2] = float (a2z * mult);
}



template <int O>
void	SplitMultibandLinBase <O>::bilinear_1p (Eq1p &eq_z, const Eq1p &eq_s, double f0_pi_fs) noexcept
{
	assert (fstb::is_eq (eq_s._a [0], 1.f));

	const double   k   = 1 / f0_pi_fs;
	const double   b1k = eq_s._b [1] * k;
	const double   b1z = eq_s._b [0] - b1k;
	const double   b0z = eq_s._b [0] + b1k;

	const double   a1k = eq_s._a [1] * k;
	const double   a1z = 1 - a1k;
	const double   a0z = 1 + a1k;

	// IIR coefficients
	assert (! fstb::is_null (a0z));
	const double   mult = 1 / a0z;

	eq_z._b [0] = float (b0z * mult);
	eq_z._b [1] = float (b1z * mult);

	eq_z._a [0] = 1.f;
	eq_z._a [1] = float (a1z * mult);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_SplitMultibandLinBase_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
