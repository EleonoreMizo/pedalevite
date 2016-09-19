/*****************************************************************************

        DelayLine.cpp
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
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/mix/Generic.h"
#include	"mfx/dsp/rspl/InterpolatorInterface.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Mandatory call
void	DelayLine::set_interpolator (rspl::InterpolatorInterface &interp)
{
	assert (mix::Generic::is_ready ());
	assert (&interp != 0);

	_interp_ptr = &interp;

	_interp_ptr->set_ovrspl_l2 (_ovrspl_l2);

	_imp_len   = _interp_ptr->get_impulse_len ();
	_group_dly = _interp_ptr->get_group_delay ();
	assert (_group_dly.get_ceil () < _imp_len);

	_line_data.set_unroll_pre (0);
	_line_data.set_unroll_post (_imp_len);
	_line_data.set_extra_len (_imp_len);
	_line_data.update_buffer_size ();
	_min_dly_time = double (_imp_len + 1) / _sample_freq;

	_interp_ptr->start (1);
}



const rspl::InterpolatorInterface &	DelayLine::use_interpolator () const
{
	assert (_interp_ptr != 0);

	return (*_interp_ptr);
}



// sample_freq = normal rate (not oversampled)
void	DelayLine::set_sample_freq (double sample_freq, int ovrspl_l2)
{
	assert (sample_freq > 0);
	assert (ovrspl_l2 >= 0);

	_sample_freq = sample_freq;
	_ovrspl_l2   = ovrspl_l2;

	_line_data.set_sample_freq (sample_freq);

	if (_interp_ptr != 0)
	{
		_interp_ptr->set_ovrspl_l2 (_ovrspl_l2);

		_min_dly_time = double (_imp_len + 1) / _sample_freq;
	}
}



double	DelayLine::get_sample_freq () const
{
	return (_sample_freq);
}



int	DelayLine::get_ovrspl_l2 () const
{
	return (_ovrspl_l2);
}



// Mandatory call
void	DelayLine::set_max_delay_time (double max_time)
{
	assert (max_time > 0);

	_max_dly_time = max_time;
	_line_data.set_max_delay_time (_max_dly_time);
}



bool	DelayLine::is_ready () const
{
	return (   _sample_freq > 0
	        && _line_data.get_max_delay_time () > 0
	        && _min_dly_time <= _max_dly_time);
}



void	DelayLine::clear_buffers ()
{
	_write_pos = 0;
	_line_data.clear_buffers ();
}



double	DelayLine::get_min_delay_time () const
{
	assert (is_ready ());

	return (_min_dly_time);
}



double	DelayLine::get_max_delay_time () const
{
	assert (is_ready ());

	return (_max_dly_time);
}



int	DelayLine::estimate_max_one_shot_proc_w_feedback (double min_delay_time) const
{
	assert (is_ready ());
	assert (min_delay_time >= get_min_delay_time ());
	assert (min_delay_time <= get_max_delay_time ());

	int            nbr_spl =
		fstb::floor_int (min_delay_time * _sample_freq);

	nbr_spl -= _imp_len;

	assert (nbr_spl >= 0);

	return (nbr_spl);
}



// pos_in_block (in samples) is subtracted to both delay_beg and delay_end
// (in seconds). Use it to offset the read position if you read the line
// in multiple times between two calls to push_data().
// Important: read data is oversampled, if oversampling has been set.
// Therefore pos_in_block is related to oversampled data, too.
void	DelayLine::read_line (float dest_ptr [], int nbr_spl, double delay_beg, double delay_end, int pos_in_block)
{
	assert (is_ready ());
	assert (dest_ptr != 0);
	assert (nbr_spl > 0);
	assert (delay_beg - pos_in_block / _sample_freq >= _min_dly_time);
	assert (delay_beg <= _max_dly_time);
	assert (delay_end - pos_in_block / _sample_freq >= _min_dly_time);
	assert (delay_end <= _max_dly_time);
	assert (pos_in_block >= 0);

	const int      unroll_post   = _line_data.get_unroll_post ();
	const int      mask          = _line_data.get_mask ();
	const int      line_len      = _line_data.get_len ();
	float * const  data_ptr      = _line_data.get_buffer ();

	const double   delay_beg_spl = delay_beg * _sample_freq;
	const double   delay_end_spl = delay_end * _sample_freq;

	const fstb::FixedPoint  dcur (delay_beg_spl);
	fstb::FixedPoint        rate (1);
	rate >>= _ovrspl_l2;
	rate += fstb::FixedPoint ((delay_beg_spl - delay_end_spl) / nbr_spl);

	const fstb::FixedPoint  comp (compute_compensation_for_group_delay (1));

	fstb::FixedPoint        pos_in_block_line (pos_in_block, 0);
	pos_in_block_line >>= _ovrspl_l2;

	fstb::FixedPoint       pos_cur (_write_pos, 0);
	pos_cur -= dcur;
	pos_cur += pos_in_block_line;
	pos_cur += comp;
	pos_cur.bound_and (mask);

	int            pos_dest = 0;

	do
	{
		const int      proc_len = _interp_ptr->process_block (
			&dest_ptr,
			&data_ptr,
			pos_dest,
			pos_cur,
			nbr_spl,
			0,
			line_len + unroll_post,
			rate,
			fstb::FixedPoint (0, 0)
		);

		pos_dest += proc_len;

		// Second pass required
		if (pos_dest < nbr_spl)
		{
			fstb::FixedPoint  skip (rate);
			skip.mul_int (proc_len);
			pos_cur += skip;
			assert (   pos_cur.get_int_val () < 0
			        || pos_cur.get_int_val () >= line_len);

			pos_cur.bound_and (mask);
		}
	}
	while (pos_dest < nbr_spl);
}



// Input data is not oversampled (if oversampling has been set)
void	DelayLine::push_data (const float src_ptr [], int nbr_spl)
{
	assert (is_ready ());
	assert (src_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _line_data.get_len () - _imp_len);

	const int      unroll_post = _line_data.get_unroll_post ();
	const int      line_len    = _line_data.get_len ();
	const int      mask        = _line_data.get_mask ();
	float *        data_ptr    = _line_data.get_buffer ();

	int            src_pos = 0;
	do
	{
		const int      rem_len  = line_len - _write_pos;
		int            work_len = nbr_spl - src_pos;
		work_len = std::min (work_len, rem_len);

		mix::Generic::copy_1_1 (
			data_ptr + _write_pos,
			src_ptr + src_pos,
			work_len
		);

		// Updates the loop-unrolling zone
		int 				update_len = unroll_post - _write_pos;
		if (update_len > 0)
		{
			update_len = std::min (update_len, work_len);

			mix::Generic::copy_1_1 (
				data_ptr + _write_pos + line_len,
				data_ptr + _write_pos,
				update_len
			);
		}

		_write_pos = (_write_pos + work_len) & mask;
		src_pos += work_len;
	}
	while (src_pos < nbr_spl);
}



void	DelayLine::push_sample (float src)
{
	assert (is_ready ());
	assert (1 <= _line_data.get_len () - _imp_len);

	const int      unroll_post = _line_data.get_unroll_post ();
	const int      line_len    = _line_data.get_len ();
	const int      mask        = _line_data.get_mask ();
	float *        data_ptr    = _line_data.get_buffer ();

	data_ptr [_write_pos] = src;

	// Updates the loop-unrolling zone
	int 				update_len = unroll_post - _write_pos;
	if (update_len > 0)
	{
		data_ptr [_write_pos + line_len] = data_ptr [_write_pos];
	}

	_write_pos = (_write_pos + 1) & mask;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DelayLine::compute_margin () const
{
	assert (_interp_ptr != 0);

	const double   gd1        = _group_dly.get_val_dbl ();
	const double   gd2        = _imp_len - gd1;
	const double   margin_flt = std::max (gd1, gd2);
	const int      margin_spl = fstb::ceil_int (margin_flt);

	return (margin_spl);
}



// Adding compensation moves the cursor in the direction opposed to dir
fstb::FixedPoint	DelayLine::compute_compensation_for_group_delay (int dir) const
{
	assert (dir == +1 || dir == -1);

	fstb::FixedPoint  comp (_imp_len - 1, 0);
	comp -= _group_dly;
	comp.mul_int (-dir);

	return (comp);
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
