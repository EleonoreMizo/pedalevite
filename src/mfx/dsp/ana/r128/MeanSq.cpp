/*****************************************************************************

        MeanSq.cpp
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/FilterK.h"
#include "mfx/dsp/ana/r128/MeanSq.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MeanSq::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	if (_win_len > 0)
	{
		set_win_len (_win_len);
	}
}



void	MeanSq::set_win_len (double t)
{
	assert (_sample_freq > 0);
	assert (t > 0);

	_win_len = t;
	int            len = fstb::round_int (_win_len * _sample_freq);
	len = std::max (len, 1);

	// Not 1/t because the window length as an integer number of samples may
	// differ slightly from the specified t value. By using the calculated
	// size in samples, we won't introduce any scale error.
	_scale = 1.0 / double (len);

	_avg.set_win_len (len);
	clear_buffers ();
}



float	MeanSq::process_sample (float x) noexcept
{
	assert (_sample_freq > 0);
	assert (_win_len > 0);

	const auto     sum = push_sample_and_get_sum (x);
	const auto     val = compute_mean_from_sum (sum);

	return val;
}



void	MeanSq::analyse_sample (float x) noexcept
{
	assert (_sample_freq > 0);
	assert (_win_len > 0);

	push_sample_and_get_sum (x);
}



void	MeanSq::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (_win_len > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



void	MeanSq::analyse_block (const float src_ptr [], int nbr_spl) noexcept
{
	assert (_sample_freq > 0);
	assert (_win_len > 0);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		push_sample_and_get_sum (src_ptr [pos]);
	}
}



float	MeanSq::get_val () const noexcept
{
	assert (_sample_freq > 0);
	assert (_win_len > 0);

	const auto     sum = _avg.get_val ();
	const auto     val = compute_mean_from_sum (sum);

	return val;
}



void	MeanSq::clear_buffers () noexcept
{
	_avg.clear_buffers ();
}



// Smallest values are clipped to about -300 LUFS.
float	MeanSq::conv_mean_sq_to_lufs (float msq) noexcept
{
	assert (msq >= 0);

	constexpr auto lvl_min =  1e-15f; // -300 dB
	constexpr auto msq_min = fstb::sq (lvl_min);
	constexpr auto l2_mult = float (10 * fstb::LOG10_2);

	const auto     msq_c   = std::max (msq, msq_min);
	const auto     lufs    =
		FilterK::_ofs_997 + l2_mult * fstb::Approx::log2 (msq_c);

	return lufs;
}



float	MeanSq::conv_lufs_to_mean_sq (float lufs) noexcept
{
	constexpr auto e2_mult = float (1.0 / (10 * fstb::LOG10_2));
	const auto     msq     =
		fstb::Approx::exp2 ((lufs - FilterK::_ofs_997) * e2_mult);

	return msq;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	MeanSq::push_sample_and_get_sum (float x) noexcept
{
	return _avg.process_sample (x * x);
}



float	MeanSq::compute_mean_from_sum (double sum) const noexcept
{
	// We clamp at 0 because in some cases the sum may be slightly below 0,
	// because of slight rounding errors during the sum calculation.
	const auto     val = std::max (sum * _scale, 0.0);

	return float (val);
}



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
