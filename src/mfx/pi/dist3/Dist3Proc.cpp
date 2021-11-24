/*****************************************************************************

        Dist3Proc.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/shape/WsSmthMax0.h"
#include "mfx/pi/dist3/Dist3Proc.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Dist3Proc::Dist3Proc ()
:	_sample_freq (0)
,	_inv_fs (0)
,	_chn_arr ()
,	_freq_hpf_pre (20)
,	_freq_lpf_bias (100)
,	_freq_lpf_post (20000)
,	_gain_pre ()
,	_gain_post ()
,	_bias_s ()
,	_bias_d ()
,	_bias_fdbk_flag (false)
,	_class_b_flag (false)
,	_psu_flag (false)
,	_hpf_pre_dirty_flag (true)
,	_dck_coef_arr ()
,	_buf_x1 ()
,	_buf_bias ()
,	_buf_ovr_arr ()
{
	_gain_pre.force_val (1);
	_gain_post.force_val (1);

	init_ovrs_coef ();
	for (auto &chn : _chn_arr)
	{
		chn._us.set_coefs (_coef_42.data (), _coef_21.data ());
		chn._ds.set_coefs (_coef_42.data (), _coef_21.data ());
		chn._psu.set_half_cycle_relative_lvl (0.5f);
		chn._psu.set_ac_freq (50);
		chn._psu.set_ac_lvl (10);
	}
}



void	Dist3Proc::reset (double sample_freq, int max_block_size, double &latency)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	const double   fs_ovr = sample_freq * _ovrspl;

	const int      mbs_align = (max_block_size + 3) & -4;
	_buf_x1.resize (mbs_align);
	_buf_bias.resize (mbs_align);
	for (auto &buf : _buf_ovr_arr)
	{
		buf.resize (mbs_align * _ovrspl);
	}

	update_dc_killer ();
	update_hpf_srl ();

	double         latency_ovr = 0;
	for (auto &chn : _chn_arr)
	{
		chn._srl_pre.set_sample_freq (fs_ovr);
		for (auto &shp : chn._shaper_arr)
		{
			shp.reset (fs_ovr, max_block_size, latency_ovr);
		}
		chn._psu.set_sample_freq (fs_ovr);
		chn._srl_post.set_sample_freq (fs_ovr);
		chn._bias_dly.setup (_fdbk_dly, max_block_size);
		chn._bias_dly.set_delay (_fdbk_dly);
	}
	latency = latency_ovr / _ovrspl;

	const double   f_fs   = 1000.0 / sample_freq;
	const double   dly_42 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_42 [0], _nbr_coef_42, f_fs * 0.25f, false
	);
	const double   dly_21 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_21 [0], _nbr_coef_21, f_fs * 0.5f , false
	);
	latency += (0.5f * dly_21 + 0.25f * dly_42) * 2;

	clear_buffers ();
}



void	Dist3Proc::Dist3Proc::set_freq_hpf_pre (float f)
{
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_freq_hpf_pre       = f;
	_hpf_pre_dirty_flag = true;
}



void	Dist3Proc::Dist3Proc::set_freq_lpf_bias (float f)
{
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_freq_lpf_bias = f;
	update_lpf_bias ();
}



void	Dist3Proc::Dist3Proc::set_freq_lpf_post (float f)
{
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_freq_lpf_post = f;
	update_lpf_post ();
}



void	Dist3Proc::set_slew_rate_pre (float srl_hz)
{
	assert (srl_hz > 0);

	const float    rate_max_s = conv_srl_hz_to_s (srl_hz);
	for (auto &chn : _chn_arr)
	{
		chn._srl_pre.set_rate_limit (rate_max_s);
	}
}



void	Dist3Proc::set_slew_rate_post (float srl_hz)
{
	assert (srl_hz > 0);

	const float    rate_max_s = conv_srl_hz_to_s (srl_hz);
	for (auto &chn : _chn_arr)
	{
		chn._srl_post.set_rate_limit (rate_max_s);
	}
}



void	Dist3Proc::Dist3Proc::set_gain (float gain)
{
	assert (gain > 0);

	const float    scale    = 8;
	const float    scale_i  = 1.f / scale;
	const float    gain_fix = (gain < 1) ? 1 / gain : 1;

	_gain_pre.set_val (gain * scale);
	_gain_post.set_val (gain_fix * scale_i);
	_hpf_pre_dirty_flag = true;
}



void	Dist3Proc::set_type (Shaper::Type type)
{
	assert (type >= 0);
	assert (type < Shaper::Type_NBR_ELT);

	for (auto &chn : _chn_arr)
	{
		for (auto &shp : chn._shaper_arr)
		{
			shp.set_type (type);
		}
	}
}



void	Dist3Proc::Dist3Proc::set_bias_s (float bs)
{
	assert (bs >= 0);

	_bias_s.set_val (bs);
}



void	Dist3Proc::Dist3Proc::set_bias_d (float bd)
{
	assert (bd >= 0);

	_bias_d.set_val (bd);
}



void	Dist3Proc::Dist3Proc::set_bias_fdbk (bool fdbk_flag)
{
	_bias_fdbk_flag = fdbk_flag;
}



void	Dist3Proc::Dist3Proc::set_class_b (bool b_flag)
{
	_class_b_flag = b_flag;
}



void	Dist3Proc::enable_psu (bool flag)
{
	_psu_flag = flag;
}



void	Dist3Proc::set_psu_ac_freq (float f)
{
	for (auto &chn : _chn_arr)
	{
		chn._psu.set_ac_freq (f);
	}
}



void	Dist3Proc::set_psu_load (float load)
{
	assert (load > 0);

	const float    lvl = 1.f / load;
	for (auto &chn : _chn_arr)
	{
		chn._psu.set_ac_lvl (lvl);
	}
}



void	Dist3Proc::process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	assert (_sample_freq > 0);
	assert (dst_ptr_arr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= int (_buf_x1.size ()));
	assert (nbr_chn > 0);
	assert (nbr_chn <= _max_nbr_chn);

	if (_bias_fdbk_flag)
	{
		std::array <      float *, _max_nbr_chn> dst2_ptr_arr {};
		std::array <const float *, _max_nbr_chn> src2_ptr_arr {};
		int            pos = 0;
		do
		{
			for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
			{
				dst2_ptr_arr [chn_cnt] = dst_ptr_arr [chn_cnt] + pos;
				src2_ptr_arr [chn_cnt] = src_ptr_arr [chn_cnt] + pos;
			}
			int         work_len = _fdbk_dly;
			work_len = std::min (work_len, nbr_spl - pos);

			process_sub_block (
				dst2_ptr_arr.data (), src2_ptr_arr.data (), work_len, nbr_chn
			);

			pos += work_len;
		}
		while (pos < nbr_spl);
	}

	else
	{
		process_sub_block (dst_ptr_arr, src_ptr_arr, nbr_spl, nbr_chn);
	}
}



void	Dist3Proc::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._hpf_pre.clear_buffers ();
		chn._lpf_bias.clear_buffers ();
		chn._srl_pre.clear_buffers ();
		chn._hpf_srl.clear_buffers ();
		chn._us_bias.clear_buffers ();
		chn._us.clear_buffers ();
		for (auto &shp : chn._shaper_arr)
		{
			shp.clear_buffers ();
		}
		chn._psu.clear_buffers ();
		chn._ds.clear_buffers ();
		chn._srl_post.clear_buffers ();
		chn._lpf_post.clear_buffers ();
		chn._bias_dly.clear_buffers_quick ();
	}

	_gain_pre.clear_buffers ();
	_gain_post.clear_buffers ();
	_bias_s.clear_buffers ();
	_bias_d.clear_buffers ();

	_hpf_pre_dirty_flag = true;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Dist3Proc::update_dc_killer ()
{
	const float          freq    = 5.f;

	static const float   b_s [2] = { 0, 1 };
	static const float   a_s [2] = { 1, 1 };
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		_dck_coef_arr [0].data (), _dck_coef_arr [1].data (),
		b_s, a_s,
		k
	);
}



void	Dist3Proc::update_hpf_pre ()
{
	float                gain    = _gain_pre.get_tgt ();
	const float          lvl     = 1.f / std::max (gain, 1.f);
	const float          b_s [2] = { lvl, 1 };
	static const float   a_s [2] = {   1, 1 };
	float                b1_z [2];
	float                a1_z [2];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_hpf_pre * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		b1_z, a1_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		set_z_coef_w_dc_kill (chn._hpf_pre, b1_z, a1_z);
	}
}



void	Dist3Proc::update_lpf_post ()
{
	static const float   b_s [2] = { 1, 0 };
	static const float   a_s [2] = { 1, 1 };
	float                b1_z [2];
	float                a1_z [2];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_lpf_post * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		b1_z, a1_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		set_z_coef_w_dc_kill (chn._lpf_post, b1_z, a1_z);
	}
}



void	Dist3Proc::update_lpf_bias ()
{
	static const float   b_s [3] = { 1, 0, 0 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_lpf_bias * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		chn._lpf_bias.set_z_eq (b_z, a_z);
	}
}



void	Dist3Proc::update_hpf_srl ()
{
	const float          freq    = 10.f;

	static const float   b_s [3] = { 0, 0, 1 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		chn._hpf_srl.set_z_eq (b_z, a_z);
	}
}



void	Dist3Proc::set_z_coef_w_dc_kill (dsp::iir::Biquad &biq, const float b1_z [2], const float a1_z [2])
{
	const float    b_z [3] =
	{
		b1_z [0] * _dck_coef_arr [0] [0],
		b1_z [0] * _dck_coef_arr [0] [1] + b1_z [1] * _dck_coef_arr [0] [0],
		b1_z [1] * _dck_coef_arr [0] [1]
	};
	const float    a_z [3] =
	{
		a1_z [0] * _dck_coef_arr [1] [0],
		a1_z [0] * _dck_coef_arr [1] [1] + a1_z [1] * _dck_coef_arr [1] [0],
		a1_z [1] * _dck_coef_arr [1] [1]
	};
	biq.set_z_eq (b_z, a_z);
}



void	Dist3Proc::process_sub_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	assert (_sample_freq > 0);
	assert (dst_ptr_arr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= int (_buf_x1.size ()));
	assert (! _bias_fdbk_flag || nbr_spl <= _fdbk_dly);
	assert (nbr_chn > 0);
	assert (nbr_chn <= _max_nbr_chn);

	_gain_pre.tick (nbr_spl);
	_gain_post.tick (nbr_spl);
	_bias_s.tick (nbr_spl);
	_bias_d.tick (nbr_spl);

	if (_hpf_pre_dirty_flag)
	{
		update_hpf_pre ();
		_hpf_pre_dirty_flag = false;
	}

	const int      nbr_spl_ovr = nbr_spl * _ovrspl;

	for (int chn_index = 0; chn_index < nbr_chn; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];
		float *        dst_ptr = dst_ptr_arr [chn_index];
		const float *  src_ptr = src_ptr_arr [chn_index];
		assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
		assert (fstb::DataAlign <true>::check_ptr (src_ptr));

		// DC removal and high-pass filtering
		chn._hpf_pre.process_block (_buf_x1.data (), src_ptr, nbr_spl);

		// Computes the bias
		if (_bias_fdbk_flag)
		{
			chn._bias_dly.pop_block (dst_ptr, nbr_spl);
			compute_bias (chn, dst_ptr, nbr_spl);
		}
		else
		{
			compute_bias (chn, _buf_x1.data (), nbr_spl);
		}

		// Gain, pre
		dsp::mix::Align::scale_1_vlrauto (
			_buf_x1.data (),
			nbr_spl,
			_gain_pre.get_beg (),
			_gain_pre.get_end ()
		);

		// Upsampling
		chn._us.process_block (
			_buf_ovr_arr [0].data (), _buf_x1.data (), nbr_spl
		);
		chn._us_bias.process_block (
			_buf_ovr_arr [1].data (), _buf_bias.data (), nbr_spl
		);

		// Slew rate limiting
		chn._srl_pre.process_block (
			_buf_ovr_arr [0].data (), _buf_ovr_arr [0].data (), nbr_spl_ovr
		);
		chn._hpf_srl.process_block (
			_buf_ovr_arr [0].data (), _buf_ovr_arr [0].data (), nbr_spl_ovr
		);

		// Distortion
		process_dist (chn, nbr_spl_ovr);

		// Slew rate limiting
		chn._srl_post.process_block (
			_buf_ovr_arr [0].data (), _buf_ovr_arr [0].data (), nbr_spl_ovr
		);

		// PSU loading
		if (_psu_flag)
		{
			chn._psu.process_block (
				_buf_ovr_arr [0].data (), _buf_ovr_arr [0].data (), nbr_spl_ovr
			);
		}

		// Downsampling
		chn._ds.process_block (
			_buf_x1.data (), _buf_ovr_arr [0].data (), nbr_spl
		);

		// Gain, post
		dsp::mix::Align::scale_1_vlrauto (
			_buf_x1.data (),
			nbr_spl,
			_gain_post.get_beg (),
			_gain_post.get_end ()
		);

		// Bias feedback
		if (_bias_fdbk_flag)
		{
			chn._bias_dly.push_block (_buf_x1.data (), nbr_spl);
		}

		// Low-pass filtering and DC removal
		chn._lpf_post.process_block (dst_ptr, _buf_x1.data (), nbr_spl);
	}
}



// Result in _buf_bias
void	Dist3Proc::compute_bias (Channel &chn, const float src_ptr [], int nbr_spl)
{
	assert (fstb::DataAlign <true>::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	// Crude envelope extraction
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           val = fstb::ToolsSimd::load_f32 (&src_ptr [pos]);
		val = fstb::abs (val);
		fstb::ToolsSimd::store_f32 (&_buf_bias [pos], val);
	}
	chn._lpf_bias.process_block (
		_buf_bias.data (), _buf_bias.data (), nbr_spl
	);

	// bias final = env * bias_d * gain + bias_s
	fstb::Vf32     bs_cur;
	fstb::Vf32     bs_step;
	fstb::ToolsSimd::start_lerp (
		bs_cur, bs_step, _bias_s.get_beg (), _bias_s.get_end (), nbr_spl
	);
	float          bd_beg = -_bias_d.get_beg ();
	float          bd_end = -_bias_d.get_end ();
	if (_class_b_flag || ! _bias_fdbk_flag)
	{
		bd_beg *= _gain_pre.get_beg ();
		bd_end *= _gain_pre.get_end ();
	}
	if (! _class_b_flag)
	{
		const float    class_a_scale = (_bias_fdbk_flag) ? 16.f : 4.f;
		bd_beg *= class_a_scale;
		bd_end *= class_a_scale;
	}
	fstb::Vf32     bd_cur;
	fstb::Vf32     bd_step;
	fstb::ToolsSimd::start_lerp (bd_cur, bd_step, bd_beg, bd_end, nbr_spl);
	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           val = fstb::ToolsSimd::load_f32 (&_buf_bias [pos]);
		val *= bd_cur;
		val += bs_cur;
		bs_cur += bs_step;
		bd_cur += bd_step;
		fstb::ToolsSimd::store_f32 (&_buf_bias [pos], val);
	}
}



// On input, oversampled buffer 0 contains the input, and buffer 1 the bias
// signal.
// Results is stored into buffer 0
void	Dist3Proc::process_dist (Channel &chn, int nbr_spl)
{
	assert (nbr_spl > 0);

	std::array <float *, 2> buf_arr =
	{{
		_buf_ovr_arr [0].data (),
		_buf_ovr_arr [1].data ()
	}};

	// Class B
	// y = dist (max0 (bias + x)) + dist (-max0 (bias - x))
	if (_class_b_flag)
	{
		// buf 0 and 1 collect bias - x and bias + x
		dsp::mix::Align::add_sub_ip_2_2 (
			buf_arr [1],
			buf_arr [0],
			nbr_spl
		);

		typedef dsp::shape::WsSmthMax0 <std::ratio <1, 8> > Max0;

		// buf 1: max0 (bias + x)
		Max0::process_block <
			fstb::DataAlign <true>,
			fstb::DataAlign <true>
		> (buf_arr [1], buf_arr [1], nbr_spl);

		// buf 0: max0 (bias - x)
		Max0::process_block <
			fstb::DataAlign <true>,
			fstb::DataAlign <true>
		> (buf_arr [0], buf_arr [0], nbr_spl);

		// Negates buf 0: -max0 (bias - x)
		const auto  sign_mask = fstb::Vf32::signbit_mask ();
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto        x = fstb::ToolsSimd::load_f32 (buf_arr [0] + pos);
			x ^= sign_mask;
			fstb::ToolsSimd::store_f32 (buf_arr [0] + pos, x);
		}

		// dist() functions
		for (int s = 0; s < 2; ++s)
		{
			chn._shaper_arr [s].process_block (buf_arr [s], buf_arr [s], nbr_spl);
		}

		// Sums both parts into buf 0
		dsp::mix::Align::mix_2_1 (
			buf_arr [0], buf_arr [0], buf_arr [1], nbr_spl
		);
	}

	// Class A
	// dist = dist (b + x)
	else
	{
		// Combines input and bias
		dsp::mix::Align::mix_2_1 (
			buf_arr [0], buf_arr [0], buf_arr [1], nbr_spl
		);

		// Distorts
		chn._shaper_arr [0].process_block (buf_arr [0], buf_arr [0], nbr_spl);
	}
}



float	Dist3Proc::conv_srl_hz_to_s (float f)
{
	assert (f > 0);

	return f;
}



void	Dist3Proc::init_ovrs_coef ()
{
	if (! _coef_init_flag)
	{
		// Rejection        : 77.1 dB
		// Bandwidth        : 21.3 kHz @ 44.1 kHz
		// Total group delay:  4.0 spl @ 1 kHz
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_42 [0], _nbr_coef_42, 0.225513
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_21 [0], _nbr_coef_21, 0.0159287
		);

		_coef_init_flag = true;
	}
}



bool	Dist3Proc::_coef_init_flag = false;
std::array <double, Dist3Proc::_nbr_coef_42>	Dist3Proc::_coef_42;
std::array <double, Dist3Proc::_nbr_coef_21>	Dist3Proc::_coef_21;



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
