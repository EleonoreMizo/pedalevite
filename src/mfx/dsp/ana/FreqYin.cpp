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
	int            win_len_max = int (sample_freq / _min_freq) + 1;
	size_t         buf_len = size_t (1) << int (ceil (log2 (win_len_max * 3)));
	_buffer.resize (buf_len, 0);
	_buf_mask    = int (buf_len) - 1;
	_buf_pos_w   = _buf_pos_w & _buf_mask;
	_delta_arr.resize (win_len_max + 1);
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

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (spl_ptr [pos]);
	}

	return _freq_smooth.get_val ();
}



float	FreqYin::process_sample (float x)
{
	assert (_sample_freq > 0);
	assert (_freq_bot < _freq_top);

	_buffer [_buf_pos_w] = x;
	_buf_pos_w = (_buf_pos_w + 1) & _buf_mask;

	update_difference_functions ();

	++ _ana_pos;
	if (_ana_pos >= _ana_per)
	{
		analyse ();
		_ana_pos = 0;
	}

	return _freq_smooth.get_val ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FreqYin::update_freq_bot_param ()
{
	assert (_sample_freq > 0);

	_win_len   = int (_sample_freq / _freq_bot) + 1;
	assert (_win_len <= _buf_mask);
}



void	FreqYin::update_freq_top_param ()
{
	assert (_sample_freq > 0);

	_min_delta = int (_sample_freq / _freq_top);
}



void	FreqYin::update_difference_functions ()
{
	const int      buf_pos_r_m_1 = _buf_pos_w - _win_len - 1; // May be negative
	const int      p_ref_o = buf_pos_r_m_1;
	const int      p_ref_i = buf_pos_r_m_1 + _win_len;
	const float    v_ref_o = _buffer [p_ref_o & _buf_mask];
	const float    v_ref_i = _buffer [p_ref_i & _buf_mask];

	for (int delta = 1; delta <= _win_len; ++delta)
	{
		Delta &        d_info  = _delta_arr [delta];

		const int      p_tst_o = p_ref_o - delta;
		const int      p_tst_i = p_ref_i - delta;
		const float    v_tst_o = _buffer [p_tst_o & _buf_mask];
		const float    v_tst_i = _buffer [p_tst_i & _buf_mask];
		const float    dif1_i  = v_ref_i - v_tst_i;
		const float    dif1_o  = v_ref_o - v_tst_o;
		const float    dif2_i  = dif1_i * dif1_i;
		const float    dif2_o  = dif1_i * dif1_o;

		d_info._sum_u += dif2_i;
		d_info._sum_d -= dif2_o;
	}

	++ _sum_pos;
	if (_sum_pos >= _win_len)
	{
		for (int delta = 1; delta <= _win_len; ++delta)
		{
			Delta &        d_info = _delta_arr [delta];
			d_info._sum_d = d_info._sum_u;
			d_info._sum_u = 0;
		}
		_sum_pos = 0;
	}
}



void	FreqYin::analyse ()
{
	float          freq    = 0; // 0 = not found yet
	float          dif_sum = 0;
	_delta_arr [0]._cmndf  = 1;

	for (int delta = 1; delta <= _win_len; ++delta)
	{
		Delta &        d_info  = _delta_arr [delta];
		const float    sum     = d_info._sum_u + d_info._sum_d;

		// Step 3: cumulative mean normalized difference function
		dif_sum += sum;

		d_info._cmndf = float (sum * delta / dif_sum);

		if (delta >= _min_delta && freq == 0)
		{
			const float    r1 = _delta_arr [delta - 2]._cmndf;
			const float    r2 = _delta_arr [delta - 1]._cmndf;
			const float    r3 = _delta_arr [delta    ]._cmndf;

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
