/*****************************************************************************

        PhaseGen.hpp
        Author: Laurent de Soras, 2010

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_lfo_PhaseGen_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_PhaseGen_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PhaseGen::PhaseGen ()
:	_phase (0)
,	_step (0)
,	_sample_freq (44100)
,	_period (1)
{
	update_period ();
}



void	PhaseGen::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_period ();
}



void	PhaseGen::set_period (double per)
{
	assert (per > 0);

	_period = per;
	update_period ();
}



void	PhaseGen::set_phase (double phase)
{
	assert (phase >= 0);
	assert (phase < 1);

	_phase = phase;
}



void	PhaseGen::tick (int nbr_spl)
{
	assert (nbr_spl > 0);

	_phase += _step * nbr_spl;
	if (_phase >= 1)
	{
		const int		phase_int = fstb::floor_int (_phase);
		_phase -= phase_int;
	}
}



double	PhaseGen::get_sample_freq () const
{
	return (_sample_freq);
}



double	PhaseGen::get_period () const
{
	return (_period);
}



double	PhaseGen::get_phase () const
{
	return (_phase);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PhaseGen::update_period ()
{
	_step = 1.0 / (_period * _sample_freq);
	assert (_step < 1e6);	// To stay in acceptable range
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_lfo_PhaseGen_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
