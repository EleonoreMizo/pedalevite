/*****************************************************************************

        OscTri.cpp
        Author: Laurent de Soras, 2016

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

#include "mfx/dsp/ctrl/lfo/OscTri.h"

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



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscTri::do_set_sample_freq (double sample_freq)
{
	_phase_gen.set_sample_freq (sample_freq);
}



void	OscTri::do_set_period (double per)
{
	_phase_gen.set_period (per);
}



void	OscTri::do_set_phase (double phase)
{
	_phase_gen.set_phase (phase);
}



void	OscTri::do_set_chaos (double chaos)
{
	_phase_gen.set_chaos (chaos);
}



void	OscTri::do_set_phase_dist (double dist)
{
	_phase_dist.set_phase_dist (dist);
}



void	OscTri::do_set_sign (bool inv_flag)
{
	_inv_flag = inv_flag;
}



void	OscTri::do_set_polarity (bool unipolar_flag)
{
	_unipolar_flag = unipolar_flag;
}



void	OscTri::do_set_variation (int /*param*/, double /*val*/)
{
	// Nothing
}



bool	OscTri::do_is_using_variation (int /*param*/) const
{
	return false;
}



void	OscTri::do_tick (long nbr_spl)
{
	_phase_gen.tick (nbr_spl);
}



double	OscTri::do_get_val () const
{
	double         val = _phase_dist.process_phase (_phase_gen.get_phase ());
	val = fabs (val - 0.5f); // 0.5 ... 0 ... 0.5

	if (_inv_flag)
	{
		val = (_unipolar_flag) ?     val * 2 : val * 4 - 1;
	}
	else
	{
		val = (_unipolar_flag) ? 1 - val * 2 : 1 - val * 4;
	}

	return val;
}



double	OscTri::do_get_phase () const
{
	return _phase_gen.get_phase ();
}



void	OscTri::do_clear_buffers ()
{
	_phase_gen.clear_buffers ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
