/*****************************************************************************

        OscNoiseFlt.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_lfo_OscNoiseFlt_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscNoiseFlt_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <algorithm>

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



template <int ORDER>
OscNoiseFlt <ORDER>::OscNoiseFlt ()
:	_phase_gen ()
,	_rnd_gen ()
,	_rnd_dist (-1, 1)
,	_amp (0)
,	_cur_val_arr ()
,	_filter_coef (0)
,	_noise_pos (0)
,	_noise_res (64)
{
	update_period ();
	clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_sample_freq (double sample_freq)
{
	// 44.1kHz-48kHz -> 64
	const int      nr_l2  = fstb::round_int (log2 (sample_freq) - 9.5);
	_noise_res = 1 << std::max (nr_l2, 0);

	_phase_gen.set_sample_freq (sample_freq);
	update_period ();
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_period (double per)
{
	_phase_gen.set_period (per);
	update_period ();
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_phase (double phase)
{
	_phase_gen.set_phase (phase);
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_chaos (double /*chaos*/)
{
	// Nothing
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_phase_dist (double /*dist*/)
{
	// Nothing
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_phase_dist_offset (double /*ofs*/)
{
	// Nothing
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_sign (bool /*inv_flag*/)
{
	// Nothing
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_polarity (bool /*unipolar_flag*/)
{
	// Nothing
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_set_variation (int /*param*/, double /*val*/)
{
	// Nothing
}



template <int ORDER>
bool	OscNoiseFlt <ORDER>::do_is_using_variation (int /*param*/) const
{
	return false;
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_tick (int nbr_spl)
{
	_phase_gen.tick (nbr_spl);

	_noise_pos += nbr_spl;
	while (_noise_pos >= _noise_res)
	{
		_noise_pos -= _noise_res;
		gen_new_val ();
	}
}



template <int ORDER>
double	OscNoiseFlt <ORDER>::do_get_val () const
{
	return (_cur_val_arr [ORDER]);
}



template <int ORDER>
double	OscNoiseFlt <ORDER>::do_get_phase () const
{
	return (_phase_gen.get_phase ());
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::do_clear_buffers ()
{
	for (int state_index = 0; state_index <= ORDER; ++state_index)
	{
		_cur_val_arr [state_index] = 0;
	}

	_noise_pos = 0;
	_rnd_gen.seed ();
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int ORDER>
void	OscNoiseFlt <ORDER>::update_period ()
{
	const double   pseudo_fs =
		_phase_gen.get_sample_freq () * (1.0 / _noise_res);
	const double   pseudo_f0 = std::min (
		double (1.0f / _phase_gen.get_period ()),
		pseudo_fs * 0.4375f
	);

	const double   freq_norm = pseudo_f0 / pseudo_fs;
//	const double   omega     = freq_norm * (fstb::PI * 2);
//	const double   k         = 2 - cos (omega);
	const float    k         =
		2 - fstb::Approx::sin_nick_2pi (0.25f - float (freq_norm));
	_filter_coef = 1 + sqrt (k*k - 1) - k;

	_amp = sqrt (pseudo_fs / (pseudo_f0 * 2));
}



template <int ORDER>
void	OscNoiseFlt <ORDER>::gen_new_val ()
{
	double         val = _rnd_dist (_rnd_gen) * _amp;
	_cur_val_arr [0] = val;

	for (int o = 1; o <= ORDER; ++o)
	{
		const double   val_old = _cur_val_arr [o];
		val = val_old + _filter_coef * (val - val_old);
		_cur_val_arr [o] = val;
	}
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_lfo_OscNoiseFlt_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
