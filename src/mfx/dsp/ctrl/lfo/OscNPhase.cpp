/*****************************************************************************

        OscNPhase.cpp
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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/ctrl/lfo/OscNPhase.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OscNPhase::OscNPhase (bool biphase_flag)
:	_sample_freq (44100)
,	_phase (0)
,	_step (0)
,	_k_phase (0)
,	_k_step (0)
,	_variation_arr ()
,	_period (1)
,	_inv_np (1)
,	_nc (1)
,	_np (1)
,	_phase_dist ()
,	_inv_flag (false)
,	_unipolar_flag (false)
,	_biphase_flag (biphase_flag)
{
	_variation_arr [Variation_TIME ] = 0;
	_variation_arr [Variation_SHAPE] = 0;

	update_period ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscNPhase::do_set_sample_freq (double sample_freq)
{
	_sample_freq = sample_freq;
	update_period ();
}



void	OscNPhase::do_set_period (double per)
{
	_period = per;
	update_period ();
}



void	OscNPhase::do_set_phase (double phase)
{
	_phase   = phase;
	_k_phase = phase * _nc * _np;
}



void	OscNPhase::do_set_phase_dist (double dist)
{
	_phase_dist.set_phase_dist (dist);
}



void	OscNPhase::do_set_phase_dist_offset (double ofs)
{
	_phase_dist.set_phase_dist_offset (ofs);
}



void	OscNPhase::do_set_chaos (double /*chaos*/)
{
	// Nothing
}



void	OscNPhase::do_set_sign (bool inv_flag)
{
	_inv_flag = inv_flag;
}



void	OscNPhase::do_set_polarity (bool unipolar_flag)
{
	_unipolar_flag = unipolar_flag;
}



void	OscNPhase::do_set_variation (int param, double val)
{
	if (is_using_variation (param))
	{
		_variation_arr [param] = val;
		update_period ();
	}
}



bool	OscNPhase::do_is_using_variation (int param) const
{
	return (   param == Variation_TIME
	        || param == Variation_SHAPE);
}



void	OscNPhase::do_tick (long nbr_spl)
{
	_phase += _step * nbr_spl;
	if (_phase >= 1)
	{
		const int		phase_int = fstb::floor_int (_phase);
		_phase -= phase_int;
	}

	_k_phase += _k_step * nbr_spl;
	if (_k_phase >= _np)
	{
		_k_phase = fmod (_k_phase, _np);
	}
}



double	OscNPhase::do_get_val () const
{
	const int		k     = fstb::floor_int (_k_phase);
	double			phase = _phase + k * _inv_np;	// [0 ; 2[
	while (phase >= 1)
	{
		phase -= 1;
	}
	phase = _phase_dist.process_phase (phase);

	float          val   = fstb::Approx::sin_nick_2pi (0.5f - float (phase));
	if (_inv_flag)
	{
		val = -val;
	}
	if (_unipolar_flag)
	{
		val = (val + 1) * 0.5f;
	}

	return val;
}



double	OscNPhase::do_get_phase () const
{
	return _phase;
}



void	OscNPhase::do_clear_buffers ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OscNPhase::update_period ()
{
	const float		ln_nc = float (_variation_arr [Variation_TIME] * 8);
	_nc = fstb::Approx::exp2 (ln_nc);
	if (_biphase_flag)
	{
		_np = 2;
		_inv_np = _variation_arr [Variation_SHAPE];
	}
	else
	{
		_np = fstb::round_int (2 + 6 * _variation_arr [Variation_SHAPE]);
		_inv_np = 1.0f / _np;
	}

	const double	ps = _period * _sample_freq;

	_step = 1.0 / (ps * _np);
	assert (_step < 1e6);	// To stay in acceptable range

	_k_step = _nc / ps;
	assert (_k_step < 1e6);	// To stay in acceptable range

	if (_biphase_flag)
	{
		_step *= 2;
	}
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
