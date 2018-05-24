/*****************************************************************************

        OscStepSeq.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/dsp/ctrl/lfo/OscStepSeq.h"

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



void	OscStepSeq::set_nbr_steps (int nbr_steps)
{
	assert (nbr_steps >= 2);
	assert (nbr_steps <= _max_nbr_steps);

	_nbr_steps = nbr_steps;
}



// Assumed bipolar
void	OscStepSeq::set_val (int index, float val)
{
	assert (index >= 0);
	assert (index < _max_nbr_steps); // Or _nbr_steps?
	assert (val >= -1); // Are these checks really required?
	assert (val <= +1);

	Step &         step = _step_arr [index];
	step._val = val;
}



void	OscStepSeq::set_curve (int index, Curve c)
{
	assert (index >= 0);
	assert (index < _max_nbr_steps); // Or _nbr_steps?
	assert (c >= 0);
	assert (c <  Curve_NBR_ELT);

	Step &         step = _step_arr [index];
	step._curve = c;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscStepSeq::do_set_sample_freq (double sample_freq)
{
	_phase_gen.set_sample_freq (sample_freq);
}



void	OscStepSeq::do_set_period (double per)
{
	_phase_gen.set_period (per);
}



void	OscStepSeq::do_set_phase (double phase)
{
	_phase_gen.set_phase (phase);
}



void	OscStepSeq::do_set_chaos (double chaos)
{
	_phase_gen.set_chaos (chaos);
}



void	OscStepSeq::do_set_phase_dist (double dist)
{
	_phase_dist.set_phase_dist (dist);
}



void	OscStepSeq::do_set_phase_dist_offset (double ofs)
{
	_phase_dist.set_phase_dist_offset (ofs);
}



void	OscStepSeq::do_set_sign (bool inv_flag)
{
	_inv_flag = inv_flag;
}



void	OscStepSeq::do_set_polarity (bool unipolar_flag)
{
	_unipolar_flag = unipolar_flag;
}



void	OscStepSeq::do_set_variation (int /*param*/, double /*val*/)
{
	// Nothing
}



bool	OscStepSeq::do_is_using_variation (int /*param*/) const
{
	return false;
}



void	OscStepSeq::do_tick (int nbr_spl)
{
	_phase_gen.tick (nbr_spl);
}



double	OscStepSeq::do_get_val () const
{
	double         phase = _phase_dist.process_phase (_phase_gen.get_phase ());
	phase *= _nbr_steps;
	const int      index = fstb::round_int (phase);
	const double   pos   = phase - double (index);
	const Step &   step  = _step_arr [index];
	const Step &   sprev = _step_arr [(index + _nbr_steps - 1) % _nbr_steps];
	const double   lerp  = map_pos (pos, step._curve);

	// val is assumed bipolar
	double         val   = sprev._val + lerp * (step._val - sprev._val);
	if (_inv_flag)
	{
		val = -val;
	}
	if (_unipolar_flag)
	{
		val = 0.5 * val + 0.5;
	}

	return val;
}



double	OscStepSeq::do_get_phase () const
{
	return _phase_gen.get_phase ();
}



void	OscStepSeq::do_clear_buffers ()
{
	_phase_gen.clear_buffers ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	OscStepSeq::map_pos (double pos, Curve c)
{
	assert (pos >= 0);
	assert (pos <= 1);

	switch (c)
	{
	case Curve_HARD:
		pos = 1;
		break;
	case Curve_SMOOTH1:
		pos = smooth (smooth (smooth (smooth (smooth (pos)))));
		break;
	case Curve_SMOOTH2:
		pos = smooth (smooth (smooth (pos)));
		break;
	case Curve_SMOOTH3:
		pos = smooth (pos);
		break;
	case Curve_LINEAR:
		// Nothing
		break;

	default:
		assert (false);
		break;
	}

	return pos;
}



double	OscStepSeq::smooth (double x)
{
	const double   x2 = x * x;

	return x2 * (3 - 2 * x);
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
