/*****************************************************************************

        NgSetup.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_nzbl_NgSetup_CODEHEADER_INCLUDED)
#define mfx_pi_nzbl_NgSetup_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <algorithm>
#include <array>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace nzbl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NgSetup::EnvTimes	NgSetup::compute_env_times (int band_idx) noexcept
{
	assert (band_idx >= 0);

	// Computes the envelope times.
	// For the lowest band, we use 30 Hz as reference to make sure that the
	// lowest frequencies are not distorted. Consequently, the gate will be
	// slow to react. But low frequency rumble is generally steady (it comes
	// from the power supply) therefore this is not a problem.
	float          f = 30;
	if (band_idx > 0)
	{
		f = compute_split_freq (band_idx - 1);
	}
	const int      mult   = 16;
	float          t      = float (mult / (2 * fstb::PI)) / f;
	// Longer release helps preventing amplitude modulation on periodic
	// noise bursts
	const float    min_at = 0.005f;
	const float    min_rt = 0.050f;
	const float    at = std::max (t, min_at);
	const float    rt = std::max (t, min_rt);

	return { at, rt };
}



template <class B>
double	NgSetup::update_filters_compute_latency (B &splitter, double f_lat, double sample_freq, float ka, float kb) noexcept
{
	constexpr int  order_0 = B::_nbr_2p_0 * 2 + B::_nbr_1p_0;
	constexpr int  order_1 = B::_nbr_2p_1 * 2 + B::_nbr_1p_1;
	static_assert (order_0 == 2, "");
	static_assert (order_1 == 3, "");

	assert (sample_freq > 0);
	assert (f_lat < sample_freq * 0.5f);
	assert (ka > 0);
	assert (kb >= 0);

	using DEq2p = mfx::dsp::iir::DesignEq2p;
	using Tsz   = dsp::iir::TransSZBilin;

	double         latency = 0;

	const float    x1      = ka + kb;
	const float    x2      = ka;
	const float    inv_fs  = float (1 / sample_freq);

	const int      nbr_split = splitter.get_nbr_bands () - 1;
	for (int split_idx = 0; split_idx < nbr_split; ++split_idx)
	{
		const float    f = compute_split_freq (split_idx);
		const float    k = Tsz::compute_k_approx (f * inv_fs);

		float          zb_1o [3];
		float          zb_1e [2];
		float          zb_0 [3];
		Tsz::map_s_to_z_ap1_approx (zb_1e,     k);
		Tsz::map_s_to_z_ap2_approx (zb_1o, x2, k);
		Tsz::map_s_to_z_ap2_approx (zb_0 , x1, k);

		const std::array <float, order_0> eq_0 =
		{
			zb_0 [0], zb_0 [1]
		};
		const std::array <float, order_1> eq_1 =
		{
			zb_1o [0], zb_1o [1],
			zb_1e [0]
		};

		splitter.set_splitter_coef (split_idx, eq_0.data (), eq_1.data ());

		if (f_lat >= 0)
		{
			const std::array <float, order_0 + 1> bz {
				eq_0 [0], eq_0 [1], 1
			};
			const std::array <float, order_0 + 1> az {
				1       , eq_0 [1], eq_0 [0]
			};
			const auto     gd_split = DEq2p::compute_group_delay (
				bz.data (), az.data (), sample_freq, f_lat
			);
			latency += gd_split;
		}
	}

	latency = std::max (latency, 0.0);

	return latency;
}



constexpr float	NgSetup::compute_split_freq (int split_idx) noexcept
{
	return float (125 << split_idx);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_nzbl_NgSetup_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
