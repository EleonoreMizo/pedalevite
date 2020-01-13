/*****************************************************************************

        DelayLineReaderPitch.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelayLineReaderPitch_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineReaderPitch_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/dly/DelayLineReadInterface.h"

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
template <typename TC>
void	DelayLineReaderPitch <TC>::set_tmp_buf (float *buf_ptr, int len)
{
	assert (buf_ptr != nullptr);
	assert (len > 0);

	_tmp_buf_ptr = buf_ptr;
	_tmp_buf_len = len;
}



template <typename TC>
float *	DelayLineReaderPitch <TC>::get_tmp_buf_ptr () const
{
	assert (_tmp_buf_ptr != 0);

	return _tmp_buf_ptr;
}



template <typename TC>
int	DelayLineReaderPitch <TC>::get_tmp_buf_len () const
{
	assert (_tmp_buf_ptr != 0);

	return _tmp_buf_len;
}



// Mandatory call
template <typename TC>
void	DelayLineReaderPitch <TC>::set_delay_line (const DelayLineReadInterface &delay_line)
{
	_delay_line_ptr = &delay_line;
	_dly_min = -1;
	_dly_max = -1;
}



// Rates are related to input data (not oversampled)
template <typename TC>
void	DelayLineReaderPitch <TC>::set_resampling_range (TC rate_inf, TC rate_sup)
{
	assert (rate_inf < rate_sup);

	_rate_inf = rate_inf;
	_rate_sup = rate_sup;
}



// Set shape_ptr to 0 to remove the shape and use the default (linear).
template <typename TC>
void	DelayLineReaderPitch <TC>::set_crossfade_normal (int nbr_spl, const float shape_ptr [])
{
	assert (nbr_spl > 0);

	// If a crossfading is currently running, tries to arrange it.
	if (_xfade_pos >= 0 && ! _ps_flag)
	{
		_xfade_pos = fstb::round_int (_xfade_pos * nbr_spl / _xfade_dn);
	}
	_xfade_dn     = nbr_spl;
	_xfade_sn_ptr = shape_ptr;
}



// Set shape_ptr to 0 to remove the shape and use the default (linear).
template <typename TC>
void	DelayLineReaderPitch <TC>::set_crossfade_pitchshift (int nbr_spl, const float shape_ptr [])
{
	assert (nbr_spl > 0);

	// If a crossfading is currently running, tries to arrange it.
	if (_xfade_pos >= 0 && _ps_flag)
	{
		_xfade_pos = fstb::round_int (_xfade_pos * nbr_spl / _xfade_dp);
	}
	_xfade_dp     = nbr_spl;
	_xfade_sp_ptr = shape_ptr;
}



template <typename TC>
bool	DelayLineReaderPitch <TC>::is_ready () const
{
	return (   _delay_line_ptr != nullptr
	        && _tmp_buf_ptr    != nullptr);
}



// transition_time is a time in samples, related to output. Therefore it is
// related to oversampled data, if oversampling has been set.
template <typename TC>
void	DelayLineReaderPitch <TC>::set_delay_time (TC delay_time, int transition_time)
{
	assert (is_ready ());
	assert (delay_time >= _delay_line_ptr->get_min_delay_time ());
	assert (delay_time <= _delay_line_ptr->get_max_delay_time ());
	assert (transition_time >= 0);

	_prog_time  = delay_time;
	_prog_trans = transition_time;
}



template <typename TC>
void	DelayLineReaderPitch <TC>::set_grain_pitch (float ratio)
{
	assert (is_ready ());

	const bool     ps_flag_old = _ps_flag;
	_rate_grain = ratio;
	_ps_flag    = (! fstb::is_eq (ratio, 1.0f, 1e-3f));

	if (_xfade_pos >= 0)
	{
		if (_ps_flag && ! ps_flag_old)
		{
			_xfade_pos = fstb::floor_int (_xfade_pos * _xfade_dp / _xfade_dn);
		}
		else if (! _ps_flag && ps_flag_old)
		{
			_xfade_pos = fstb::floor_int (_xfade_pos * _xfade_dn / _xfade_dp);
		}
		assert (_xfade_pos < get_xfade_len ());
	}

	if (_ps_flag && ! is_time_change_programmed ())
	{
		_prog_time  = _time_cur;
		_prog_trans = 0;
	}
}



// src_pos refers to the position within the virtual block bounded by two
// consecutive calls to DelayLine::push_block(). It can be negative if
// push_block() is called before reading the line.
// Read data is oversampled, if delay line has been oversampled.
template <typename TC>
void	DelayLineReaderPitch <TC>::read_data (float dst_ptr [], int nbr_spl, int src_pos)
{
	assert (is_ready ());
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_dly_min < 0)
	{
		_dly_min = TC (_delay_line_ptr->get_min_delay_time ());
		_dly_max = TC (_delay_line_ptr->get_max_delay_time ());
	}

	check_and_start_transition ();

	int            dst_pos = 0;
	do
	{
		int            work_len = nbr_spl - dst_pos;
		Grain &        g_cur = _grain_arr [    _grain_cur];
		Grain &        g_old = _grain_arr [1 - _grain_cur];

		work_len = g_cur.clip_block_len (work_len);
		const bool     xfade_flag = (_xfade_pos >= 0);
		int            xfade_dur  = 0;
		const float *  xfade_shape_ptr = nullptr;
		if (xfade_flag)
		{
			xfade_dur       = get_xfade_len ();
			xfade_shape_ptr = use_xfase_shape ();
			work_len        = std::min (work_len, xfade_dur - _xfade_pos);
			work_len        = std::min (work_len, _tmp_buf_len);
			work_len        = g_old.clip_block_len (work_len);
		}

		process_grain (
			g_cur,
			dst_ptr + dst_pos,
			src_pos + dst_pos,
			work_len
		);

		if (xfade_flag)
		{
			process_grain (
				g_old,
				_tmp_buf_ptr,
				src_pos + dst_pos,
				work_len
			);

			const float    inv_dur  = 1.0f / float (xfade_dur);
			const float    lerp_beg = float (_xfade_pos           ) * inv_dur;
			const float    lerp_end = float (_xfade_pos + work_len) * inv_dur;

			if (xfade_shape_ptr == nullptr)
			{
				// Default: linear
				mix::Generic::scale_1_vlr (
					dst_ptr + dst_pos,
					work_len,
					lerp_beg,
					lerp_end
				);
				mix::Generic::mix_1_1_vlr (
					dst_ptr + dst_pos,
					_tmp_buf_ptr,
					work_len,
					1 - lerp_beg,
					1 - lerp_end
				);
			}
			else
			{
				// Custom shape
				mix::Generic::copy_xfade_3_1 (
					dst_ptr + dst_pos,
					_tmp_buf_ptr,
					dst_ptr + dst_pos,
					xfade_shape_ptr + _xfade_pos,
					work_len
				);
			}

			_xfade_pos += work_len;
			if (_xfade_pos >= xfade_dur)
			{
				_xfade_pos = -1;
			}
		}

		// Updates the time position of the programmed delay change
		if (is_time_change_programmed ())
		{
			_prog_trans -= work_len;
			_prog_trans  = std::max (_prog_trans, 0);
		}

		check_and_start_transition ();

		dst_pos += work_len;
	}
	while (dst_pos < nbr_spl);
}



template <typename TC>
bool	DelayLineReaderPitch <TC>::is_time_change_programmed () const
{
	assert (is_ready ());

	return (_prog_trans >= 0);
}



template <typename TC>
void	DelayLineReaderPitch <TC>::clear_buffers ()
{
	assert (is_ready ());

	if (is_time_change_programmed ())
	{
		_time_cur = _prog_time;
	}
	_prog_trans  = -1;
	_xfade_pos   = -1;
	_grain_cur   = 0;

	Grain &        g = _grain_arr [_grain_cur];
	g._dly_cur   = _time_cur;
	g._dly_tgt   = _time_cur;
	g._dly_stp   = 0;
	g._trans_pos = -1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename TC>
bool	DelayLineReaderPitch <TC>::Grain::is_ramping () const
{
	return (_trans_len > 0);
}



template <typename TC>
int	DelayLineReaderPitch <TC>::Grain::clip_block_len (int len) const
{
	assert (len > 0);

	if (is_ramping ())
	{
		assert (_trans_len > _trans_pos);
		len = std::min (len, _trans_len - _trans_pos);
	}

	return len;
}



template <typename TC>
void	DelayLineReaderPitch <TC>::check_and_start_transition ()
{
	// Check if we need a transition
	bool           req_trans_flag = _ps_flag;
	bool           req_xfade_flag = _ps_flag;
	TC             dly            = _time_cur;
	const int      xfade_dur      = get_xfade_len ();
	int            nbr_spl        = xfade_dur;
	if (is_time_change_programmed ())
	{
		req_trans_flag = true;
		dly            = _prog_time;
		nbr_spl        = _prog_trans;
	}

	if (req_trans_flag)
	{
		const bool     time_chg_flag       = (_time_cur != dly);
		const int      ovrspl_l2           = _delay_line_ptr->get_ovrspl_l2 ();
		const int      nbr_spl_normal_rate = nbr_spl >> ovrspl_l2;
		const TC       sample_freq         =
			TC (_delay_line_ptr->get_sample_freq ());
		if (nbr_spl_normal_rate == 0)
		{
			req_xfade_flag |= time_chg_flag;
		}
		else
		{
			const TC       rate =
				1 + (_time_cur - dly) * sample_freq / nbr_spl_normal_rate;

			req_xfade_flag |= (rate < _rate_inf || rate > _rate_sup);
		}

		// With crossfading
		if (req_xfade_flag)
		{
			if (_xfade_pos < 0)
			{
				// Grain switch
				const int      pos_other = 1 - _grain_cur;
				Grain &        g_old     = _grain_arr [_grain_cur];
				Grain &        g_new     = _grain_arr [pos_other ];
				_grain_cur = pos_other;

				// New grain data
				g_new._dly_cur   = dly;
				g_new._dly_tgt   = dly;
				g_new._dly_stp   = 0;
				g_new._trans_pos = -1;

				// If pitch shifting is activated, applies it on both parts
				if (_ps_flag)
				{
					const TC       add_step = (1 - _rate_grain) / sample_freq;

					if (! g_old.is_ramping ())
					{
						g_old._dly_stp   = add_step;
						g_old._dly_tgt   = g_old._dly_cur + add_step * xfade_dur;
						g_old._trans_len = xfade_dur;
						g_old._trans_pos = 0;
					}

					g_new._dly_stp    = add_step;
					g_new._dly_tgt   += add_step * xfade_dur;
					g_new._trans_len  = xfade_dur;
					g_new._trans_pos  = 0;
				}

				// No pitch shifting: if there was a transition, extend it
				// during the fadeout
				else if (g_old._dly_stp != 0)
				{
					g_old._trans_len = xfade_dur;
					g_old._trans_pos = std::min (g_old._trans_pos, g_old._trans_len - 1);
				}

				// Starts crossfading
				_xfade_pos = 0;

				_time_cur   = dly;
				_prog_trans = -1;
			}
		}

		// Simple time transition
		else if (time_chg_flag)
		{
			Grain &        g = _grain_arr [_grain_cur];
			if (! g.is_ramping ())
			{
				const TC       dif = dly - g._dly_cur;
				assert (nbr_spl > 0);
				g._dly_stp   = dif / nbr_spl;
				g._dly_tgt   = dly;
				g._trans_pos = 0;
				g._trans_len = nbr_spl;
			}

			_time_cur   = dly;
			_prog_trans = -1;
		}
	}
}



// Returns true if the ramp is terminated
template <typename TC>
bool	DelayLineReaderPitch <TC>::process_grain (Grain &g, float dest_ptr [], int src_pos, int nbr_spl)
{
	assert (dest_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (g._trans_len <= 0 || nbr_spl <= g._trans_len);

	bool           term_flag = false;

	TC             dly_beg   = g._dly_cur;
	TC             dly_end   = dly_beg;
	if (g.is_ramping ())
	{
		g._dly_cur   += nbr_spl * g._dly_stp;
		g._trans_pos += nbr_spl;
		if (g._trans_pos >= g._trans_len)
		{
			g._trans_len = -1;
			g._dly_cur   = g._dly_tgt;
			term_flag    = true;
		}
		dly_end = g._dly_cur;
	}

	dly_beg = fstb::limit (dly_beg, _dly_min, _dly_max);
	dly_end = fstb::limit (dly_end, _dly_min, _dly_max);

	_delay_line_ptr->read_block (
		dest_ptr,
		nbr_spl,
		dly_beg,
		dly_end,
		src_pos
	);

	return term_flag;
}



template <typename TC>
int	DelayLineReaderPitch <TC>::get_xfade_len () const
{
	return _ps_flag ? _xfade_dp : _xfade_dn;
}



template <typename TC>
const float *	DelayLineReaderPitch <TC>::use_xfase_shape () const
{
	return _ps_flag ? _xfade_sp_ptr : _xfade_sn_ptr;
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelayLineReaderPitch_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
