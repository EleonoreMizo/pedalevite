/*****************************************************************************

        SnhTool.cpp
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

#include	"mfx/dsp/rspl/SnhTool.h"

#include <algorithm>

#include	<cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SnhTool::SnhTool ()
:	_chn_state_arr ()
,	_nbr_chn (1)
,	_ovrspl_l2 (0)
,	_hold_time (1)
,	_rep_index (0)
,	_interp_val (0)
,	_interp_step (0)
,	_rem_spl (0)
,	_sub_index (0)
,	_nbr_sub (1)
{
	// Nothing
}



/*
==============================================================================
Name: set_nbr_chn
Description:
	Sets the number of channels of the data to be processed.
	Can be called from a real-time thread.
Input parameters:
	- nbr_chn: Number of channels, [1 ; Constants::MAX_NBR_CHN]
Throws: Nothing
==============================================================================
*/

void	SnhTool::set_nbr_chn (int nbr_chn)
{
	assert (nbr_chn > 0);
	assert (nbr_chn <= Cst::MAX_NBR_CHN);

	_nbr_chn = nbr_chn;

	clear_buffers ();
}



int	SnhTool::get_nbr_chn () const
{
	return _nbr_chn;
}



/*
==============================================================================
Name: set_ovrspl
Description:
	Sets the global oversampling rates.
	Can be called from a real-time thread.
Input parameters:
	- ovrspl_l2: base-2 logarithm of the oversampling rate, >= 0. Ex: 4 = 16x
		oversampling, 0 = no oversampling.
Throws: Nothing
==============================================================================
*/

void	SnhTool::set_ovrspl (int ovrspl_l2)
{
	assert (ovrspl_l2 >= 0);

	_ovrspl_l2 = ovrspl_l2;
	_hold_time = 1;

	clear_buffers ();
}



/*
==============================================================================
Name: compute_snh_data
Description:
	Call this function before resampling a block of data.
	Then, process one sample every hold_time, starting the skip count at
	rep_index (increasing), 0 being the index where the sample has to be
	processed.
	Other samples will be ignored and can be left unprocessed.
	Here, provided rate must be positive. It's possible to use negative rates
	in your processing.
Input parameters:
	- max_nbr_spl: Maximum block length, in samples. Actual processed block may
		be shorter. > 0.
	- rate: Resampling rate at the beginning of the block. Must be positive.
	- rate_step: Per-sample rate increment.
Output parameters:
	- hold_time: Size of the gap between two significant samples. > 0.
	- rep_index: Skip index for the first sample of the block.
		Range [0 ; hold_time-1].
Throws: Nothing
==============================================================================
*/

void	SnhTool::compute_snh_data (int &hold_time, int &rep_index, long max_nbr_spl, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step) const
{
	assert (&hold_time != 0);
	assert (&rep_index != 0);
	assert (max_nbr_spl > 0);
	assert (&rate != 0);
	assert (&rate_step != 0);

	// Optimisation for non-oversampled data
	if (_ovrspl_l2 == 0)
	{
		assert (_hold_time == 1);
		assert (_rep_index == 0);
		hold_time = 1;
		rep_index = 0;
	}

	else
	{
		// Finds the minimum hold time which can be expected during the block
		fstb::FixedPoint  rate_beg (rate);
		rate_beg.abs ();

		fstb::FixedPoint	rate_end (rate_step);
		rate_end.mul_int (max_nbr_spl);
		rate_end.add (rate);
		rate_end.abs ();

		fstb::FixedPoint	rate_max (std::max (rate_beg, rate_end));
		hold_time = compute_hold_time (rate_max, _ovrspl_l2);

		// If we are currently in transition, the current hold time may be shorter
		// than the computed one.
		if (_hold_time < hold_time)
		{
			hold_time = _hold_time;
		}

		// Wraps the repeat index because computed hold time may be shorter than
		// the transition one.
		rep_index = _rep_index & (hold_time - 1);
	}
}



// Same for a single sample. Indicates if we should generate data.
bool	SnhTool::compute_snh_data_sample (const fstb::FixedPoint &rate) const
{
	if (_ovrspl_l2 == 0)
	{
		return true;
	}

	/*** To do: optimize by just comparing the provided rate with the current rate ***/

	int            hold_time = compute_hold_time (rate, _ovrspl_l2);

	// If we are currently in transition, the current hold time may be shorter
	// than the computed one.
	if (_hold_time < hold_time)
	{
		hold_time = _hold_time;
	}

	// Wraps the repeat index because computed hold time may be shorter than
	// the transition one.
	const int      rep_index = _rep_index & (hold_time - 1);

	return (rep_index == 0);
}



/*
==============================================================================
Name: process_data
Description:
	Calls this function once the block of data has been processed. Rate
	parameters must be the same as the ones passed to compute_snh_data().
Input parameters:
	- nbr_spl: Size of the processed block. Must be less or equal as
		max_nbr_spl in compute_snh_data(). > 0.
	- rate: Resampling rate at the beginning of the block. Must be positive.
	- rate_step: Per-sample rate increment.
Input/output parameters:
	- data_ptr_arr: Array of pointers on (partially) processed data as input,
		one pointer per channel. On output, data is filled with missing samples
		and transitions are smoothed, so the buffers are ready to be
		downsampled.
Throws: Nothing
==============================================================================
*/

void	SnhTool::process_data (float * const data_ptr_arr [], long nbr_spl, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step)
{
	assert (data_ptr_arr != 0);
	assert (data_ptr_arr [0] != 0);
	assert (nbr_spl > 0);
	assert (rate.get_val_int64 () > 0);

	bool				need_trans_flag = false;
	int				ht_new;
	long				pos = 0;
	do
	{
		// First, terminates the transition, if any.
		if (_rem_spl > 0)
		{
			const long		rem_len = nbr_spl - pos;
			const long		work_len = std::min (rem_len, _rem_spl);
			process_data_interpolate (data_ptr_arr, pos, pos + work_len);
			pos += work_len;
		}

		// Computes the end rate
		if (_rem_spl == 0)
		{
			fstb::FixedPoint	rate_end (rate_step);
			rate_end.mul_int (nbr_spl);
			rate_end.add (rate);
			ht_new = compute_hold_time (rate_end, _ovrspl_l2);
			need_trans_flag = (ht_new != _hold_time);
		}

		// Advances up to the next block in steady state, or up to the end
		if (_rem_spl == 0)
		{
			const long		rem_len = nbr_spl - pos;
			long				work_len = rem_len;
			if (need_trans_flag)
			{
				if (_rep_index == 0)
				{
					work_len = 0;
				}
				else
				{
					const long		rem_block_len = _hold_time - _rep_index;
					work_len = std::min (work_len, rem_block_len);
				}
			}
			if (work_len > 0)
			{
				process_data_steady_state (data_ptr_arr, pos, pos + work_len);
				pos += work_len;
			}
		}

		// Now we are correcly positionned regarding _rep_index, starts the
		// transition, if it is required
		if (need_trans_flag)
		{
			assert (_rep_index == 0);
			_sub_index = 0;

			int				ht_max;
			int				ht_min;
			fstb::sort_2_elt (ht_min, ht_max, _hold_time, ht_new);
			_rem_spl = FADE_LEN;
			if (ht_max == _hold_time)
			{
				_interp_val = 0;
				_interp_step = 1.0f / FADE_LEN;
			}
			else
			{
				_interp_val = 1;
				_interp_step = -1.0f / FADE_LEN;
			}
			_nbr_sub = ht_max / ht_min;
			_hold_time = ht_min;

			need_trans_flag = false;
		}

	}	// We loop to process the transition, if any
	while (pos < nbr_spl);
}



/*
==============================================================================
Name: clear_buffers
Description:
	
	Can be called from a real-time thread.
Throws: Nothing
==============================================================================
*/

void	SnhTool::clear_buffers ()
{
	_sub_index = 0;
	_rep_index = 0;
}



/*
==============================================================================
Name: adjust_rate_param
Description:
	Transforms the resampling rate parameters according to the
	compute_snh_data() results, so the optimised processing loop can be run
	with only one rate update between two generated samples, whatever the
	hold_time value.
Input parameters:
	- hold_time: result of the compute_snh_data() call.
	- rep_index: result of the compute_snh_data() call.
Input/output parameters:
	- pos_dest: On input, this is the destination cursor (generally 0).
		On output, pos_dest is possibly increased in order to point right on the
		first sample to generate.
	- pos_src: Cursor position within the source block. On output, it is
		updated to match the new pos_dest.
	- rate: normal resampling rate on input, modified resampling rate on
		output. Negative rates are accepted.
	- rate_step: normal resampling rate per-sample increment on input, modified
		one on output.
Throws: Nothing
==============================================================================
*/

void	SnhTool::adjust_rate_param (long &pos_dest, fstb::FixedPoint &pos_src, fstb::FixedPoint &rate, fstb::FixedPoint &rate_step, int hold_time, int rep_index)
{
	assert (&pos_dest != 0);
	assert (&pos_src != 0);
	assert (&rate != 0);
	assert (&rate_step != 0);
	assert (hold_time > 0);
	assert (rep_index >= 0);
	assert (rep_index < hold_time);

/*
Iteration:
	rate [t+1] = rate [t] + rate_step
	pos [t+1] = pos [t] + rate [t]

Integration:
	rate [t+k] = rate [t] + rate_step * k
	pos [t+k] = pos [t] + rate [t] * k + rate_step * k * (k-1) / 2

Substitution:
	rate' [t+k] = rate' [t] + rate_step'
	pos [t+k] = pos [t] + rate' [t]

Substitued variable values:
	rate' [t] = pos [t+k] - pos [t]
	          = rate [t] * k + rate_step * k * (k-1) / 2
	rate_step' = rate' [t+k] - rate' [t]
	           = rate [t+k] * k - rate [t] * k
				  = (rate [t] + rate_step * k) * k - rate [t] * k
	           = rate_step * k * k
*/

	if (rep_index > 0)
	{
		pos_dest += hold_time - rep_index;
		while (rep_index < hold_time)
		{
			pos_src += rate;
			rate += rate_step;
			++ rep_index;
		}
	}

	rate_step.mul_int (hold_time);

	fstb::FixedPoint	rate_fix (rate_step);
	rate_fix.mul_int (hold_time - 1);
	rate_fix.shift_right (1);

	rate.mul_int (hold_time);
	rate.add (rate_fix);

	rate_step.mul_int (hold_time);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SnhTool::process_data_steady_state (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (_rem_spl == 0);
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_end > pos_beg);

	if (_hold_time > 1)
	{
		// Finishes the current hold block
		if (_rep_index > 0)
		{
			const long		block_end = std::min (
				long (pos_beg + _hold_time - _rep_index),
				pos_end
			);
			process_data_steady_state_naive (data_ptr_arr, pos_beg, block_end);
			pos_beg = block_end;
		}

		if (pos_beg < pos_end)
		{
			// Full hold blocks
			const long		nbr_blocks = (pos_end - pos_beg) / _hold_time;
			if (nbr_blocks > 0)
			{
				const long		block_end = pos_beg + nbr_blocks * _hold_time;
				process_data_steady_state_block (data_ptr_arr, pos_beg, block_end);
				pos_beg = block_end;
			}

			// Beginning of the last hold block
			if (pos_beg < pos_end)
			{
				process_data_steady_state_naive (data_ptr_arr, pos_beg, pos_end);
			}
		}
	}
}



void	SnhTool::process_data_steady_state_naive (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (_rem_spl == 0);
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_end > pos_beg);

	int				rep_index;

	int				chn_cnt = 0;
	do
	{
		ChnState &		chn_state = _chn_state_arr [chn_cnt];
		float *			data_ptr = data_ptr_arr [chn_cnt];

		rep_index = _rep_index;

		long				pos = pos_beg;
		do
		{
			if (rep_index == 0)
			{
				chn_state._hold_val = data_ptr [pos];
			}

			else
			{
				data_ptr [pos] = chn_state._hold_val;
			}

			++ pos;
			++ rep_index;
			if (rep_index >= _hold_time)
			{
				rep_index = 0;
			}
		}
		while (pos < pos_end);

		++ chn_cnt;
	}
	while (chn_cnt < _nbr_chn);

	_rep_index = rep_index;
}



void	SnhTool::process_data_steady_state_block (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (_rem_spl == 0);
	assert (_rep_index == 0);
	assert (_hold_time > 1);
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_end > pos_beg);
	assert ((pos_end - pos_beg) % _hold_time == 0);

	int				chn_cnt = 0;
	do
	{
		ChnState &		chn_state = _chn_state_arr [chn_cnt];
		float *			data_ptr = data_ptr_arr [chn_cnt];

		long				pos = pos_beg;
		do
		{
			const long		block_end = pos + _hold_time;

			chn_state._hold_val = data_ptr [pos];
			++ pos;

			do
			{
				data_ptr [pos] = chn_state._hold_val;
				++ pos;
			}
			while (pos < block_end);
		}
		while (pos < pos_end);

		++ chn_cnt;
	}
	while (chn_cnt < _nbr_chn);
}



void	SnhTool::process_data_interpolate (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_beg < pos_end);

	const long		nbr_spl = pos_end - pos_beg;
	assert (nbr_spl <= _rem_spl);

	const long		block_len = _hold_time * _nbr_sub;

	// Finishes the current hold block
	if (_rep_index > 0 || _sub_index > 0)
	{
		const long		block_end = std::min (
			long (pos_beg + block_len - _sub_index * _hold_time - _rep_index),
			pos_end
		);
		process_data_interpolate_naive (data_ptr_arr, pos_beg, block_end);
		pos_beg = block_end;
	}

	if (pos_beg < pos_end)
	{
		// Full hold blocks
		const long		nbr_blocks = (pos_end - pos_beg) / block_len;
		if (nbr_blocks > 0)
		{
			const long		block_end = pos_beg + nbr_blocks * block_len;
			process_data_interpolate_block (data_ptr_arr, pos_beg, block_end);
			pos_beg = block_end;
		}

		// Beginning of the last hold block
		if (pos_beg < pos_end)
		{
			process_data_interpolate_naive (data_ptr_arr, pos_beg, pos_end);
		}
	}

	_rem_spl -= nbr_spl;

	// Sets the new hold time
	if (_rem_spl <= 0 && _interp_val < 0.5f)
	{
		_hold_time *= _nbr_sub;
	}
}



void	SnhTool::process_data_interpolate_naive (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_beg < pos_end);

	int				rep_index;
	int				sub_index;
	float				interp_val;

	int				chn_cnt = 0;
	do
	{
		ChnState &		chn_state = _chn_state_arr [chn_cnt];
		float *			data_ptr = data_ptr_arr [chn_cnt];

		rep_index  = _rep_index;
		sub_index  = _sub_index;
		interp_val = _interp_val;

		float				dif = chn_state._hold_val - chn_state._hold_val_max;

		long				pos = pos_beg;
		do
		{
			if (rep_index == 0)
			{
				chn_state._hold_val = data_ptr [pos];
				if (sub_index == 0)
				{
					chn_state._hold_val_max = chn_state._hold_val;
				}
				dif = chn_state._hold_val - chn_state._hold_val_max;
			}

			data_ptr [pos] = chn_state._hold_val_max + interp_val * dif;

			interp_val += _interp_step;
			++ pos;
			++ rep_index;
			if (rep_index >= _hold_time)
			{
				rep_index = 0;

				++ sub_index;
				if (sub_index >= _nbr_sub)
				{
					sub_index = 0;
				}
			}
		}
		while (pos < pos_end);

		++ chn_cnt;
	}
	while (chn_cnt < _nbr_chn);

	_rep_index  = rep_index;
	_sub_index  = sub_index;
	_interp_val = interp_val;
}



void	SnhTool::process_data_interpolate_block (float * const data_ptr_arr [], long pos_beg, long pos_end)
{
	assert (_rep_index == 0);
	assert (_sub_index == 0);
	assert (_nbr_sub > 1);
	assert (data_ptr_arr != 0);
	assert (pos_beg >= 0);
	assert (pos_end > pos_beg);
	assert ((pos_end - pos_beg) % (_hold_time * _nbr_sub) == 0);

	const long		block_len = _hold_time * _nbr_sub;
	float				interp_val;

	int				chn_cnt = 0;
	do
	{
		ChnState &		chn_state = _chn_state_arr [chn_cnt];
		float *			data_ptr = data_ptr_arr [chn_cnt];

		interp_val = _interp_val;

		long				pos = pos_beg;
		do
		{
			const long		block_end = pos + block_len;

			chn_state._hold_val_max = data_ptr [pos];

			{
				const long		sub_block_end = pos + _hold_time;
				do
				{
					data_ptr [pos] = chn_state._hold_val_max;
					++ pos;
				}
				while (pos < sub_block_end);
				interp_val += _interp_step * _hold_time;
			}

			do
			{
				chn_state._hold_val = data_ptr [pos];
				const float		dif = chn_state._hold_val - chn_state._hold_val_max;

				const long		sub_block_end = pos + _hold_time;
				do
				{
					data_ptr [pos] = chn_state._hold_val_max + interp_val * dif;
					interp_val += _interp_step;
					++ pos;
				}
				while (pos < sub_block_end);
			}
			while (pos < block_end);
		}
		while (pos < pos_end);

		++ chn_cnt;
	}
	while (chn_cnt < _nbr_chn);

	_interp_val = interp_val;
}



int	SnhTool::compute_hold_time (const fstb::FixedPoint &rate, int ovrspl_l2)
{
	assert (&rate != 0);
	assert (rate.get_val_int64 () > 0);
	assert (ovrspl_l2 >= 0);

	int				k = 0;

	// - 1 to avoid useless oversampling for rates 1/2, 1/4, 1/8, etc.
	const int64_t  r = rate.get_val_int64 () - 1;

	if (r <= 0xFFFFFFFFUL)
	{
		// k = min (ovrspl_l2, 32 - greater bit set in frac_val)
		uint32_t       frac_val = uint32_t (r);
		while (k < ovrspl_l2 && ((frac_val << k) & 0x80000000UL) == 0)
		{
			++ k;
		}
	}

	return (1 << k);
}



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
