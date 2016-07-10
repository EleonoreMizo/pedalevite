/*****************************************************************************

        OscInterface.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_lfo_OscInterface_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscInterface_CODEHEADER_INCLUDED



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



void	OscInterface::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	do_set_sample_freq (sample_freq);
}



void	OscInterface::set_period (double per)
{
	assert (per > 0);

	do_set_period (per);
}



void	OscInterface::set_phase (double phase)
{
	assert (phase >= 0);
	assert (phase < 1);

	do_set_phase (phase);
}



void	OscInterface::set_chaos (double chaos)
{
	assert (chaos >= 0);
	assert (chaos <= 1);

	do_set_chaos (chaos);
}



void	OscInterface::set_phase_dist (double dist)
{
	assert (dist >= 0);
	assert (dist <= 1);

	do_set_phase_dist (dist);
}



void	OscInterface::set_sign (bool inv_flag)
{
	do_set_sign (inv_flag);
}



void	OscInterface::set_polarity (bool unipolar_flag)
{
	do_set_polarity (unipolar_flag);
}



void	OscInterface::set_variation (int param, double val)
{
	assert (param >= 0);
	assert (param < Variation_NBR_ELT);
	assert (val >= 0);
	assert (val <= 1);

	do_set_variation (param, val);
}



bool	OscInterface::is_using_variation (int param) const
{
	assert (param >= 0);
	assert (param < Variation_NBR_ELT);

	return (do_is_using_variation (param));
}



void	OscInterface::tick (long nbr_spl)
{
	assert (nbr_spl > 0);

	do_tick (nbr_spl);
}



double	OscInterface::get_val () const
{
	const double	val = do_get_val ();
	assert (val > -1000);	// Assume it is in a reasonable range
	assert (val < +1000);

	return (val);
}



double	OscInterface::get_phase () const
{
	const double	phase = do_get_phase ();
	assert (phase >= 0);
	assert (phase < 1);

	return (phase);
}



void	OscInterface::clear_buffers ()
{
	do_clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_lfo_OscInterface_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
