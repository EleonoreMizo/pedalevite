/*****************************************************************************

        DistoSimple.cpp
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

#include "fstb/ToolsSimd.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/Param.h"
#include "mfx/piapi/EventTs.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace dist1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoSimple::DistoSimple ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (44100)
,	_inv_fs (1 / _sample_freq)
,	_gain (float (DistoSimpleDesc::_gain_min))
,	_hpf_in_freq (1)
,	_bias (0)
,	_buf_ovrspl ()
,	_chn_arr ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_GAIN    ,   1  ); // 0 dB
	_state_set.set_val_nat (desc_set, Param_HPF_FREQ, 600  );
	_state_set.set_val_nat (desc_set, Param_BIAS    ,   0.3);

	for (int index = 0; index < Param_NBR_ELT; ++index)
	{
		_state_set.add_observer (index, _param_change_flag);
	}

	_state_set.set_ramp_time (Param_GAIN    , 0.010);
	_state_set.set_ramp_time (Param_HPF_FREQ, 0.010);
	_state_set.set_ramp_time (Param_BIAS    , 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistoSimple::do_get_state () const
{
	return _state;
}



double	DistoSimple::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistoSimple::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (sample_freq);
	_inv_fs      = float (1 / sample_freq);
	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();
	update_filter_in ();

	double         coef_42 [_nbr_coef_42];
	double         coef_21 [_nbr_coef_21];
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		coef_42, _nbr_coef_42, 1.0 / 5
	);
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		coef_21, _nbr_coef_21, 1.0 / 100
	);

	static const float   b_s [3] = { 1, 0, 0 };
	static const float   a_s [3] = { 1, float (fstb::SQRT2), 1 };
	float                b_z [3];
	float                a_z [3];
	const float    env_cutoff = 500;  // Hz
	const float    k          =
		dsp::iir::TransSZBilin::compute_k_approx (env_cutoff * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (int chn = 0; chn < _max_nbr_chn; ++chn)
	{
		Channel &      c = _chn_arr [chn];
		c._hpf_in.clear_buffers ();
		c._env_lpf.set_z_eq (b_z, a_z);
		c._env_lpf.clear_buffers ();
		c._us->set_coefs (coef_42, coef_21);
		c._us->clear_buffers ();
		c._ds->set_coefs (coef_42, coef_21);
		c._ds->clear_buffers ();
		c._buf.resize (max_buf_len);
		c._buf_env.resize (max_buf_len);
	}
	_buf_ovrspl.resize (max_buf_len * _ovrspl);

	_state = State_ACTIVE;

	return Err_OK;
}



#define mfx_pi_dist1_DistoSimple_USE_MIXALIGN

void	DistoSimple::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_src = proc._nbr_chn_arr [Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);

	const int      nbr_evt = proc._nbr_evt;
	for (int index = 0; index < nbr_evt; ++index)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [index]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	const int      nbr_spl = proc._nbr_spl;

	_state_set.process_block (nbr_spl);

	bool           ramp_gain_flag = false;
	float          gain_beg = _gain;
	float          gain_end = _gain;
	float          bias_beg = _bias;
	float          bias_end = _bias;

	if (_param_change_flag (true))
	{
		ramp_gain_flag = true;
		gain_beg = float (_state_set.get_val_beg_nat (Param_GAIN));
		gain_end = float (_state_set.get_val_end_nat (Param_GAIN));
		const float       hpf_freq =
			float (_state_set.get_val_end_nat (Param_HPF_FREQ));
		if (hpf_freq != _hpf_in_freq)
		{
			_hpf_in_freq = hpf_freq;
			update_filter_in ();
		}
		bias_end = 2 * float (_state_set.get_val_end_nat (Param_BIAS));
	}

	for (int chn = 0; chn < nbr_chn_src; ++chn)
	{
		// High-pass filtering
		dsp::iir::OnePole &  hpf = _chn_arr [chn]._hpf_in;
		hpf.process_block (
			&_chn_arr [chn]._buf [0],
			&proc._src_arr [chn] [0],
			nbr_spl
		);

		// Crude envelope extraction
		for (int pos = 0; pos < nbr_spl; pos += 4)
		{
			auto           val =
				fstb::ToolsSimd::load_f32 (&_chn_arr [chn]._buf [pos]);
			val = fstb::ToolsSimd::abs (val);
			fstb::ToolsSimd::store_f32 (&_chn_arr [chn]._buf_env [pos], val);
		}
		dsp::iir::Biquad &   lpf = _chn_arr [chn]._env_lpf;
		lpf.process_block (
			&_chn_arr [chn]._buf_env [0],
			&_chn_arr [chn]._buf_env [0],
			nbr_spl
		);

		// Bias
		dsp::mix::Align::mix_1_1_vlrauto (
			&_chn_arr [chn]._buf [0],
			&_chn_arr [chn]._buf_env [0],
			nbr_spl,
			bias_beg,
			bias_end
		);
	}
	_bias = bias_end;

	// Gain (ramp)
	if (ramp_gain_flag)
	{
		if (nbr_chn_src == 1)
		{
			mfx::dsp::mix::Align::scale_1_vlr (
				&_chn_arr [0]._buf [0],
				nbr_spl,
				gain_beg,
				gain_end
			);
		}
		else
		{
			static_assert (_max_nbr_chn == 2, "Multichannel not supported");
			mfx::dsp::mix::Align::scale_2_vlr (
				&_chn_arr [0]._buf [0],
				&_chn_arr [1]._buf [0],
				nbr_spl,
				gain_beg,
				gain_end
			);
		}

		_gain = gain_end;
	}

	// Gain (constant)
	else
	{
		if (nbr_chn_src == 1)
		{
			mfx::dsp::mix::Align::scale_1_v (
				&_chn_arr [0]._buf [0],
				nbr_spl,
				_gain
			);
		}
		else
		{
			static_assert (_max_nbr_chn == 2, "Multichannel not supported");
			mfx::dsp::mix::Align::scale_2_v (
				&_chn_arr [0]._buf [0],
				&_chn_arr [1]._buf [0],
				nbr_spl,
				_gain
			);
		}
	}

	for (int chn = 0; chn < nbr_chn_src; ++chn)
	{
		Channel &      c = _chn_arr [chn];

		const float *  src_ptr = &c._buf [0];
		float *        ovr_ptr = &_buf_ovrspl [0];
		float *        dst_ptr = &proc._dst_arr [chn] [0];

#if 1
		c._us->process_block (ovr_ptr, src_ptr, nbr_spl);
		distort_block (ovr_ptr, ovr_ptr, nbr_spl * _ovrspl);
		c._ds->process_block (dst_ptr, ovr_ptr, nbr_spl);
#else
		distort_block (dst_ptr, src_ptr, nbr_spl);
#endif
	}

	for (int chn = nbr_chn_src; chn < nbr_chn_dst; ++chn)
	{
		const float *  src_ptr = &_chn_arr [0]._buf [0];
		float *        dst_ptr = &proc._dst_arr [chn] [0];
		memcpy (dst_ptr, src_ptr, sizeof (*dst_ptr) * nbr_spl);
	}
}



void	DistoSimple::update_filter_in ()
{
	static const float   b_s [2] = { 0, 1 };
	static const float   a_s [2] = { 1, 1 };
	float                b_z [2];
	float                a_z [2];
	const float    k       =
		dsp::iir::TransSZBilin::compute_k_approx (_hpf_in_freq * _inv_fs);
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		b_z, a_z,
		b_s, a_s,
		k
	);

	for (int chn = 0; chn < _max_nbr_chn; ++chn)
	{
		dsp::iir::OnePole &  hpf = _chn_arr [chn]._hpf_in;
		hpf.set_z_eq (b_z, a_z);
	}
}



// x -> { x - x^9/9 if x >  0
//      { x + x^2/2 if x <= 0
// x * (1 - x^8/9)
//
// With an attenuation:
// x -> x - (a^(p-1) / p) * x^p
// p = power
// a = attenuation (inverse of gain). 8 = -18 dB
void	DistoSimple::distort_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	const auto     mi   = fstb::ToolsSimd::set1_f32 (-1.0f / _attn);
	const auto     ma   = fstb::ToolsSimd::set1_f32 ( 1.0f / _attn);
	const auto     zero = fstb::ToolsSimd::set_f32_zero ();
	const auto     c_9  = fstb::ToolsSimd::set1_f32 (_m_9);
	const auto     c_2  = fstb::ToolsSimd::set1_f32 (_m_2);
	const auto     bias = fstb::ToolsSimd::set1_f32 ( 0.2f / _attn);

	for (int pos = 0; pos < nbr_spl; pos += 4)
	{
		auto           x = fstb::ToolsSimd::load_f32 (src_ptr + pos);

		x += bias;

		x = fstb::ToolsSimd::min_f32 (x, ma);
		x = fstb::ToolsSimd::max_f32 (x, mi);

		const auto     x2  = x  * x;
		const auto     x4  = x2 * x2;
		const auto     x8  = x4 * x4;
		const auto     x9  = x8 * x;
		const auto     x_n = x + x2 * c_2;
		const auto     x_p = x - x9 * c_9;
		const auto     t_0 = fstb::ToolsSimd::cmp_gt_f32 (x, zero);
		x = fstb::ToolsSimd::select (t_0, x_p, x_n);

		x -= bias;

		fstb::ToolsSimd::store_f32 (dst_ptr + pos, x);
	}
}



const float	DistoSimple::_attn = 8;
const float	DistoSimple::_m_9  = fstb::ipowp (_attn, 9 - 1) / 9;
const float	DistoSimple::_m_2  = fstb::ipowp (_attn, 2 - 1) / 2;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
