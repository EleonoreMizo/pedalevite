/*****************************************************************************

        OscSaw.cpp
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

#include "mfx/dsp/ctrl/lfo/OscSaw.h"

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



void	OscSaw::do_set_sample_freq (double sample_freq)
{
	_phase_gen.set_sample_freq (sample_freq);
}



void	OscSaw::do_set_period (double per)
{
	_phase_gen.set_period (per);
}



void	OscSaw::do_set_phase (double phase)
{
	_phase_gen.set_phase (phase);
}



void	OscSaw::do_set_chaos (double chaos)
{
	_phase_gen.set_chaos (chaos);
}



void	OscSaw::do_set_phase_dist (double dist)
{
	_phase_dist.set_phase_dist (dist);
}



void	OscSaw::do_set_phase_dist_offset (double ofs)
{
	_phase_dist.set_phase_dist_offset (ofs);
}



void	OscSaw::do_set_sign (bool inv_flag)
{
	_inv_flag = inv_flag;
}



void	OscSaw::do_set_polarity (bool unipolar_flag)
{
	_unipolar_flag = unipolar_flag;
}



void	OscSaw::do_set_variation (int /*param*/, double /*val*/)
{
	// Nothing
}



bool	OscSaw::do_is_using_variation (int /*param*/) const
{
	return false;
}



void	OscSaw::do_tick (long nbr_spl)
{
	_phase_gen.tick (nbr_spl);
}



double	OscSaw::do_get_val () const
{
	double         val = _phase_dist.process_phase (_phase_gen.get_phase ());
	if (_inv_flag)
	{
		if (! _unipolar_flag)
		{
			val *= 2;
		}
		val = 1 - val;
	}
	else if (! _unipolar_flag)
	{
		val = 2 * val - 1;
	}

	return val;
}



double	OscSaw::do_get_phase () const
{
	return _phase_gen.get_phase ();
}



void	OscSaw::do_clear_buffers ()
{
	_phase_gen.clear_buffers ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
