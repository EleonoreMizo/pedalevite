/*****************************************************************************

        MoogLadderDAngeloData.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_MoogLadderDAngeloData_CODEHEADER_INCLUDED)
#define mfx_dsp_va_MoogLadderDAngeloData_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int N>
float	MoogLadderDAngeloData <N>::compute_g_max (float fmax_over_fs) noexcept
{
	assert (fmax_over_fs > 0);
	assert (fmax_over_fs < 0.5f);

	return float (tan (fstb::PI * fmax_over_fs));
}



template <int N>
float	MoogLadderDAngeloData <N>::compute_k0_max (float gmax) noexcept
{
	assert (gmax > 0);

	return 2 * _vt * (1 - 1 / (1 + gmax));
}



template <int N>
float	MoogLadderDAngeloData <N>::compute_alpha (float k) noexcept
{
	float          alpha = 1 + k;

	if (N > 1)
	{
		const float    k_nrt = pow (k, 1.0f / N);
		alpha = float (sqrt (1 + k_nrt * (k_nrt - 2 * cos (fstb::PI / N))));
	}

	return alpha;
}



template <int N>
float	MoogLadderDAngeloData <N>::compute_knorm_factor () noexcept
{
	return 1.f / fstb::ipowp (float (cos (fstb::PI / N)), N);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_MoogLadderDAngeloData_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

