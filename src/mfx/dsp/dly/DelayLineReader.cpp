/*****************************************************************************

        DelayLineReader.cpp
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

#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dly/DelayLineReader.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Mandatory call
void	DelayLineReader::set_tmp_buf (float *buf_ptr, int len)
{
	assert (buf_ptr != 0);
	assert (len > 0);

	_tmp_buf_ptr = buf_ptr;
	_tmp_buf_len = len;
}



float *	DelayLineReader::get_tmp_buf_ptr () const
{
	assert (_tmp_buf_ptr != 0);

	return (_tmp_buf_ptr);
}



int	DelayLineReader::get_tmp_buf_len () const
{
	assert (_tmp_buf_ptr != 0);

	return (_tmp_buf_len);
}



// Mandatory call
void	DelayLineReader::set_delay_line (DelayLine &delay_line)
{
	assert (&delay_line != 0);

	_delay_line_ptr = &delay_line;
}



// Rates are related to input data (not oversampled)
void	DelayLineReader::set_resampling_range (double rate_inf, double rate_sup)
{
	assert (rate_inf < rate_sup);

	_rate_inf = rate_inf;
	_rate_sup = rate_sup;
}



// Set shape_ptr to 0 to remove the shape and use the default (linear).
void	DelayLineReader::set_crossfade (int nbr_spl, const float shape_ptr [])
{
	assert (nbr_spl > 0);

	_xfade_dur       = nbr_spl;
	_xfade_shape_ptr = shape_ptr;
}



bool	DelayLineReader::is_ready () const
{
	return (   _delay_line_ptr != 0
	        && _tmp_buf_ptr != 0);
}



// transition_time is a time in samples, related to output. Therefore it is
// related to oversampled data, if oversampling has been set.
void	DelayLineReader::set_delay_time (double delay_time, int transition_time)
{
	assert (is_ready ());
	assert (delay_time >= _delay_line_ptr->get_min_delay_time ());
	assert (delay_time <= _delay_line_ptr->get_max_delay_time ());
	assert (transition_time >= 0);

	// We're in the middle of a crossfading transition ? Let's program another one.
	if (   _trans_pos > 0	// 0 means it was set up but not started yet
	    && _xfade_flag)
	{
		_time_prog  = delay_time;
		_trans_prog = transition_time;
	}

	// Transition not started, starts one.
	else
	{
		setup_immediate_transition (delay_time, transition_time);
	}
}



// src_pos refers to the position within the virtual block bounded by two
// consecutive calls to DelayLine::push_data().
// Read data is oversampled, if delay line has been oversampled.
void	DelayLineReader::read_data (float dest_ptr [], int nbr_spl, int src_pos)
{
	assert (is_ready ());
	assert (dest_ptr != 0);
	assert (nbr_spl > 0);
	assert (src_pos >= 0);

	int            dest_pos = 0;
	do
	{
		int            work_len = nbr_spl - dest_pos;
		if (is_time_ramping ())
		{
			const int      rem_len = _trans_dur - _trans_pos;
			work_len = std::min (work_len, rem_len);

				const double	lerp_pos_end =
					double (_trans_pos + work_len) / double (_trans_dur);

			// Crossfade
			if (_xfade_flag)
			{
				work_len = std::min (work_len, _tmp_buf_len);
				apply_crossfade (
					dest_ptr + dest_pos,
					work_len,
					lerp_pos_end,
					src_pos + dest_pos
				);
			}

			// Smooth ramp
			else
			{
				const double   time_end =
					_time_beg + (_time_end - _time_beg) * lerp_pos_end;

				_delay_line_ptr->read_line (
					dest_ptr + dest_pos,
					work_len,
					_time_cur,
					time_end,
					src_pos + dest_pos
				);

				_time_cur = time_end;
			}

			_trans_pos += work_len;

			// Transition finished ?
			if (_trans_pos >= _trans_dur)
			{
				_trans_pos = -1;
				_time_cur = _time_end;
				_time_beg = _time_end;
			}
		}

		// Steady state
		else
		{
			_delay_line_ptr->read_line (
				dest_ptr + dest_pos,
				work_len,
				_time_cur,
				_time_cur,
				src_pos + dest_pos
			);
		}

		dest_pos += work_len;

		if (is_time_change_programmed ())
		{
			_trans_prog -= work_len;
			_trans_prog  = std::max (_trans_prog, 0);

			if (! is_time_ramping ())
			{
				setup_immediate_transition (_time_prog, _trans_prog);
			}
		}
	}
	while (dest_pos < nbr_spl);
}



bool	DelayLineReader::is_time_ramping () const
{
	assert (is_ready ());

	return (_trans_pos >= 0);
}



bool	DelayLineReader::is_time_change_programmed () const
{
	assert (is_ready ());

	return (_trans_prog >= 0);
}



void	DelayLineReader::clear_buffers ()
{
	assert (is_ready ());

	if (is_time_change_programmed ())
	{
		_time_end = _time_prog;
	}
	_time_cur = _time_end;
	_time_beg = _time_end;
	_trans_prog = -1;
	_trans_pos  = -1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DelayLineReader::setup_immediate_transition (double delay_time, int transition_time)
{
	assert (transition_time >= 0);

	_time_beg   = _time_cur;
	_time_end   = delay_time;
	_trans_dur  = transition_time;
	_trans_pos  = 0;
	_trans_prog = -1;	// Clears any pending transition

	const int      ovrspl_l2 = _delay_line_ptr->get_ovrspl_l2 ();
	const int      ttime_normal_rate = transition_time >> ovrspl_l2;
	if (ttime_normal_rate == 0)
	{
		_xfade_flag = true;
	}
	else
	{
		const double   sample_freq = _delay_line_ptr->get_sample_freq ();
		const double   rate =
			1 + (_time_beg - _time_end) * sample_freq / ttime_normal_rate;

		_xfade_flag = (rate < _rate_inf || rate > _rate_sup);
	}

	if (_xfade_flag)
	{
		_trans_dur = _xfade_dur;
	}
}



void	DelayLineReader::apply_crossfade (float dest_ptr [], int nbr_spl, double lerp_pos_end, int src_pos)
{
	assert (_xfade_flag);
	assert (_trans_pos + nbr_spl <= _trans_dur);
	assert (_trans_dur == _xfade_dur);

	// Default: linear
	if (_xfade_shape_ptr == 0)
	{
		// The temporary buffer contains the old delay (fade out)
		_delay_line_ptr->read_line (
			_tmp_buf_ptr,
			nbr_spl,
			_time_beg,
			_time_beg,
			src_pos
		);

		// The destination buffer contains the new delay (fade in)
		_delay_line_ptr->read_line (
			dest_ptr,
			nbr_spl,
			_time_end,
			_time_end,
			src_pos
		);

		// Crossfade and mix
		const double   lerp_pos_beg = double (_trans_pos) / double (_trans_dur);

		mix::Generic::scale_1_vlr (
			dest_ptr,
			nbr_spl,
			float (lerp_pos_beg),
			float (lerp_pos_end)
		);
		mix::Generic::mix_1_1_vlr (
			dest_ptr,
			_tmp_buf_ptr,
			nbr_spl,
			float (1 - lerp_pos_beg),
			float (1 - lerp_pos_end)
		);
	}

	// Custom crossfade
	else
	{
		const float *  shape_ptr = _xfade_shape_ptr + _trans_pos;

		// Fade out part (temporary buffer)
		mix::Generic::linop_cst_1_1 (dest_ptr, shape_ptr, nbr_spl, -1, 1);
		_delay_line_ptr->read_line (
			_tmp_buf_ptr,
			nbr_spl,
			_time_beg,
			_time_beg,
			src_pos
		);
		mix::Generic::mult_ip_1_1 (_tmp_buf_ptr, dest_ptr, nbr_spl);

		// Fade in part (destination buffer)
		_delay_line_ptr->read_line (
			dest_ptr,
			nbr_spl,
			_time_end,
			_time_end,
			src_pos
		);
		mix::Generic::mult_ip_1_1 (dest_ptr, shape_ptr, nbr_spl);

		// Mix
		mix::Generic::mix_1_1 (dest_ptr, _tmp_buf_ptr, nbr_spl);
	}
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
