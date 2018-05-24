/*****************************************************************************

        Voice.cpp
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

#include "fstb/fnc.h"
#include	"mfx/dsp/rspl/Voice.h"
#include	"mfx/dsp/rspl/InterpolatorInterface.h"
#include	"mfx/dsp/mix/Generic.h"
#include	"mfx/dsp/SplDataRetrievalInterface.h"

#include <algorithm>

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Voice::Voice ()
:	_interp_ptr (0)
,	_imp_len (1)
,	_group_dly (0)
,	_data_provider_ptr (0)
,	_ovrspl_l2 (0)
,	_sample_freq (44100)
,	_fade_time (0.005)
,	_tmp_buf_ptr (0)
,	_tmp_buf_len (0)
,	_room_for_lin_src (-1)
,	_in_fs (44100)
,	_in_len (0)
,	_nbr_chn (1)
,	_rate (1)
,	_rate_step (0)
,	_loop_mode (LoopMode_NONE)
,	_loop_pos_arr ()
,	_pos_int (0)
,	_pos_frac (0)
,	_pbk_dir (1)
,	_started_flag (false)
,	_stopped_flag (false)
,	_fade_rem_spl (0)
,	_fade_val (0)
,	_fade_step (0)
{
	// Nothing
}



void	Voice::set_interpolator (InterpolatorInterface &interp)
{
	assert (! is_active ());
	assert (&interp != 0);

	_interp_ptr = &interp;

	_interp_ptr->set_ovrspl_l2 (_ovrspl_l2);

	_imp_len = _interp_ptr->get_impulse_len ();
	_group_dly = _interp_ptr->get_group_delay ();
	assert (_group_dly.get_ceil () < _imp_len);
}



const InterpolatorInterface &	Voice::use_interpolator () const
{
	assert (_interp_ptr != 0);

	return (*_interp_ptr);
}



void	Voice::set_tmp_buf (float *buf_ptr, int len)
{
	assert (buf_ptr != 0);
	assert (len > 0);

	_tmp_buf_ptr = buf_ptr;
	_tmp_buf_len = len;

	if (is_active ())
	{
		update_tmp_buf_info ();
	}
}



// sample_freq = oversampled sample rate.
void	Voice::set_sample_freq (double sample_freq, int ovrspl_l2)
{
	assert (! is_playing ());
	assert (sample_freq > 0);
	assert (ovrspl_l2 >= 0);

	_sample_freq = sample_freq;
	_ovrspl_l2 = ovrspl_l2;

	_interp_ptr->set_ovrspl_l2 (_ovrspl_l2);
}



void	Voice::set_fade_duration (double fade_time)
{
	assert (! is_playing ());
	assert (fade_time > 0);

	_fade_time = fade_time;
}



bool	Voice::is_active () const
{
	return (_data_provider_ptr != 0);
}



void	Voice::activate (SplDataRetrievalInterface &data_provider, int64_t len, int nbr_chn, double sample_freq)
{
	assert (&data_provider != 0);
	assert (len >= 0);
	assert (nbr_chn > 0);
	assert (nbr_chn <= Cst::MAX_NBR_CHN);
	assert (sample_freq > 0);

	_data_provider_ptr = &data_provider;
	_in_fs = sample_freq;
	_in_len = len;
	_nbr_chn = nbr_chn;

	_loop_mode = LoopMode_NONE;
	_pos_int = 0;
	_pos_frac = 0;
	_pbk_dir = 1;
	compensate_pos_for_group_delay (_pos_int, _pos_frac, _pbk_dir);
	_started_flag = false;
	_stopped_flag = false;

	_fade_rem_spl = 0;

	update_tmp_buf_info ();
}



void	Voice::deactivate ()
{
	_data_provider_ptr = 0;
}



void	Voice::start (bool fade_flag)
{
	assert (is_active ());
	assert (! _started_flag);

	_interp_ptr->start (_nbr_chn);

	if (fade_flag)
	{
		_fade_val = 0;
		fade_to (1);
	}
	else
	{
		_fade_rem_spl = 0;
	}

	_started_flag = true;
}



void	Voice::stop (bool fade_flag)
{
	assert (is_active ());
	assert (_started_flag);
	assert (! _stopped_flag);

	if (fade_flag)
	{
		fade_to (0);
	}
	else
	{
		_fade_rem_spl = 0;
	}

	_stopped_flag = true;
}



void	Voice::set_rate (double rate, double rate_step)
{
	assert (is_active ());
	assert (rate > 0);

	_rate.set_val (rate);
	_rate_step.set_val (rate_step);
}



void	Voice::set_direction (bool backward_flag)
{
	assert (is_active ());

	compensate_pos_for_group_delay (_pos_int, _pos_frac, -_pbk_dir);

	_pbk_dir = (backward_flag) ? -1 : 1;

	compensate_pos_for_group_delay (_pos_int, _pos_frac, _pbk_dir);
}



void	Voice::set_loop_info (LoopMode loop_mode, int64_t pos, int64_t len)
{
	assert (is_active ());
	assert (loop_mode >= 0);
	assert (loop_mode < LoopMode_NBR_ELT);
	assert (loop_mode == LoopMode_NONE || pos >= 0);
	assert (loop_mode == LoopMode_NONE || len >= 2);
	assert (loop_mode == LoopMode_NONE || pos + len <= _in_len);

	_loop_mode = loop_mode;
	_loop_pos_arr [0] = pos;
	_loop_pos_arr [1] = pos + len;
}



void	Voice::set_playback_pos (int64_t pos_int, uint32_t pos_frac)
{
	compensate_pos_for_group_delay (pos_int, pos_frac, _pbk_dir);
	_pos_int  = pos_int;
	_pos_frac = pos_frac;
}



void	Voice::get_playback_pos (int64_t &pos_int, uint32_t &pos_frac) const
{
	assert (is_active ());
	assert (&pos_int != 0);
	assert (&pos_frac != 0);

	pos_int  = _pos_int;
	pos_frac = _pos_frac;
	compensate_pos_for_group_delay (pos_int, pos_frac, -_pbk_dir);
}



bool	Voice::is_running_backward () const
{
	return (_pbk_dir < 0);
}



void	Voice::process_block (float *out_ptr_arr [], int nbr_spl)
{
	assert (is_active ());
	assert (_interp_ptr != 0);
	assert (out_ptr_arr != 0);
	assert (out_ptr_arr [0] != 0);
	assert (nbr_spl > 0);
	assert (_rate.get_val_int64 () + _rate_step.get_val_int64 () * nbr_spl > 0);

	int            dest_pos = 0;

	check_finished (out_ptr_arr, dest_pos, nbr_spl);

	DataPtrArray	src_ptr_arr;
	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		src_ptr_arr [chn_cnt] = _tmp_buf_ptr + chn_cnt * _room_for_lin_src;
	}

	while (dest_pos < nbr_spl)
	{
		const int      rem_len = nbr_spl - dest_pos;

		// Computes the required buffer length. We don't take the loops into
		// account here, so it's just an upper limit for the next segment, which
		// may be shorter (we'll stop at the next jump)
		const fstb::FixedPoint	ext =
			InterpolatorInterface::integrate_rate (rem_len, _rate, _rate_step);

		// Computes the distance from this stop position, in source samples
		fstb::FixedPoint	dist (0, _pos_frac);
		dist += ext;

		// Number of required samples
		const int      nbr_req_spl = dist.get_int_val () + _imp_len;

		// Number of fetched samples is limited by the buffer size
		const int      nbr_fetch_spl = std::min (nbr_req_spl, _room_for_lin_src);

		// Linearise data
		linearise_sample (nbr_fetch_spl);

		// Interpolates
		fstb::FixedPoint	cur_pos_rel (0, _pos_frac);

		const int      dest_len = _interp_ptr->process_block (
			&out_ptr_arr [0],
			const_cast <const float **> (&src_ptr_arr [0]),
			dest_pos,
			cur_pos_rel,
			rem_len,
			0,
			nbr_fetch_spl,
			_rate,
			_rate_step
		);

		// Updates cursor and other things
		fstb::FixedPoint	stride =
			InterpolatorInterface::integrate_rate (dest_len, _rate, _rate_step);
		stride.add_frac (_pos_frac);

		_pos_frac = stride.get_frac_val ();
		const int      stride_int = stride.get_int_val ();
		advance_cursor_pos (_pos_int, _pbk_dir, stride_int);

		fstb::FixedPoint	rs (_rate_step);
		rs.mul_int (dest_len);
		_rate += rs;

		dest_pos += dest_len;

		// Check if we have finished
		check_finished (out_ptr_arr, dest_pos, nbr_spl);
	}

	// Handles fade in or out
	if (_fade_rem_spl > 0)
	{
		const int      fade_len = std::min (_fade_rem_spl, nbr_spl);
		const float    fade_end_val = _fade_val + _fade_step * fade_len;

		for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
		{
			mix::Generic::scale_1_vlr (
				&out_ptr_arr [chn_cnt] [0],
				fade_len,
				_fade_val,
				fade_end_val
			);
		}

		_fade_rem_spl -= fade_len;
		_fade_val = fade_end_val;

		if (_stopped_flag && fade_len < nbr_spl)
		{
			const int      silence_len = nbr_spl - fade_len;
			for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
			{
				mix::Generic::clear (
					&out_ptr_arr [chn_cnt] [fade_len],
					silence_len
				);
			}
		}
	}

	// Stop ?
	if (_stopped_flag && _fade_rem_spl <= 0)
	{
		deactivate ();
	}
}



void	Voice::conv_pos_flt_to_fix (int64_t &pos_int, uint32_t &pos_frac, double pos)
{
	assert (&pos_int != 0);
	assert (&pos_frac != 0);

	const float		uint_scale = 65536.0f * 65536.0f;

	pos_int = fstb::floor_int64 (pos);
	const int64_t	frac =
		fstb::floor_int64 ((pos - pos_int) * uint_scale);
	assert (frac >= 0);
	assert (double (frac) < double (uint_scale));	// double required here (> 32 bit mantissa)
	pos_frac = uint32_t (frac);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Voice::is_playing () const
{
	return (is_active () && _started_flag);
}



void	Voice::fade_to (float val)
{
	assert (val >= 0);
	assert (val <= 1);

	if (val == _fade_val)
	{
		_fade_rem_spl = 0;
	}

	else
	{
		const double	fade_time_spl = _fade_time * _sample_freq;
		_fade_rem_spl = fstb::conv_int_fast (fade_time_spl);
		_fade_rem_spl = std::max (_fade_rem_spl, 1);

		const float		dif = val - _fade_val;
		_fade_step = dif / _fade_rem_spl;
	}
}



void	Voice::update_tmp_buf_info ()
{
	assert (is_active ());

	_room_for_lin_src = _tmp_buf_len / _nbr_chn;
}



// nbr_spl = number of source samples to fetch
// If the loop cursors are fractionnal, the linearisation is approximative.
void	Voice::linearise_sample (int nbr_spl)
{
	assert (nbr_spl > 0);

	int            offset  = 0;
	int64_t        cur_pos = _pos_int;
	int            cur_dir = _pbk_dir;

	// If we start out of the sample, we generate silence
	if (cur_pos < 0 || cur_pos >= _in_len)
	{
		int            clear_len = nbr_spl;

		const int      dir_01   = (1 - cur_dir) >> 1;
		const int64_t  stop_pos = _in_len * dir_01;
		const int64_t  dif      = (stop_pos - cur_pos) * cur_dir;
		if (dif > 0)
		{
			clear_len = std::min (clear_len, int (dif));
		}

		for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
		{
			mix::Generic::clear (
				_tmp_buf_ptr + offset + chn_cnt * _room_for_lin_src,
				clear_len
			);
		}

		offset += clear_len;
		cur_pos += clear_len * cur_dir;
	}

	while (offset < nbr_spl)
	{
		int64_t        stop_pos;
		const bool     loop_flag = compute_next_stop (stop_pos, cur_pos, cur_dir);

		const int      rem_len   = nbr_spl - offset;
		const int      len       = collect_source_spl (
			offset,
			rem_len,
			cur_dir,
			cur_pos,
			stop_pos
		);

		offset += len;

		// If we generated less samples than the maximum, we have reached
		// the stop position (loop or file end).
		const int      clear_len = rem_len - len;
		if (clear_len > 0)
		{
			// Loop: updates position and playback direction
			if (loop_flag)
			{
				assert (_loop_mode != LoopMode_NONE);

				if (_loop_mode == LoopMode_PING_PONG)
				{
					cur_dir = -cur_dir;
				}

				const int		beg_index = (1 - cur_dir) >> 1;
				const int		dir_fix = compute_dir_fix (cur_dir);
				cur_pos = _loop_pos_arr [beg_index] + dir_fix;
			}

			// File end: clears remaining buffer
			else
			{
				for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
				{
					mix::Generic::clear (
						_tmp_buf_ptr + offset + chn_cnt * _room_for_lin_src,
						clear_len
					);
				}

				offset += clear_len;
			}
		}
	}
}



// true if it is the loop
bool	Voice::compute_next_stop (int64_t &stop_pos, int64_t cur_pos, int dir) const
{
	assert (&stop_pos != 0);
	assert (cur_pos >= 0);
	assert (cur_pos < _in_len);
	assert (dir == -1 || dir == +1);

	bool				loop_flag = false;

	if (dir > 0)
	{
		if (_loop_mode != LoopMode_NONE && cur_pos < _loop_pos_arr [1])
		{
			stop_pos = _loop_pos_arr [1];
			loop_flag = true;
		}
		else
		{
			stop_pos = _in_len;
		}
	}

	else
	{
		if (_loop_mode != LoopMode_NONE && _loop_pos_arr [0] - 1 < cur_pos)
		{
			stop_pos = _loop_pos_arr [0] - 1;
			loop_flag = true;
		}
		else
		{
			stop_pos = -1;
		}
	}

	return (loop_flag);
}



int	Voice::collect_source_spl (int offset, int max_len, int cur_dir, int64_t pos, int64_t stop_pos)
{
	assert (offset >= 0);
	assert (max_len > 0);
	assert (offset + max_len <= _room_for_lin_src);
	assert (cur_dir == +1 || cur_dir == -1);
	assert (pos >= 0);
	assert (pos < _in_len);
	assert (stop_pos >= 0 + compute_dir_fix (cur_dir));
	assert (stop_pos <= _in_len + compute_dir_fix (cur_dir));
	assert (cur_dir * (stop_pos - pos) > 0);

	int64_t        beg;
	int64_t        len;
	if (cur_dir > 0)
	{
		beg = pos;
		len = stop_pos - pos;
		len = std::min (len, int64_t (max_len));
	}
	else
	{
		beg = stop_pos + 1;
		len = pos - stop_pos;
		if (len > max_len)
		{
			beg += len - max_len;
			len = max_len;
		}
	}

	DataPtrArray	ptr_arr;
	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		ptr_arr [chn_cnt] = _tmp_buf_ptr + offset + chn_cnt * _room_for_lin_src;
	}

	const int      len_int = int (len);

	_data_provider_ptr->get_data (
		&ptr_arr [0],
		beg,
		len_int,
		(cur_dir < 0)
	);

	return (len_int);
}



// pos can be < 0 or >= _in_len if there is no loop to stop the cursor.
void	Voice::advance_cursor_pos (int64_t &pos, int &dir, int stride) const
{
	assert (&pos != 0);
	assert (&dir != 0);
	assert (dir == +1 || dir == -1);
	assert (stride >= 0);

	const int64_t  old_pos = pos;

	pos += stride * dir;

	if (_loop_mode != LoopMode_NONE)
	{
		// Checks if we have overtaken the loop end
		const int      end_index = (1 + dir) >> 1;
		const int      dir_fix   = compute_dir_fix (dir);
		const int64_t  loop_end  = _loop_pos_arr [end_index] + dir_fix;
		const int64_t  alg_dist_before_loop_pos = loop_end - old_pos;
		int64_t        alg_dist_after_loop_pos  = pos - loop_end;
		if (   alg_dist_before_loop_pos * dir >  0	// Not >= because if we start from the loop end point, we shouldn't loop.
		    && alg_dist_after_loop_pos  * dir >= 0)	// But if we finish on the loop end point, we start the next loop section.
		{
			const int      beg_index = 1 - end_index;
			const int64_t  loop_beg  = _loop_pos_arr [beg_index] + dir_fix;
			const int64_t  loop_len  = _loop_pos_arr [1] - _loop_pos_arr [0];
			assert (loop_len > 0);

			// Skips full loops
			int64_t        loop_wrap = loop_len;
			if (_loop_mode == LoopMode_PING_PONG)
			{
				loop_wrap <<= 1;
			}
			alg_dist_after_loop_pos %= loop_wrap;	// Here alg_dist_2 can be negative.

			if (_loop_mode == LoopMode_PING_PONG)
			{
				// Odd number of loops: doesn't change the direction
				if (alg_dist_after_loop_pos * dir >= loop_len)
				{
					const int64_t  loop_len_alg = loop_end - loop_beg;
					pos = loop_beg + alg_dist_after_loop_pos - loop_len_alg;
				}

				// Even number of loops: changes the direction
				else
				{
					dir = -dir;

					const int      new_dir_fix = compute_dir_fix (dir);
					const int64_t  loop_beg_new =
						_loop_pos_arr [end_index] + new_dir_fix;

					pos = loop_beg_new - alg_dist_after_loop_pos;
				}
			}
			else	// LoopMode_NORMAL
			{
				pos = loop_beg + alg_dist_after_loop_pos;
			}
		}
	}
}



void	Voice::check_finished (float *out_ptr_arr [], int &dest_pos, int nbr_spl)
{
	assert (&dest_pos != 0);
	assert (dest_pos >= 0);
	assert (nbr_spl > 0);
	assert (dest_pos <= nbr_spl);

	if (   (_pbk_dir < 0 && _pos_int < 0)
	    || (_pbk_dir > 0 && _pos_int >= _in_len))
	{
		const int      silence_len = nbr_spl - dest_pos;

		if (silence_len > 0)
		{
			for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
			{
				mix::Generic::clear (
					&out_ptr_arr [chn_cnt] [dest_pos],
					silence_len
				);
			}

			dest_pos += silence_len;
		}

		_stopped_flag = true;
		_fade_rem_spl = 0;
	}
}



// Compensation moves the cursor in the direction opposed to dir
void	Voice::compensate_pos_for_group_delay (int64_t &pos_int, uint32_t &pos_frac, int dir) const
{
	assert (&pos_int != 0);
	assert (&pos_frac != 0);
	assert (dir == +1 || dir == -1);

	fstb::FixedPoint	comp (_imp_len - 1, 0);
	comp -= _group_dly;
	comp.mul_int (-dir);
	comp.add_frac (pos_frac);

	pos_frac = comp.get_frac_val ();
	pos_int += comp.get_int_val ();
}



int	Voice::compute_dir_fix (int dir)
{
	assert (dir == +1 || dir == -1);

	// Backward: -1
	// Forward :  0
	const int		dir_fix = (dir - 1) >> 1;

	assert (dir_fix >= -1);
	assert (dir_fix <= 0);

	return (dir_fix);
}



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
