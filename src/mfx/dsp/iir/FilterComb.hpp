/*****************************************************************************

        FilterComb.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_FilterComb_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_FilterComb_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IM, typename FP, typename OP, typename AL>
FilterComb <IM, FP, OP, AL>::FilterComb (const AllocatorType &al)
:	_interp_base ()
,	_interp_final ()
,	_buf (al)
,	_sample_freq (44100)
,	_freq (441)
,	_period (44100 / 441.0f - 1)
,	_min_freq (50)
,	_feedback (0.5f)
,	_period_int (fstb::ceil_int (44100 / 441.0f) - 1)
,	_feedback_proc ()
,	_output_proc ()
,	_interpolator_maker ()
{
/*#* LDS :
	It would be better to remove these lines from here to let the interpolator
	maker be completly configured by the caller, and only call the update
	functions on set_sample_freq().
*/
	update_buffer ();
	update_interpolator ();
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::set_sample_freq (float sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = sample_freq;
	update_buffer ();
	update_interpolator ();
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::get_sample_freq () const noexcept
{
	return _sample_freq;
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::set_min_freq (float freq)
{
	assert (freq > 0);
	assert (freq < get_max_freq ());

	_min_freq = freq;
	update_buffer ();
	if (_freq < _min_freq)
	{
		set_freq (_min_freq);
	}
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::get_min_freq () const noexcept
{
	return _min_freq;
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::get_max_freq () const noexcept
{
	return _sample_freq * 0.25f;
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::set_freq (float freq) noexcept
{
	assert (freq >= get_min_freq ());
	assert (freq <= get_max_freq ());

	_freq = freq;
	update_interpolator ();
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::get_freq () const noexcept
{
	return _freq;
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::set_feedback (float feedback) noexcept
{
	assert (fabsf (feedback) <= 1);

	_feedback = feedback;
	update_feedback ();
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::get_feedback () const noexcept
{
	return _feedback;
}



template <typename IM, typename FP, typename OP, typename AL>
typename FilterComb <IM, FP, OP, AL>::FeedbackProcessor &	FilterComb <IM, FP, OP, AL>::use_feedback_proc () noexcept
{
	return _feedback_proc;
}



template <typename IM, typename FP, typename OP, typename AL>
const typename FilterComb <IM, FP, OP, AL>::FeedbackProcessor &	FilterComb <IM, FP, OP, AL>::use_feedback_proc () const noexcept
{
	return _feedback_proc;
}



template <typename IM, typename FP, typename OP, typename AL>
typename FilterComb <IM, FP, OP, AL>::OutputProcessor &	FilterComb <IM, FP, OP, AL>::use_output_proc () noexcept
{
	return _output_proc;
}



template <typename IM, typename FP, typename OP, typename AL>
const typename FilterComb <IM, FP, OP, AL>::OutputProcessor &	FilterComb <IM, FP, OP, AL>::use_output_proc () const noexcept
{
	return _output_proc;
}



template <typename IM, typename FP, typename OP, typename AL>
typename FilterComb <IM, FP, OP, AL>::InterpolatorMaker &	FilterComb <IM, FP, OP, AL>::use_interpolator_maker () noexcept
{
	return _interpolator_maker;
}



template <typename IM, typename FP, typename OP, typename AL>
const typename FilterComb <IM, FP, OP, AL>::InterpolatorMaker &	FilterComb <IM, FP, OP, AL>::use_interpolator_maker () const noexcept
{
	return _interpolator_maker;
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::process_sample (float sample) noexcept
{
	const float    read_val = read_sample_with_feedback ();
	const float    fdbk_val = _feedback_proc.process_sample (read_val);
	const float    out_val  = _output_proc.process_sample (sample + fdbk_val);

	return write_in_delay_line (out_val);
}



// Can work in-place
template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::process_block (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dest_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::process_block_mix (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dest_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] += process_sample (src_ptr [pos]);
	}
}



// Can work in-place
template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::process_block_vff (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_freq, float final_feedback) noexcept
{
	assert (dest_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (final_freq >= get_min_freq ());
	assert (final_freq <= get_max_freq ());
	assert (fabsf (final_feedback) <= 1);

	VariableFreqFeedbackData   data;
	init_variable_freq_feedback (data, nbr_spl, final_freq, final_feedback);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] = iterate_variable_freq_feedback (data, src_ptr [pos]);
	}

	set_freq (final_freq);
	set_feedback (final_feedback);
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::process_block_vff_mix (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_freq, float final_feedback) noexcept
{
	assert (dest_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (final_freq >= get_min_freq ());
	assert (final_freq <= get_max_freq ());
	assert (fabsf (final_feedback) <= 1);

	VariableFreqFeedbackData   data;
	init_variable_freq_feedback (data, nbr_spl, final_freq, final_feedback);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dest_ptr [pos] += iterate_variable_freq_feedback (data, src_ptr [pos]);
	}

	set_freq (final_freq);
	set_feedback (final_feedback);
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::clear_buffers () noexcept
{
	_buf.clear_buffers ();
}



/*
==============================================================================
Name: clear_buffers_fast
Description:
	Clears only the buffer part for the current period (not the whole buffer)
Throws: Nothing
==============================================================================
*/

template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::clear_buffers_fast () noexcept
{
	_buf.clear_buffers_fast ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::update_buffer ()
{
	const int      max_time_spl     = fstb::ceil_int (_sample_freq / _min_freq);
	const int      additional_delay = _interpolator_maker.get_delay ();
	const int      security         = 1; // Because of rounding errors accumulating in iterate_variable_freq_feedback()
	_buf.set_max_time (max_time_spl + additional_delay + security);
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::update_interpolator () noexcept
{
	_period = _sample_freq / _freq + float (_interpolator_maker.get_delay ());
	_period_int = fstb::ceil_int (_period);
	_buf.set_time (_period_int);
	const float    d = float (_period_int) - _period;
	update_interpolator_base_only (d);

	update_feedback ();
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::update_interpolator_base_only (float d) noexcept
{
	assert (_interpolator_maker.get_length () == 4);

	_interpolator_maker.make_interpolator (&_interp_base [0], d);
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::update_feedback () noexcept
{
	_interp_final [0] = _interp_base [0] * _feedback;
	_interp_final [1] = _interp_base [1] * _feedback;
	_interp_final [2] = _interp_base [2] * _feedback;
	_interp_final [3] = _interp_base [3] * _feedback;
	assert (_interp_final.size () == 4);
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::read_sample_without_feedback () const noexcept
{
	return _buf.read_sample (&_interp_base [0]);
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::read_sample_with_feedback () const noexcept
{
	return _buf.read_sample (&_interp_final [0]);
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::write_in_delay_line (float sample) noexcept
{
	_buf.write_sample (sample);
	_buf.step_one_sample ();

	return sample;
}



template <typename IM, typename FP, typename OP, typename AL>
void	FilterComb <IM, FP, OP, AL>::init_variable_freq_feedback (VariableFreqFeedbackData &data, int nbr_spl, float final_freq, float final_feedback) noexcept
{
	assert (nbr_spl > 0);
	assert (final_freq >= get_min_freq ());
	assert (final_freq <= get_max_freq ());
	assert (fabsf (final_feedback) <= 1);

	const float    final_period =
		_sample_freq / final_freq + float (_interpolator_maker.get_delay ());
	const float    inv_nbr_spl  = 1.0f / float (nbr_spl);
	data._cur_feedback  = _feedback;
	data._cur_period    = _period;
	data._feedback_step = (final_feedback - _feedback) * inv_nbr_spl;
	data._period_step   = (final_period - _period) * inv_nbr_spl;
}



template <typename IM, typename FP, typename OP, typename AL>
float	FilterComb <IM, FP, OP, AL>::iterate_variable_freq_feedback (VariableFreqFeedbackData &data, float sample) noexcept
{
	_period_int = fstb::ceil_int (data._cur_period);
	_buf.set_time (_period_int);
	const float    d        = float (_period_int) - data._cur_period;
	update_interpolator_base_only (d);
	const float    read_val = read_sample_without_feedback () * data._cur_feedback;
	const float    fdbk_val = _feedback_proc.process_sample (read_val);
	const float    out_val  = _output_proc.process_sample (sample + fdbk_val);

	data._cur_period   += data._period_step;
	data._cur_feedback += data._feedback_step;

	return write_in_delay_line (out_val);
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_FilterComb_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
