/*****************************************************************************

        DistoStage.cpp
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

#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/shape/WsAsym1.h"
#include "mfx/dsp/shape/WsBadmood.h"
#include "mfx/dsp/shape/WsBelt.h"
#include "mfx/dsp/shape/WsBitcrush.h"
#include "mfx/dsp/shape/WsHardclip.h"
#include "mfx/dsp/shape/WsLight1.h"
#include "mfx/dsp/shape/WsLight2.h"
#include "mfx/dsp/shape/WsLight3.h"
#include "mfx/dsp/shape/WsRcp1.h"
#include "mfx/dsp/shape/WsRcp2.h"
#include "mfx/dsp/shape/WsSqrt.h"
#include "mfx/pi/dist2/DistoStage.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoStage::DistoStage ()
:	_sbag (cdsp::ShaperBag::use ())
,	_chn_arr ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_freq_hpf_pre (30)
,	_freq_lpf_post (5000)
,	_freq_lpf_bias (500)
,	_gain_pre (1)
,	_gain_pre_old (_gain_pre)
,	_gain_post (1)
,	_gain_post_old (_gain_post)
,	_bias (0)
,	_bias_old (_bias)
,	_type (Type_ASINH)
,	_buf_x1 ()
,	_buf_ovr ()
,	_post_clear_flag (true)
{
	init_coef ();
	update_hpf_pre ();
	update_lpf_post ();
	update_lpf_bias ();
	for (auto &chn : _chn_arr)
	{
		chn._us.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._ds.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._porridge_limiter.set_time (0.001f);
		chn._porridge_limiter.set_level (1.0f);
	}
}



void	DistoStage::reset (double sample_freq, int max_block_size, double &latency)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);
	const float    fs_ovr = _sample_freq * _ovrspl;

	const int      mbs_align = (max_block_size + 3) & -4;
	_buf_x1.resize (mbs_align);
	_buf_ovr.resize (mbs_align * _ovrspl);

	// Let pass a sine of 100 Hz at 0 dB
	const float    slew_rate_limit = float (100 * 2 * fstb::PI);

	for (auto &chn : _chn_arr)
	{
		chn._porridge_limiter.set_sample_freq (fs_ovr);
		chn._attractor.set_sample_freq (fs_ovr);
		chn._random_walk.set_sample_freq (fs_ovr);
		chn._bounce.set_sample_freq (fs_ovr);
		chn._slew_rate_lim.set_sample_freq (fs_ovr);
		chn._slew_rate_lim.set_rate_limit (slew_rate_limit);
	}

	clear_buffers ();

	const double   f_fs   = 1000.0 / sample_freq;
	const double   dly_42 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_42 [0], _nbr_coef_42, f_fs * 0.25f, false
	);
	const double   dly_21 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_21 [0], _nbr_coef_21, f_fs * 0.5f , false
	);
	latency = (0.5f * dly_21 + 0.25f * dly_42) * 2;
}



void	DistoStage::set_hpf_pre_cutoff (float f)
{
	assert (f > 0);

	_freq_hpf_pre = f;
	update_hpf_pre ();
}



void	DistoStage::set_lpf_post_cutoff (float f)
{
	assert (f > 0);

	_freq_lpf_post = f;
	update_lpf_post ();
}



void	DistoStage::set_bias (float bias)
{
	_bias = 2 * bias;
}



void	DistoStage::set_bias_freq (float f)
{
	assert (f > 0);

	_freq_lpf_bias = f;
	update_lpf_bias ();
}



void	DistoStage::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



void	DistoStage::set_gain_pre (float g)
{
	assert (g >= 0);

	_gain_pre = g;
}



void	DistoStage::set_gain_post (float g)
{
	assert (g >= 0);

	_gain_post = g;
}



void	DistoStage::process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn)
{
	assert (_sample_freq > 0);
	assert (dst_ptr_arr != nullptr);
	assert (src_ptr_arr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= int (_buf_x1.size ()));
	assert (nbr_chn > 0);
	assert (nbr_chn <= _max_nbr_chn);

	if (_post_clear_flag)
	{
		set_next_block ();
		_post_clear_flag = false;
	}

	const float    bias_beg      = _bias_old;
	const float    bias_end      = _bias;
	const float    gain_pre_beg  = _gain_pre_old;
	const float    gain_pre_end  = _gain_pre;
	const float    gain_post_beg = _gain_post_old;
	const float    gain_post_end = _gain_post;

	const int      nbr_spl_ovr   = nbr_spl * _ovrspl;

	for (int chn_index = 0; chn_index < nbr_chn; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];
		float *        dst_ptr = dst_ptr_arr [chn_index];
		const float *  src_ptr = src_ptr_arr [chn_index];
		assert (fstb::DataAlign <true>::check_ptr (dst_ptr));
		assert (fstb::DataAlign <true>::check_ptr (src_ptr));

		// High-pass filtering
		chn._hpf_pre.process_block (&_buf_x1 [0], src_ptr, nbr_spl);

		// Crude envelope extraction for the bias
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto           val = fstb::ToolsSimd::load_f32 (&_buf_x1 [pos]);
			val = fstb::ToolsSimd::abs (val);
			fstb::ToolsSimd::store_f32 (&dst_ptr [pos], val);
		}
		chn._lpf_env.process_block (dst_ptr, dst_ptr, nbr_spl);

		// Bias
		dsp::mix::Align::mix_1_1_vlrauto (
			&_buf_x1 [0], dst_ptr, nbr_spl, bias_beg, bias_end
		);

		// Gain, pre
		dsp::mix::Align::scale_1_vlrauto (
			&_buf_x1 [0], nbr_spl, gain_pre_beg, gain_pre_end
		);

		// Upsampling
		chn._us.process_block (&_buf_ovr [0], &_buf_x1 [0], nbr_spl);

		// Distortion
		distort_block (chn, &_buf_ovr [0], &_buf_ovr [0], nbr_spl_ovr);

		// Downsampling
		chn._ds.process_block (&_buf_x1 [0], &_buf_ovr [0], nbr_spl);

		// Gain, post
		dsp::mix::Align::scale_1_vlrauto (
			&_buf_x1 [0], nbr_spl, gain_post_beg, gain_post_end
		);

		// Low-pass filtering
		chn._lpf_post.process_block (dst_ptr, &_buf_x1 [0], nbr_spl);
	}

	set_next_block ();
}



void	DistoStage::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._hpf_pre.clear_buffers ();
		chn._lpf_post.clear_buffers ();
		chn._lpf_env.clear_buffers ();
		chn._us.clear_buffers ();
		chn._ds.clear_buffers ();
		chn._porridge_limiter.clear_buffers ();
		chn._attractor.clear_buffers ();
		chn._random_walk.clear_buffers ();
		chn._bounce.clear_buffers ();
		chn._slew_rate_lim.clear_buffers ();
	}

	_post_clear_flag = true;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoStage::init_coef ()
{
	if (! _coef_init_flag)
	{
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_42 [0], _nbr_coef_42, 1.0 / 5
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_21 [0], _nbr_coef_21, 1.0 / 100
		);

		_coef_init_flag = true;
	}
}



void	DistoStage::set_next_block ()
{
	_gain_pre_old  = _gain_pre;
	_gain_post_old = _gain_post;
	_bias_old      = _bias;
}



void	DistoStage::update_hpf_pre ()
{
	static const float   b_s [3] = { 0, 0, 1 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_hpf_pre * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		chn._hpf_pre.set_z_eq (b_z, a_z);
	}
}



void	DistoStage::update_lpf_post ()
{
	static const float   b_s [3] = { 1, 0, 0 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float          k       =
		dsp::iir::TransSZBilin::compute_k_approx (_freq_lpf_post * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (auto &chn : _chn_arr)
	{
		chn._lpf_post.set_z_eq (b_z, a_z);
	}
}



void	DistoStage::update_lpf_bias ()
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
		chn._lpf_env.set_z_eq (b_z, a_z);
	}
}



void	DistoStage::distort_block (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	typedef fstb::DataAlign <true> DA;

	assert (DA::check_ptr (dst_ptr));
	assert (DA::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	switch (_type)
	{
	case Type_ASINH:
		distort_block_shaper (_sbag._s_asinh, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ASYM1:
		dsp::shape::WsAsym1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG1:
		distort_block_shaper (_sbag._s_prog1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG2:
		distort_block_shaper (_sbag._s_prog2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG3:
		dsp::shape::WsRcp1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SUDDEN:
		dsp::shape::WsRcp2::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_HARDCLIP:
		dsp::shape::WsHardclip::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER1:
		distort_block_shaper (_sbag._s_puncher1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER2:
		distort_block_shaper (_sbag._s_puncher2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER3:
		distort_block_shaper (_sbag._s_puncher3, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_OVERSHOOT:
		distort_block_shaper (_sbag._s_overshoot, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BITCRUSH:
		dsp::shape::WsBitcrush::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SLEWRATE:
		chn._slew_rate_lim.process_block (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_LOPSIDED:
		distort_block_shaper (_sbag._s_lopsided, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PORRIDGE:
		chn._porridge_limiter.process_block (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SMARTE1:
		distort_block_shaper (_sbag._s_smarte1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SMARTE2:
		distort_block_shaper (_sbag._s_smarte2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ATTRACT:
		chn._attractor.process_block (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_RANDWALK:
		distort_block_shaper (_sbag._s_atan, dst_ptr, src_ptr, nbr_spl);
		chn._random_walk.process_block (dst_ptr, dst_ptr, nbr_spl);
		break;
	case Type_SQRT:
		dsp::shape::WsSqrt::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BELT:
		dsp::shape::WsBelt::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BADMOOD:
		dsp::shape::WsBadmood::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BOUNCE:
		chn._bounce.process_block (dst_ptr, src_ptr, nbr_spl);
		distort_block_shaper (_sbag._s_tanh, dst_ptr, dst_ptr, nbr_spl);
		break;
	case Type_LIGHT1:
		dsp::shape::WsLight1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_LIGHT2:
		dsp::shape::WsLight2::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_LIGHT3:
		dsp::shape::WsLight3::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_TANH:
		distort_block_shaper (_sbag._s_tanh, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_TANHLIN:
		distort_block_shaper (_sbag._s_tanhlin, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BREAK:
		distort_block_shaper (_sbag._s_break, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ASYM2:
		distort_block_shaper (_sbag._s_asym2, dst_ptr, src_ptr, nbr_spl);
		break;

	default:
		assert (false);
		dsp::mix::Align::copy_1_1 (dst_ptr, src_ptr, nbr_spl);
		break;
	}
}



template <typename S>
void	DistoStage::distort_block_shaper (S &shaper, float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = shaper (src_ptr [pos]);
	}
}



bool	DistoStage::_coef_init_flag = false;
std::array <double, DistoStage::_nbr_coef_42>	DistoStage::_coef_42;
std::array <double, DistoStage::_nbr_coef_21>	DistoStage::_coef_21;



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
