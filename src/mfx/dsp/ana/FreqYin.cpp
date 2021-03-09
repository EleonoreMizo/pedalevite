/*****************************************************************************

        FreqYin.cpp
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

#include "fstb/def.h"
#include "mfx/dsp/ana/FreqYin.h"

#include <algorithm>

#include <cassert>
#include <cmath>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqYin::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	const int      win_len_max = int (sample_freq / _min_freq) + 1;
	_buf_len     = 1 << int (ceil (log2 (win_len_max * 3 + (_max_blk_size - 1))));
	_buffer.resize (_buf_len + _vec_size - 1, 0);
	_buf_mask    = _buf_len - 1;
	_buf_pos_w   = _buf_pos_w & _buf_mask;
	_delta_arr.resize ((win_len_max + 1 + (_vec_size - 1)) >> _vec_size_l2);
	_freq_smooth.reset ();

	update_freq_bot_param ();
	update_freq_top_param ();
}



void	FreqYin::set_freq_bot (float f)
{
	_freq_bot = f;
	if (_sample_freq > 0)
	{
		update_freq_bot_param ();
	}
}



void	FreqYin::set_freq_top (float f)
{
	_freq_top = f;
	if (_sample_freq > 0)
	{
		update_freq_top_param ();
	}
}



void	FreqYin::set_smoothing (float responsiveness, float thr)
{
	assert (responsiveness > 0);
	assert (responsiveness <= 1);
	assert (thr >= 0);

	_freq_smooth.set_responsiveness (responsiveness);
	_freq_smooth.set_threshold (thr);
}



void	FreqYin::set_analysis_period (int per)
{
	assert (per > 0);

	_ana_per = per;
	if (_ana_pos >= _ana_per)
	{
		_ana_pos = 0;
	}
}



void	FreqYin::clear_buffers ()
{
	std::fill (_buffer.begin (), _buffer.end (), 0.f);
	_buf_pos_w = 0;
	_ana_pos   = 0;
	_sum_pos   = 0;
	_freq_smooth.reset ();
	std::fill (_delta_arr.begin (), _delta_arr.end (), Delta ());
}



float	FreqYin::process_block (const float spl_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (_freq_bot < _freq_top);
	assert (spl_ptr != nullptr);
	assert (nbr_spl >= 0);

	int            pos_blk = 0;
	do
	{
		int            work_len = nbr_spl - pos_blk;
		work_len = std::min (work_len, _ana_per - _ana_pos);
		work_len = std::min (work_len, _win_len - _sum_pos);
		work_len = std::min (work_len, int (_max_blk_size));

		for (int pos = 0; pos < work_len; ++pos)
		{
			const float    x     = spl_ptr [pos_blk + pos];
			const int      pos_w = (_buf_pos_w + pos) & _buf_mask;
			_buffer [pos_w] = x;
			if (pos_w < _vec_size - 1)
			{
				_buffer [_buf_len + pos_w] = x;
			}
		}

		update_difference_functions_block (work_len);

		_ana_pos  += work_len;
		check_ana_position ();

		_buf_pos_w = (_buf_pos_w + work_len) & _buf_mask;
		pos_blk   += work_len;
	}
	while (pos_blk < nbr_spl);

	return _freq_smooth.get_val ();
}



float	FreqYin::process_sample (float x)
{
	assert (_sample_freq > 0);
	assert (_freq_bot < _freq_top);

	_buffer [_buf_pos_w] = x;
	if (_buf_pos_w < _vec_size - 1)
	{
		_buffer [_buf_len + _buf_pos_w] = x;
	}
	_buf_pos_w = (_buf_pos_w + 1) & _buf_mask;

	update_difference_functions ();

	++ _ana_pos;
	check_ana_position ();

	return _freq_smooth.get_val ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqYin::update_freq_bot_param ()
{
	assert (_sample_freq > 0);

	_max_delta = int (_sample_freq / _freq_bot) + 1;
	_win_len   = _max_delta;
	assert (_win_len + _max_delta + _max_blk_size <= _buf_mask);
	if (_sum_pos >= _win_len)
	{
		// We don't reset the full state here because all running sums will be
		// reset within two periods anyway.
		_sum_pos = 0;
	}
}



void	FreqYin::update_freq_top_param ()
{
	assert (_sample_freq > 0);

	_min_delta = int (_sample_freq / _freq_top);
}



void	FreqYin::update_difference_functions ()
{
	const int      buf_pos_r_m_1 = _buf_pos_w - _win_len - 1; // May be negative
	int            p_ref_o = buf_pos_r_m_1;
	int            p_ref_i = buf_pos_r_m_1 + _win_len;
	const auto     v_ref_o = TS::set1_f32 (_buffer [p_ref_o & _buf_mask]);
	const auto     v_ref_i = TS::set1_f32 (_buffer [p_ref_i & _buf_mask]);

	// Because we're going backward, we have to offset the reading position.
	// We'll have to reverse the vectors after reading.
	p_ref_o -= _vec_size - 1;
	p_ref_i -= _vec_size - 1;

#if fstb_ARCHI == fstb_ARCHI_X86
	// For some reasons, reaching data forward is faster on x86 and
	// backwards faster on ARM.
	for (int delta = _max_delta & ~_vec_mask; delta >= 0; delta -= _vec_size)
#else
	for (int delta = 0; delta <= _max_delta; delta += _vec_size)
#endif // fstb_ARCHI
	{
		Delta &        d_info  = _delta_arr [delta >> _vec_size_l2];
		auto           sum_u   = TS::load_f32 (d_info._sum_u.data ());
		auto           sum_d   = TS::load_f32 (d_info._sum_d.data ());
		const int      p_tst_o = p_ref_o - delta;
		const int      p_tst_i = p_ref_i - delta;
		auto           v_tst_o = TS::loadu_f32 (&_buffer [p_tst_o & _buf_mask]);
		auto           v_tst_i = TS::loadu_f32 (&_buffer [p_tst_i & _buf_mask]);
		v_tst_o = TS::reverse_f32 (v_tst_o);
		v_tst_i = TS::reverse_f32 (v_tst_i);
		const auto     dif1_i  = v_ref_i - v_tst_i;
		const auto     dif1_o  = v_ref_o - v_tst_o;
		const auto     dif2_i  = dif1_i * dif1_i;
		const auto     dif2_o  = dif1_i * dif1_o;

		sum_u += dif2_i;
		sum_d -= dif2_o;

		TS::store_f32 (d_info._sum_u.data (), sum_u);
		TS::store_f32 (d_info._sum_d.data (), sum_d);
	}

	++ _sum_pos;
	check_sum_position ();
}



void	FreqYin::update_difference_functions_block (int nbr_spl)
{
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_blk_size);

	const int      buf_pos_r_m_1 = _buf_pos_w - _win_len - 1; // May be negative
	int            p_ref_o = buf_pos_r_m_1;
	int            p_ref_i = buf_pos_r_m_1 + _win_len;

	typedef std::array <float, _max_blk_size> SrcCache;
	SrcCache       v_ref_o_arr;
	SrcCache       v_ref_i_arr;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		v_ref_o_arr [pos] = _buffer [(p_ref_o + pos) & _buf_mask];
		v_ref_i_arr [pos] = _buffer [(p_ref_i + pos) & _buf_mask];
	}

	// Because we're going backward, we have to offset the reading position.
	// We'll have to reverse the vectors after reading.
	p_ref_o -= _vec_size - 1;
	p_ref_i -= _vec_size - 1;

#if fstb_ARCHI == fstb_ARCHI_X86
	// For some reasons, reaching data forward is faster on x86 and
	// backwards faster on ARM.
	for (int delta = _max_delta & ~_vec_mask; delta >= 0; delta -= _vec_size)
#else
	for (int delta = 0; delta <= _max_delta; delta += _vec_size)
#endif // fstb_ARCHI
	{
		Delta &        d_info  = _delta_arr [delta >> _vec_size_l2];
		auto           sum_u   = TS::load_f32 (d_info._sum_u.data ());
		auto           sum_d   = TS::load_f32 (d_info._sum_d.data ());
		sum_u = TS::reverse_f32 (sum_u);
		sum_d = TS::reverse_f32 (sum_d);

		const int      p_tst_o_base = p_ref_o - delta;
		const int      p_tst_i_base = p_ref_i - delta;
		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const auto     v_ref_o = TS::set1_f32 (v_ref_o_arr [pos]);
			const auto     v_ref_i = TS::set1_f32 (v_ref_i_arr [pos]);
			const int      p_tst_o = p_tst_o_base + pos;
			const int      p_tst_i = p_tst_i_base + pos;
			const auto     v_tst_o = TS::loadu_f32 (&_buffer [p_tst_o & _buf_mask]);
			const auto     v_tst_i = TS::loadu_f32 (&_buffer [p_tst_i & _buf_mask]);
			const auto     dif1_i  = v_ref_i - v_tst_i;
			const auto     dif1_o  = v_ref_o - v_tst_o;
			const auto     dif2_i  = dif1_i * dif1_i;
			const auto     dif2_o  = dif1_i * dif1_o;
			sum_u += dif2_i;
			sum_d -= dif2_o;
		}

		sum_u = TS::reverse_f32 (sum_u);
		sum_d = TS::reverse_f32 (sum_d);
		TS::store_f32 (d_info._sum_u.data (), sum_u);
		TS::store_f32 (d_info._sum_d.data (), sum_d);
	}

	_sum_pos += nbr_spl;
	check_sum_position ();
}



void	FreqYin::check_sum_position ()
{
	if (_sum_pos >= _win_len)
	{
		assert (_sum_pos == _win_len);
		for (int delta = 0; delta <= _win_len; delta += _vec_size)
		{
			Delta &        d_info = _delta_arr [delta >> _vec_size_l2];
			const auto     sum_u  = TS::load_f32 (d_info._sum_u.data ());
			TS::store_f32 (d_info._sum_d.data (), sum_u);
			TS::store_f32 (d_info._sum_u.data (), TS::set_f32_zero ());
		}
		_sum_pos = 0;
	}
}



void	FreqYin::check_ana_position ()
{
	if (_ana_pos >= _ana_per)
	{
		analyse ();
		_ana_pos = 0;
	}
}



void	FreqYin::analyse ()
{
	float          freq    = 0; // 0 = not found yet
	float          dif_sum = 0;
	_delta_arr [0]._cmndf [0] = 1;

	for (int delta = 1; delta <= _max_delta; ++delta)
	{
		Delta &        d_info  = _delta_arr [delta >> _vec_size_l2];
		const int      s_idx   = delta & _vec_mask;
		const float    sum     = d_info._sum_u [s_idx] + d_info._sum_d [s_idx];

		// Step 3: cumulative mean normalized difference function
		dif_sum += sum;
		const float    cmndf = float (sum * delta / dif_sum);

		d_info._cmndf [s_idx] = cmndf;

		if (delta >= _min_delta && freq == 0)
		{
			const float    r1 = get_cmndf (delta - 2);
			const float    r2 = get_cmndf (delta - 1);
			const float    r3 = cmndf;

			if (r2 < _threshold && r3 > r2 && r1 >= r2)
			{
				// Step 5: parabolic interpolation
				// The peak is located between r1 and r3
				const float    frac = (r1 - r3) * 0.5f / (r1 + r3 - 2 * r2);
				assert (frac >= -1);
				assert (frac <= 1);
				freq = _sample_freq / (delta - 1 + frac);
			}
		}
	}

	_freq_smooth.proc_val (freq);
}



float	FreqYin::get_cmndf (int delta) const
{
	assert (delta >= 0);
	assert (delta <= _max_delta);

	const int      d_idx  = delta >> _vec_size_l2;
	const int      s_idx  = delta & _vec_mask;
	const Delta &  d_info = _delta_arr [d_idx];

	return d_info._cmndf [s_idx];
}



/*

Parabolic interpolation formula:

f (x) = a * x^2 + b * x + c

f (-1) = r1
f ( 0) = r2
f ( 1) = r3

a - b + c = r1
        c = r2
a + b + c = r3

a - b = r1 - r2
a + b = r3 - r2
c = r2

2 * a = r1 + r3 - 2 * r2
2 * b = r3 - r1
c = r2

a = r2 + (r3 + r1) * 0.5
b =      (r3 - r1) * 0.5
c = r2

f'(x) = 2 * a * x + b
f'(x) = 0
2 * a * x + b = 0
(r1 + r3 - 2 * r2) * x + (r3 - r1) * 0.5 = 0
x = (r1 - r3) * 0.5 / (r1 + r3 - 2 * r2)

*/



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
