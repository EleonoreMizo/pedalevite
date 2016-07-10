/*****************************************************************************

        PhaseDist.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_lfo_PhaseDist_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_PhaseDist_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// 0.5 = neutral (no distortion)
void	PhaseDist::set_phase_dist (double dist)
{
	assert (dist >= 0);
	assert (dist <= 1);

	const double   margin = 1e-4;
	if (dist < margin)
	{
		_thr    = 0;
		_s1_mul = 1;
		_s2_add = 1;
		_s2_mul = 0.5;
	}
	else if (dist > 1 - margin)
	{
		_thr    = 1;
		_s1_mul = 0.5;
		_s2_add = 0;
		_s2_mul = 1;
	}
	else
	{
		_thr    = dist;
		_s1_mul = 1 / (2 *      dist);
		_s2_add = 1 - 2 * dist;
		_s2_mul = 1 / (2 * (1 - dist));
	}
}



double	PhaseDist::process_phase (double phase) const
{
	assert (phase >= 0);
	assert (phase < 1);

	if (_active_flag)
	{
		if (phase < _thr)
		{
			phase *= _s1_mul;
		}
		else
		{
			phase += _s2_add;
			phase *= _s2_mul;
		}
	}

	assert (phase >= 0);
	assert (phase < 1);

	return phase;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_lfo_PhaseDist_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
