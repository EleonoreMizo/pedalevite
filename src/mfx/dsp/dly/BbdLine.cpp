/*****************************************************************************

        BbdLine.cpp
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
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/dly/BbdLine.h"
#include	"mfx/dsp/rspl/InterpolatorInterface.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BbdLine::init (int max_bbd_size, double sample_freq, rspl::InterpolatorInterface &interp, int ovrspl_l2)
{
	assert (max_bbd_size > 1);
	assert (sample_freq > 0);
	assert (ovrspl_l2 >= 0);

	const int      margin = 256;

	_ovrspl_l2  = ovrspl_l2;
	_interp_ptr = &interp;
	_interp_ptr->set_ovrspl_l2 (_ovrspl_l2);
	_interp_ptr->start (1);

	_max_bbd_size = max_bbd_size;
	_line_ts.set_sample_freq (1);
	_line_ts.set_extra_len (margin);
	set_bbd_size (_max_bbd_size);

	_line_data.set_sample_freq (1);
	_line_data.set_max_delay_time (_max_bbd_size << -_min_speed_l2);
	_imp_len = int (_interp_ptr->get_impulse_len ());
	_group_dly = _interp_ptr->get_group_delay ();
	assert (_group_dly.get_ceil () < _imp_len);
	_line_data.set_unroll_pre (0);
	_line_data.set_unroll_post (_imp_len);
	_line_data.set_extra_len (_imp_len + margin);
	_line_data.update_buffer_size ();

	clear_buffers ();
}



const rspl::InterpolatorInterface &	BbdLine::use_interpolator () const
{
	assert (_interp_ptr != 0);

	return *_interp_ptr;
}



// May cause some transients when the size is increased. It's safer to call
// clear_buffers() after.
void	BbdLine::set_bbd_size (int bbd_size)
{
	assert (bbd_size > 1);
	assert (bbd_size <= _max_bbd_size);

	_bbd_size = bbd_size;
	_line_ts.set_max_delay_time (bbd_size);
	_line_ts.update_buffer_size ();
	_line_ts.update_unroll ();
}



// Call this first, before any other function.
void	BbdLine::set_speed (float speed)
{
	assert (speed * (1 << -_min_speed_l2) >= 1);
	assert (speed <= 1 << _max_speed_l2);

	_speed     =     speed;
	_speed_inv = 1 / speed;
}



// Call this after set_speed() and before read_block().
int	BbdLine::estimate_max_one_shot_proc_w_feedback (float dly_min) const
{
	assert (dly_min >= 1);

	const float    nbr_spl_f = (dly_min - 1) * _speed_inv;
	const int      nbr_spl_i = fstb::floor_int (nbr_spl_f);

	return nbr_spl_i;
}



// dly_beg and dly_end in number of samples of the BBD line.
void	BbdLine::read_block (float dst_ptr [], long nbr_spl, float dly_beg, float dly_end) const
{
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);
	assert (dly_beg >= 1);
	assert (dly_end >= 1);
	assert (dly_beg <= _bbd_size);
	assert (dly_end <= _bbd_size);

	const int      ts_mask      = _line_ts.get_mask ();
	const fstb::FixedPoint *   ts_buf_ptr = _line_ts.get_buffer ();
	const int      data_mask    = _line_data.get_mask ();
	const int      data_len     = _line_data.get_len ();
	const float *  data_buf_ptr = _line_data.get_buffer ();

	double         dly_cur      = dly_beg;
	double         dly_step     = double (dly_end - dly_beg) / nbr_spl;
	dly_step -= _speed;

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = read_sample (
			float (dly_cur), ts_mask, ts_buf_ptr, data_mask, data_len, data_buf_ptr
		);
		dly_cur += dly_step;
	}
}



float	BbdLine::read_sample (float dly) const
{
	assert (dly >= 1);
	assert (dly <= _bbd_size);

	const int      ts_mask      = _line_ts.get_mask ();
	const fstb::FixedPoint *   ts_buf_ptr = _line_ts.get_buffer ();
	const int      data_mask    = _line_data.get_mask ();
	const int      data_len     = _line_data.get_len ();
	const float *  data_buf_ptr = _line_data.get_buffer ();

	return read_sample (
		dly, ts_mask, ts_buf_ptr, data_mask, data_len, data_buf_ptr
	);
}



// speed indicates the rate of the BDD relative to the in/out sampling freq.
void	BbdLine::push_block (const float src_ptr [], int nbr_spl)
{
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	push_timestamps (nbr_spl);

	// Fills the data line
	const int      data_mask    = _line_data.get_mask ();
	float *        data_buf_ptr = _line_data.get_buffer ();
	int            pos_w        = 0;
	do
	{
		const int      rem_len  = nbr_spl - pos_w;
		const int      max_len  = _line_data.get_len () - _data_pos_w;
		const int      work_len = std::min (rem_len, max_len);
		dsp::mix::Generic::copy_1_1 (
			data_buf_ptr + _data_pos_w,
			src_ptr + pos_w,
			work_len
		);
		if (_data_pos_w < _line_data.get_unroll_post ())
		{
			_line_data.update_unroll ();
		}

		_data_pos_w = (_data_pos_w + work_len) & data_mask;
		pos_w      += work_len;
	}
	while (pos_w < nbr_spl);
}



void	BbdLine::push_sample (float x)
{
	push_timestamps (1);

	const int      data_mask    = _line_data.get_mask ();
	float *        data_buf_ptr = _line_data.get_buffer ();

	data_buf_ptr [_data_pos_w] = x;
	if (_data_pos_w < _line_data.get_unroll_post ())
	{
		_line_data.update_unroll ();
	}

	_data_pos_w = (_data_pos_w + 1) & data_mask;
}



void	BbdLine::clear_buffers ()
{
	_line_ts.clear_buffers ();
	_line_data.clear_buffers ();
	_ts_pos_w   = 0;
	_data_pos_w = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	BbdLine::read_sample (float dly_cur, int ts_mask, const fstb::FixedPoint ts_buf_ptr [], int data_mask, int data_len, const float data_buf_ptr []) const
{
	assert (dly_cur >= 1);

	const float    read_pos_f = _ts_pos_w - dly_cur;
	const int      read_pos_i = fstb::floor_int (read_pos_f);
	const float    read_pos_r = read_pos_f - read_pos_i;
	const int      idx_0      =  read_pos_i      & ts_mask;
	const int      idx_1      = (read_pos_i + 1) & ts_mask;
	fstb::FixedPoint  t_0     = ts_buf_ptr [idx_0];
	fstb::FixedPoint  t_1     = ts_buf_ptr [idx_1];
	if (t_1 < t_0)
	{
		t_1 += fstb::FixedPoint (data_len, 0);
		assert (t_0 <= t_1);
	}
	fstb::FixedPoint  ts (t_1);
	ts -= t_0;
	ts.mul_flt (read_pos_r);
	ts += t_0;
	ts -= _group_dly;
	ts.bound_and (data_mask);

	const fstb::FixedPoint  rate (/*** To do ***/ 1);
	const float       y =
		_interp_ptr->process_sample (data_buf_ptr, ts, rate);

	return y;
}



void	BbdLine::push_timestamps (int nbr_spl)
{
	assert (nbr_spl > 0);

	const float    ts_span = _speed * nbr_spl;
	assert (ts_span <= float (_bbd_size)); // This is an upper bound. Real limit depends on the relative reading position.
	float          ts_pos_w_end = _ts_pos_w + ts_span;

	// Stores every position from ceil(_bbd_pos_w) included to
	// ceil(bbd_pos_w_end) excluded.
	const int      ts_pos_w_i_beg = fstb::ceil_int (_ts_pos_w);
	const int      ts_pos_w_i_end = fstb::ceil_int (ts_pos_w_end);

	// Finds the position within the data for bbd_pos_w_i_beg
	const float    delta = ts_pos_w_i_beg - _ts_pos_w;
	const fstb::FixedPoint  offset (delta * _speed_inv);
	fstb::FixedPoint  data_pos_cur (_data_pos_w, 0);
	data_pos_cur += offset;

	// Fills the timestamp line
	const int      ts_mask = _line_ts.get_mask ();
	fstb::FixedPoint *  ts_buf_ptr = _line_ts.get_buffer ();
	const fstb::FixedPoint  speed_fp (_speed_inv);
	for (int ts_pos_cur = ts_pos_w_i_beg
	;	ts_pos_cur < ts_pos_w_i_end
	;	++ ts_pos_cur)
	{
		const int      buf_pos = ts_pos_cur & ts_mask;
		ts_buf_ptr [buf_pos] = data_pos_cur;
		data_pos_cur += speed_fp;
	}

	// Next step
	const int      wrap_back = ts_pos_w_i_end & ~ts_mask;
	_ts_pos_w = ts_pos_w_end - float (wrap_back);
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
