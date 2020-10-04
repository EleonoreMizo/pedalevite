/*****************************************************************************

        DcKiller2p.cpp
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

#include "fstb/def.h"
#include "mfx/dsp/iir/DcKiller2p.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DcKiller2p::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	update_coefs ();
}



void	DcKiller2p::set_cutoff_freq (float f0)
{
	assert (f0 > 0);
	assert (f0 < _sample_freq * 0.5f);

	_cutoff_freq = f0;
	update_coefs ();
}



void	DcKiller2p::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



void	DcKiller2p::clear_buffers ()
{
	_v1 = 0;
	_v2 = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DcKiller2p::update_coefs ()
{
	constexpr double   q   = fstb::SQRT2 * 0.5;
	const double       g2  = tan (fstb::PI * _cutoff_freq / _sample_freq);
	const double       d2  = 2 / q;
	const double       nrm = 1 / sqrt (1 + d2 * g2 + g2 * g2);
	_m = float (   nrm * nrm);
	_g = float (2 * g2 * nrm);
	_d = float (    d2 * nrm);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
