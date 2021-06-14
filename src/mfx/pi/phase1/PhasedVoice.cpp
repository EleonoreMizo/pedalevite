/*****************************************************************************

        PhasedVoice.cpp
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

#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/pi/phase1/PhasedVoice.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace phase1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PhasedVoice::PhasedVoice ()
:	_sample_freq (44100)
,	_inv_fs (1 / _sample_freq)
,	_depth (0)
,	_osc_freq (0)
,	_osc_hold_flag (false)
,	_osc_neg_flag (false)
,	_fdbk_level (0)
,	_fdbk_color (0)
,	_fdbk_color_old (0)
,	_apfd_delay (0)
,	_apfd_coef (0)
,	_phase_osc_cur (0)
,	_phase_osc_step (0)
,	_phase_man (0)
,	_phase_ref (0)
,	_phase_man_cur (0)
,	_phase_man_old (0)
,	_phase_hold_cur (0)
,	_phase_hold_old (0)
,	_fdbk_lvl_cur (0)
,	_fdbk_lvl_old (0)
,	_fdbk_buf (0)
,	_fdbk_col_c (1)
,	_fdbk_col_s (0)
,	_bpf_freq (640)
,	_bpf_q (0.1f)
,	_osc_freq_update_flag (true)
,	_hold_param_changed_flag (true)
,	_hold_phase_chanded_flag (true)
,	_bpf_changed_flag (true)
,	_bpf_flag (false)
,	_dist_flag (true)
,	_apf_changed_flag (true)
,	_phase_filter_0 ()
,	_phase_filter_1 ()
,	_phase_filter_2 ()
,	_phase_filter_3 ()
,	_phase_filter_list ()
,	_phase_filter_ptr (nullptr)
,	_osc_phase ()
,	_fdbk_color_ramper ()
,	_tmp_buf_ptr (nullptr)
,	_max_buf_len (0)
,	_buf_size (0)
,	_bpf_arr ()
,	_apf_delay ()
{
	assert (_bpf_q < _bpf_q_threshold);

	setup_phase_filters ();
	set_depth (0);
}



// Size of the area pointed by tmp_buf_ptr should be at least
// Buf_NBR_ELT * (max_buf_len rounded to the next multiple of 4).
void	PhasedVoice::reset (float sample_freq, int max_buf_len, float *tmp_buf_ptr)
{
	assert (sample_freq > 0);
	assert (max_buf_len > 0);
	assert (tmp_buf_ptr != nullptr);

	_max_buf_len = max_buf_len;
	_tmp_buf_ptr = tmp_buf_ptr;
	_sample_freq = sample_freq;
	_inv_fs      = 1.0f / sample_freq;

	_buf_size = (_max_buf_len + 3) & -4;

	const int      nbr_apf_max =
		fstb::ceil_int (Cst::_max_apf_delay_time * sample_freq);
	_apf_delay.reserve (nbr_apf_max);
	_apf_changed_flag = true;

	clear_buffers ();
}



void	PhasedVoice::set_polarity (bool neg_flag)
{
	_osc_neg_flag = neg_flag;
	update_phase_osc_step ();
}



void	PhasedVoice::set_speed (float speed)
{
	assert (speed > 0);

	_osc_freq = speed;
	update_phase_osc_step ();
}



void	PhasedVoice::set_depth (int depth)
{
	assert (depth >= 0);
	assert (depth < Cst::_nbr_phase_filters);

	_phase_filter_ptr = _phase_filter_list [depth];
}



void	PhasedVoice::set_fdbk_level (float lvl)
{
	assert (lvl >= 0);
	assert (lvl <= 1);

	_fdbk_level = lvl;
}



void	PhasedVoice::set_fdbk_color (float col)
{
	assert (col >= 0);
	assert (col <= 1);

	_fdbk_color = col;
	update_fdbk_color ();
}



void	PhasedVoice::set_phase (float phase)
{
	_phase_ref     = phase * float (fstb::PI * 2);
	_phase_man_cur = _phase_man + _phase_ref;
	_osc_freq_update_flag = true;
}



void	PhasedVoice::set_phase_shift (float phase)
{
	_phase_man     = phase * float (fstb::PI * 2);
	_phase_man_cur = _phase_man + _phase_ref;
	_osc_freq_update_flag = true;
}



void	PhasedVoice::set_hold (bool hold_flag)
{
	_osc_hold_flag           = hold_flag;
	_osc_freq_update_flag    = true;
	_hold_param_changed_flag = true;
}



void	PhasedVoice::set_bpf_cutoff (float freq)
{
	assert (freq > 0);
	assert (freq < _sample_freq * 0.5f);

	_bpf_freq = freq;
	_bpf_changed_flag = true;
}



void	PhasedVoice::set_bpf_q (float q)
{
	assert (q > 0);

	_bpf_q = q;
	_bpf_flag = (q >= _bpf_q_threshold);
	_bpf_changed_flag = true;
}



void	PhasedVoice::set_dist (bool dist_flag)
{
	_dist_flag = dist_flag;
}



void	PhasedVoice::set_ap_delay (float dly)
{
	assert (dly >= 0);
	assert (dly <= Cst::_max_apf_delay_time);

	_apfd_delay       = dly;
	_apf_changed_flag = true;
}



void	PhasedVoice::set_ap_coef (float coef)
{
	assert (coef >= -1);
	assert (coef <=  1);

	_apfd_coef        = coef;
	_apf_changed_flag = true;
}



void	PhasedVoice::clear_buffers ()
{
	_osc_phase.set_phase (0);
	_fdbk_color_ramper.set_phase (0);

	for (auto &filter_ptr : _phase_filter_list)
	{
		filter_ptr->clear_buffers ();
	}
	for (auto &filter : _bpf_arr)
	{
		filter.clear_buffers ();
	}

	_fdbk_buf = 0;
	update_fdbk_color ();
	_fdbk_color_old = _fdbk_color;

	_hold_param_changed_flag = true;
	_hold_phase_chanded_flag = true;
	_apf_changed_flag        = true;

	_apf_delay.clear_buffers ();
}



void	PhasedVoice::process_block (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr_arr [0]));
	assert (fstb::DataAlign <true>::check_ptr (dst_ptr_arr [1]));
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_apf_changed_flag)
	{
		update_apf ();
	}

	update_hold_phase ();
	update_osc_phase (nbr_spl);
	process_osc (nbr_spl);

	if (_fdbk_color == _fdbk_color_old)
	{
		process_block_cfc (dst_ptr_arr, src_ptr, nbr_spl);
	}
	else
	{
		process_block_vfc (dst_ptr_arr, src_ptr, nbr_spl);
	}

	process_block_fx (dst_ptr_arr, nbr_spl);

	_hold_param_changed_flag = false;
	_hold_phase_chanded_flag = false;
	_fdbk_color_old = _fdbk_color;
	_phase_man_old  = _phase_man_cur;
	_phase_hold_old = _phase_hold_cur;
}



const float	PhasedVoice::_bpf_q_threshold = 0.101f;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float *	PhasedVoice::use_buf (Buf buf)
{
	assert (buf >= 0);
	assert (buf < Buf_NBR_ELT);
	assert (_tmp_buf_ptr != nullptr);
	assert (_buf_size > 0);

	return &_tmp_buf_ptr [_buf_size * buf];
}



void	PhasedVoice::setup_phase_filters ()
{
	_phase_filter_list [0] = &_phase_filter_0;
	_phase_filter_list [1] = &_phase_filter_1;
	_phase_filter_list [2] = &_phase_filter_2;
	_phase_filter_list [3] = &_phase_filter_3;

	static const std::array <double, Cst::_nbr_phase_filters>   tbw_arr =
	{{
		1.0 / 100, 1.0 / 1000, 1.0 / 20000, 1.0 / 20000
	}};
	std::array <double, 256>   coef_list;
	for (size_t i = 0; i < _phase_filter_list.size (); ++i)
	{
		auto &         pp_uptr  = _phase_filter_list [i];
		const int      nbr_coef = pp_uptr->get_nbr_coefs ();
		const double   tbw      = tbw_arr [i];
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&coef_list [0], nbr_coef, tbw
		);
		pp_uptr->set_coefs (&coef_list [0]);
	}
}



void	PhasedVoice::update_phase_osc_step ()
{
	_phase_osc_step = float (2 * fstb::PI) * _osc_freq / _sample_freq;
	if (_osc_neg_flag)
	{
		_phase_osc_step = -_phase_osc_step;
	}

	_osc_freq_update_flag = true;
}



void	PhasedVoice::update_fdbk_color ()
{
	const float    phase = _fdbk_color * float (2 * fstb::PI);

	_fdbk_col_c = float (cos (phase));
	_fdbk_col_s = float (sin (phase));
}



void	PhasedVoice::update_hold_phase ()
{
	if (_hold_phase_chanded_flag)
	{
		if (_osc_hold_flag)
		{
			_phase_osc_cur  = _phase_hold_cur;
		}
		else
		{
			_phase_hold_cur = _phase_osc_cur;
			_phase_hold_old = _phase_hold_cur;
		}
	}
	else if (_hold_param_changed_flag)
	{
		_phase_hold_cur = _phase_osc_cur;
		_phase_hold_old = _phase_hold_cur;
	}
}



void	PhasedVoice::update_osc_phase (int nbr_spl)
{
	assert (nbr_spl > 0);

	_osc_phase.set_phase (_phase_osc_cur + _phase_man_old);

	if (! _osc_hold_flag)
	{
		_phase_osc_cur += _phase_osc_step * float (nbr_spl);
	}

	if (_osc_freq_update_flag)
	{
		_osc_freq_update_flag = false;

		float          freq_rad_spl = // rad/spl
			  (_osc_hold_flag)
			? 0.f
			: _osc_freq * float (fstb::PI * 2) / _sample_freq;

		// Little trick to get smooth manual phase changes: we shift the osc
		// frequency to match _phase_man_cur at the end of the block
		if (	 (_phase_man_old  != _phase_man_cur ) 
		    || (_phase_hold_old != _phase_hold_cur) )
		{
			const float    phase_dif =
				_phase_man_cur - _phase_man_old + _phase_hold_cur - _phase_hold_old;
			const float    freq_dif_rad_spl = phase_dif / float (nbr_spl);

			freq_rad_spl += freq_dif_rad_spl;

			// We'll have to set the real frequency during the next frame 
			_osc_freq_update_flag = true;
		}

		_osc_phase.set_step (freq_rad_spl);
	}
}



void	PhasedVoice::update_bpf ()
{
	float          bs [3];
	float          as [3];
	dsp::iir::DesignEq2p::make_band_pass (bs, as, _bpf_q);

	const float    k =
		dsp::iir::TransSZBilin::compute_k_approx (_bpf_freq * _inv_fs);

	float          bz [3];
	float          az [3];
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);

	for (auto &bpf : _bpf_arr)
	{
		bpf.set_z_eq (bz, az);
	}
}



void	PhasedVoice::update_apf ()
{
	const int      nbr_apf_old = _apf_delay.get_length ();
	const float    nbr_apf_flt = _apfd_delay * _sample_freq;
	const int      nbr_apf     = fstb::ceil_int (nbr_apf_flt - 1e-3f); // Small margin for no delay at all

	if (nbr_apf <= nbr_apf_old)
	{
		_apf_delay.set_length (nbr_apf);
	}
	else
	{
		float          state_last = 0;
		if (nbr_apf_old > 0)
		{
			state_last = _apf_delay.get_state (nbr_apf_old);
		}
		_apf_delay.set_length (nbr_apf);
		for (int pos = nbr_apf_old; pos < nbr_apf; ++pos)
		{
			_apf_delay.set_state (pos + 1, state_last);
		}
	}

	_apf_delay.set_coef_all (_apfd_coef);

	// Last element: fractional delay
	if (nbr_apf_flt < float (nbr_apf))
	{
		const float     delay   = nbr_apf_flt - float (nbr_apf - 1);
		float           coef    = _apfd_coef;
		const float     nodelay = (coef < 0) ? -1.f : 1.f;
		coef = nodelay + delay * (coef - nodelay);

		_apf_delay.set_coef (nbr_apf - 1, coef);
	}
}



void	PhasedVoice::process_osc (int nbr_spl)
{
	assert ((nbr_spl & ((1 << Osc::_nbr_units_l2) - 1)) == 0);

	float *        buf_c_ptr = use_buf (Buf_C);
	float *        buf_s_ptr = use_buf (Buf_S);
	_osc_phase.process_block (
		buf_c_ptr,
		buf_s_ptr,
		nbr_spl >> Osc::_nbr_units_l2
	);
	_osc_phase.correct_fast ();
}



void	PhasedVoice::process_osc_fdbk_col (int nbr_spl)
{
	assert ((nbr_spl & ((1 << Osc::_nbr_units_l2) - 1)) == 0);

	float *        buf_c_ptr = use_buf (Buf_FC_C);
	float *        buf_s_ptr = use_buf (Buf_FC_S);
	_fdbk_color_ramper.process_block (
		buf_c_ptr,
		buf_s_ptr,
		nbr_spl >> Osc::_nbr_units_l2
	);
}



void	PhasedVoice::process_block_cfc (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl)
{
	assert (_tmp_buf_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_buf_len);

	float *        buf_l_ptr = dst_ptr_arr [0];
	float *        buf_r_ptr = dst_ptr_arr [1];
	const float *  buf_c_ptr = use_buf (Buf_C);
	const float *  buf_s_ptr = use_buf (Buf_S);

	const float    fdbk_level_step =
		(_fdbk_level - _fdbk_lvl_old) / float (nbr_spl);
	_fdbk_lvl_cur = _fdbk_lvl_old;

	int            pos = 0;
	do
	{
		const float    cos_a = buf_c_ptr [pos];
		const float    sin_a = buf_s_ptr [pos];
		process_sample (
			buf_l_ptr [pos],
			buf_r_ptr [pos],
			src_ptr [pos],
			cos_a,
			sin_a
		);
		_fdbk_lvl_old += fdbk_level_step;
		++pos;
	}
	while (pos < nbr_spl);

	_fdbk_lvl_old = _fdbk_level;
}



void	PhasedVoice::process_block_vfc (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl)
{
	assert (_tmp_buf_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_buf_len);

	float *        buf_l_ptr = dst_ptr_arr [0];
	float *        buf_r_ptr = dst_ptr_arr [1];
	const float *  buf_c_ptr = use_buf (Buf_C);
	const float *  buf_s_ptr = use_buf (Buf_S);
	const float *  buf_fc_c_ptr = use_buf (Buf_FC_C);
	const float *  buf_fc_s_ptr = use_buf (Buf_FC_S);

	const float    inv_nbr_spl = 1.f / float (nbr_spl);
	const float    old_phase = _fdbk_color_old * float (2 * fstb::PI);
	const float    new_phase = _fdbk_color     * float (2 * fstb::PI);
	_fdbk_color_ramper.set_phase (old_phase);
	_fdbk_color_ramper.set_step ((new_phase - old_phase) * inv_nbr_spl);

	const float		fdbk_level_step =
		(_fdbk_level - _fdbk_lvl_old) * inv_nbr_spl;
	_fdbk_lvl_cur = _fdbk_lvl_old;

	process_osc_fdbk_col (nbr_spl);

	const float    fdbk_col_c_end = _fdbk_col_c;
	const float    fdbk_col_s_end = _fdbk_col_s;

	int            pos = 0;
	do
	{
		const float    cos_a = buf_c_ptr [pos];
		const float    sin_a = buf_s_ptr [pos];
		_fdbk_col_c = buf_fc_c_ptr [pos];
		_fdbk_col_s = buf_fc_s_ptr [pos];
		process_sample (
			buf_l_ptr [pos],
			buf_r_ptr [pos],
			src_ptr [pos],
			cos_a,
			sin_a
		);
		_fdbk_lvl_old += fdbk_level_step;
		++pos;
	}
	while (pos < nbr_spl);

	_fdbk_lvl_old = _fdbk_level;
	_fdbk_col_c   = fdbk_col_c_end;
	_fdbk_col_s   = fdbk_col_s_end;
}



void	PhasedVoice::process_sample (float &dst_l, float &dst_r, float src, float cos_a, float sin_a)
{
	float          pf_in = src + _fdbk_buf;
	
	// All-pass delay
	pf_in = _apf_delay.process_sample (pf_in);

	// All-pass stage, Hilbert transform
	float				b [2];
	_phase_filter_ptr->process_sample (
		b [0],
		b [1],
		pf_in
	);

	// SSB modulation
	const float		b_0_cos = b [0] * cos_a;
	const float		b_1_sin = b [1] * sin_a;
	dst_l = b_0_cos + b_1_sin;
	dst_r = b_0_cos - b_1_sin;

	// Feedback
	const float		cos_fdbk = cos_a * _fdbk_col_c - sin_a * _fdbk_col_s;
	_fdbk_buf = src + b_0_cos + b_1_sin * cos_fdbk;
	_fdbk_buf *= _fdbk_lvl_cur;
	_fdbk_buf = clip_feedback (_fdbk_buf);
}



void	PhasedVoice::process_block_fx (float * const dst_ptr_arr [_nbr_chn_out], int nbr_spl)
{
	if (_bpf_flag && _bpf_changed_flag)
	{
		update_bpf ();
		_bpf_changed_flag = false;
	}

	for (int chn = 0; chn < _nbr_chn_out; ++chn)
	{
		if (_bpf_flag)
		{
			_bpf_arr [chn].process_block (
				dst_ptr_arr [chn],
				dst_ptr_arr [chn],
				nbr_spl
			);
		}

		if (_dist_flag)
		{
			process_block_dist_mono (dst_ptr_arr [chn], nbr_spl);
		}
	}
}



// x - x^2/4    0 < x < 2
// x + x^2/10  -5 < x <= 0
void	PhasedVoice::process_block_dist_mono (float dst_ptr [], int nbr_spl)
{
	const auto     mi   = fstb::ToolsSimd::set1_f32 (-5.0f);
	const auto     ma   = fstb::ToolsSimd::set1_f32 ( 2.0f);
	const auto     c_p  = fstb::ToolsSimd::set1_f32 (-1.0f /  4);
	const auto     c_n  = fstb::ToolsSimd::set1_f32 ( 1.0f / 10);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = fstb::ToolsSimd::load_f32 (dst_ptr + pos);

		x = fstb::ToolsSimd::min_f32 (x, ma);
		x = fstb::ToolsSimd::max_f32 (x, mi);

		const auto     t_0 = fstb::ToolsSimd::cmp_lt0_f32 (x);
		const auto     c   = fstb::ToolsSimd::select (t_0, c_n, c_p);
		fstb::ToolsSimd::mac (x, c, x * x);

		fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
	}
}



float	PhasedVoice::clip_feedback (float x)
{
	const float    xl = fstb::limit (x, -1.5f, 1.5f);
	const float    y  = xl * (1 - (4.0f / 27) * xl * xl);

	return y;
}



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
