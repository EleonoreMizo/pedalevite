/*****************************************************************************

        DelayLineFracFir4.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelayLineFracFir4_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineFracFir4_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IM, typename AL>
DelayLineFracFir4 <IM, AL>::DelayLineFracFir4 (const AllocatorType &al)
:	_interp_fir ()
,	_buf (al)
,	_sample_freq (44100)
,	_real_period (0.01f)
,	_period (44100 * 0.01f - 1)
,	_max_time (0.020f)
,	_period_int (fstb::ceil_int (44100 * 0.01f - 1))
,	_interpolator_maker ()
{
/*#* LDS :
	It would be better to remove these lines from here to let the interpolator
	maker be completly configured by caller, and only call the update functions
	on set_sample_freq().
*/
	update_buffer ();
	update_period ();
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::set_sample_freq (float sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_buffer ();
	update_period ();
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::get_sample_freq () const
{
	return _sample_freq;
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::set_max_time (float per)
{
	assert (per > get_min_time ());

	_max_time = per;
	update_buffer ();
	if (_real_period > _max_time)
	{
		set_time (_max_time);
	}
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::get_max_time () const
{
	return _max_time;
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::get_min_time () const
{
	return 4.0f / _sample_freq;
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::set_time (float per)
{
	assert (per >= get_min_time ());
	assert (per <= get_max_time ());

	_real_period = per;
	update_period ();
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::get_time () const
{
	return _real_period;
}



template <typename IM, typename AL>
typename DelayLineFracFir4 <IM, AL>::InterpolatorMaker &	DelayLineFracFir4 <IM, AL>::use_interpolator_maker ()
{
	return _interpolater_maker;
}



template <typename IM, typename AL>
const typename DelayLineFracFir4 <IM, AL>::InterpolatorMaker &	DelayLineFracFir4 <IM, AL>::use_interpolator_maker () const
{
	return _interpolater_maker;
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::process_sample (float sample)
{
	const float    read_val = _buf.read_sample (&_interp_fir [0]);
	_buf.write_sample (sample);
	_buf.step_one_sample ();

	return read_val;
}



// Can work in-place
template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::process_block (float dest_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dest_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::process_block_mix (float dest_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dest_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] += process_sample (src_ptr [pos]);
		++pos;
	}
}



// Can work in-place
template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::process_block_vt (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_time)
{
	assert (dest_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (final_time >= get_min_time ());
	assert (final_time <= get_max_time ());

	const float    period_step = init_variable_time (nbr_spl, final_time);
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] = iterate_variable_time (period_step, src_ptr [pos]);
	}

	set_time (final_time);
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::process_block_vt_mix (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_time)
{
	assert (dest_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (final_time >= get_min_time ());
	assert (final_time <= get_max_time ());

	const float    period_step = init_variable_time (nbr_spl, final_time);
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] += iterate_variable_time (period_step, src_ptr [pos]);
	}

	set_time (final_time);
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::clear_buffers ()
{
	_buf.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::update_buffer ()
{
	const int      max_time_spl     = fstb::ceil_int (_max_time * _sample_freq);
	const int      additional_delay = _interpolator_maker.get_delay ();
	_buf.set_max_time (max_time_spl + additional_delay);
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::update_period ()
{
	_period = _real_period * _sample_freq + _interpolator_maker.get_delay ();
	update_interpolator ();
}



template <typename IM, typename AL>
void	DelayLineFracFir4 <IM, AL>::update_interpolator ()
{
	assert (_interpolator_maker.get_length () == 4);

	_period_int = fstb::ceil_int (_period);
	_buf.set_time (_period_int);
	const float    d = _period_int - _period;
	_interpolator_maker.make_interpolator (&_interp_fir [0], d);
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::init_variable_time (int nbr_spl, float final_time)
{
	assert (nbr_spl > 0);
	assert (final_time >= get_min_time ());
	assert (final_time <= get_max_time ());

	const float    final_period =
		final_time * _sample_freq + _interpolator_maker.get_delay ();
	const float    inv_nbr_spl  = 1.0f / nbr_spl;
	const float    period_step  = (final_period - _period) * inv_nbr_spl;

	return period_step;
}



template <typename IM, typename AL>
float	DelayLineFracFir4 <IM, AL>::iterate_variable_time (float period_step, float sample)
{
	_period += period_step;
	update_interpolator ();

	return process_sample (sample);
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelayLineFracFir4_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
