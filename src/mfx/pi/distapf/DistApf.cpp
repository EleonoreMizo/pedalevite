/*****************************************************************************

        DistApf.cpp
        Author: Laurent de Soras, 2019

Approximation of the bilinear transform for 1st-order all-pass filter
coefficients:
u         = pi * f0 / fs
k_inv     = tan (u)
b0_real   = (k_inv - 1) / (k_inv + 1)
q         = u - pi / 4
b0_approx = q + 0.3345 * q^3 + 0.121 * q^5 + 0.0884 * q^7
Relative error < 0.9e-4
Polynomial found manually, could be refined, esp. near f0 = 0.

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
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/distapf/DistApf.h"
#include "mfx/pi/distapf/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace distapf
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistApf::DistApf ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_ovrspl ()
,	_sample_freq (44100)
,	_inv_fs (1 / _sample_freq)
,	_chn_arr ()
,	_buf_tmp ()
,	_buf_ovr ()
,	_gain_cur (1)
,	_gain_old (_gain_cur)
,	_map_a (0)
,	_map_b (0)
,	_freq_scale (0)
,	_srl (0)
,	_ovrspl_flag (false)
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_GAIN,    1);
	_state_set.set_val_nat (desc_set, Param_SRL , 2000);
	_state_set.set_val_nat (desc_set, Param_OVRS,    0);

	_state_set.add_observer (Param_GAIN, _param_change_flag_misc);
	_state_set.add_observer (Param_SRL , _param_change_flag_misc);
	_state_set.add_observer (Param_OVRS, _param_change_flag_ovrspl);

	_param_change_flag_misc.add_observer (_param_change_flag);
	_param_change_flag_ovrspl.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_GAIN, 0.010);

	init_coef ();
	for (auto &chn : _chn_arr)
	{
		chn._ovrspl_up.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._ovrspl_dw.set_coefs (&_coef_42 [0], &_coef_21 [0]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistApf::do_get_state () const
{
	return _state;
}



double	DistApf::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistApf::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	_sample_freq = float (sample_freq);
	_inv_fs      = float (1 / sample_freq);

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_tmp.resize (mbs_align * _ovrspl_ratio);
	_buf_ovr.resize (mbs_align * _ovrspl_ratio);

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	latency = 0;

	return piapi::Err_OK;
}



void	DistApf::do_clean_quick ()
{
	clear_buffers ();
}



void	DistApf::do_process_block (piapi::ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._nbr_chn_arr [piapi::Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [piapi::Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);

	// Events
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

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param ();

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Audio processing

	const auto     mapa    = fstb::ToolsSimd::set1_f32 (_map_a);
	const auto     mapb    = fstb::ToolsSimd::set1_f32 (_map_b);
	const auto     fmin    = fstb::ToolsSimd::set1_f32 (-0.5f);
	const auto     fmax    = fstb::ToolsSimd::set1_f32 (+0.5f);
	const auto     c7      = fstb::ToolsSimd::set1_f32 (0.0884f);
	const auto     c5      = fstb::ToolsSimd::set1_f32 (0.1210f);
	const auto     c3      = fstb::ToolsSimd::set1_f32 (0.3345f);
	const auto     limn    = fstb::ToolsSimd::set1_f32 (-0.999f);
	const auto     limp    = fstb::ToolsSimd::set1_f32 (+0.999f);
	float * const  tmp_ptr = _buf_tmp.data ();

	int            nbr_spl_proc = nbr_spl;
	if (_ovrspl_flag)
	{
		nbr_spl_proc *= _ovrspl_ratio;
	}

	fstb::ToolsSimd::VectF32   gain_beg;
	fstb::ToolsSimd::VectF32   gain_step;
	fstb::ToolsSimd::start_lerp (
		gain_beg, gain_step, _gain_old, _gain_cur, nbr_spl_proc
	);

	for (int chn_cnt = 0; chn_cnt < nbr_chn_src; ++chn_cnt)
	{
		Channel &      chn     = _chn_arr [chn_cnt];

		const float *  src_ptr = proc._src_arr [chn_cnt];
		float *        dst_ptr = proc._dst_arr [chn_cnt];

		if (_ovrspl_flag)
		{
			chn._ovrspl_up.process_block (
				_buf_ovr.data (), proc._src_arr [chn_cnt], nbr_spl
			);

			dst_ptr = _buf_ovr.data ();
			src_ptr = dst_ptr;
		}

		// Modulator: gain and clipping
		auto           gain = gain_beg;
		for (int pos = 0; pos < nbr_spl_proc; pos += 4)
		{
			auto           f = fstb::ToolsSimd::load_f32 (src_ptr + pos);
			f *= gain;
			f  = fstb::ToolsSimd::min_f32 (f, fmax);
			f  = fstb::ToolsSimd::max_f32 (f, fmin);
			fstb::ToolsSimd::store_f32 (tmp_ptr + pos, f);
			gain += gain_step;
		}

		// Modulator: slew rate limiting
		for (int pos = 0; pos < nbr_spl_proc; ++pos)
		{
			float          f   = _buf_tmp [pos];
			float          dif = f - chn._srl_state;
			dif = fstb::limit (dif, -_srl, _srl);
			f   = chn._srl_state + dif;
			chn._srl_state = f;

#if 1
			_buf_tmp [pos] = f;
		}

		// Modulator: all-pass filter coefficient modulation
		for (int pos = 0; pos < nbr_spl_proc; pos += 4)
		{
			auto           f = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
			f *= mapa;
			f += mapb;
			const auto     f2 = f * f;
			const auto     f3 = f * f2;
			auto           b0 = c7;
			b0 *= f2;
			b0 += c5;
			b0 *= f2;
			b0 += c3;
			b0 *= f3;
			b0 += f;
			b0 = fstb::ToolsSimd::min_f32 (b0, limp);
			b0 = fstb::ToolsSimd::max_f32 (b0, limn);
			fstb::ToolsSimd::store_f32 (tmp_ptr + pos, b0);
		}

		for (int pos = 0; pos < nbr_spl_proc; ++pos)
		{
			const float    x  = src_ptr [pos];
			const float    b0 = tmp_ptr [pos];

#else // Reference code

			// Modulator: all-pass filter coefficient modulation
			f  = fstb::limit (f, -0.5f, +0.5f);
			f *= _map_a;
			f += _map_b;
			const float    f2 = f * f;
			float          b0 =
				((0.0884f * f2 + 0.121f) * f2 + 0.3345f) * f2 * f + f;
			b0 = fstb::limit (b0, -0.999f, +0.999f);

#endif

			// All-pass filtering
			const float    y = b0 * (x - chn._apf_mem_y) + chn._apf_mem_x;
			chn._apf_mem_x = x;
			chn._apf_mem_y = y;

			dst_ptr [pos]  = y;
		}

		if (_ovrspl_flag)
		{
			chn._ovrspl_dw.process_block (
				proc._dst_arr [chn_cnt], dst_ptr, nbr_spl
			);
		}
	}

	_gain_old = _gain_cur;

	// Additional output channels
	for (int chn_cnt = nbr_chn_src; chn_cnt < nbr_chn_dst; ++chn_cnt)
	{
		const float *  src_ptr = proc._dst_arr [0      ];
		float *        dst_ptr = proc._dst_arr [chn_cnt];
		dsp::mix::Align::copy_1_1 (dst_ptr, src_ptr, nbr_spl);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistApf::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_ovrspl (true) || force_flag)
		{
			// This param first, so _map_a, _map_b and _srl use up-to-date values
			_ovrspl_flag = (_state_set.get_val_end_nat (Param_OVRS) >= 0.5f);
			update_freq ();
			_param_change_flag_misc.set ();
		}

		if (_param_change_flag_misc (true) || force_flag)
		{
			_gain_cur = float (_state_set.get_val_end_nat (Param_GAIN));

			_map_a = (_freq_max - _freq_min) * _freq_scale;
			_map_b =   _freq_scale * (_freq_min + (_freq_max - _freq_min) * 0.5f)
			         - float (fstb::PI * 0.25f);

			const float    srl_hz =
				float (_state_set.get_val_end_nat (Param_SRL));
			_srl = srl_hz * _srl_scale;
		}
	}
}



void	DistApf::update_freq ()
{
	float          fs     = _sample_freq;
	float          fs_inv = _inv_fs;
	if (_ovrspl_flag)
	{
		fs     *=               _ovrspl_ratio;
		fs_inv *= 1.0f / float (_ovrspl_ratio);
	}

	_srl_scale  = fs_inv;
	_freq_scale = float (fstb::PI) * fs_inv;
}



void	DistApf::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._srl_state = 0;
		chn._apf_mem_x = 0;
		chn._apf_mem_y = 0;
		chn._ovrspl_up.clear_buffers ();
		chn._ovrspl_dw.clear_buffers ();
	}
	_gain_old = _gain_cur;
}



void	DistApf::init_coef ()
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



bool	DistApf::_coef_init_flag = false;
std::array <double, DistApf::_nbr_coef_42>	DistApf::_coef_42;
std::array <double, DistApf::_nbr_coef_21>	DistApf::_coef_21;



}  // namespace distapf
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
