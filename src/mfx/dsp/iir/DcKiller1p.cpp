/*****************************************************************************

        DcKiller1p.cpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/DcKiller1p.h"
#include "mfx/dsp/iir/TransSZBilin.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DcKiller1p::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	update_z_eq ();
}



void	DcKiller1p::set_cutoff_freq (float f0)
{
	assert (f0 > 0);

	_f0 = f0;
	if (_sample_freq > 0)
	{
		update_z_eq ();
	}
}



void	DcKiller1p::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_filter.process_block (dst_ptr, src_ptr, nbr_spl);
}



void	DcKiller1p::clear_buffers ()
{
	_filter.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DcKiller1p::update_z_eq ()
{
	assert (_sample_freq > 0);

	const float    k = TransSZBilin::compute_k_approx (_f0 * _inv_fs);

	static const float   eq_s_b [2] = { 0, 1 };
	static const float   eq_s_a [2] = { 1, 1 };
	float          eq_z_b [2];
	float          eq_z_a [2];
	TransSZBilin::map_s_to_z_one_pole_approx (
		eq_z_b, eq_z_a,
		eq_s_b, eq_s_a,
		k
	);

	_filter.set_z_eq (eq_z_b, eq_z_a);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
