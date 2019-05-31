/*****************************************************************************

        DistoPwm2.cpp
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
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/distpwm2/DistoPwm2.h"
#include "mfx/pi/distpwm2/Param.h"
#include "mfx/pi/distpwm2/PreFilterType.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace distpwm2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoPwm2::DistoPwm2 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_osc_arr ()
,	_filter_in ()
,	_chn_arr ()
,	_voice_arr ()
,	_prefilter (PreFilterType_WEAK)
,	_threshold (1e-4f)
,	_buf_tmp ()
,	_buf_mix_arr ()
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LPF, PreFilterType_MILD);
	_state_set.set_val_nat (desc_set, Param_THR, 1e-4);

	_state_set.add_observer (Param_LPF, _param_change_flag_misc);
	_state_set.add_observer (Param_THR, _param_change_flag_misc);

	_param_change_flag_misc.add_observer (_param_change_flag);

	for (int osc = 0; osc < OscType_NBR_ELT; ++osc)
	{
		const int      base = Param_OSC_BASE + osc * ParamOsc_NBR_ELT;
		const double   lvl  = (osc == OscType_STD) ? 1 : 0;

		_state_set.set_val_nat (desc_set, base + ParamOsc_PULSE, PulseType_RECT);
		_state_set.set_val_nat (desc_set, base + ParamOsc_LVL  , lvl);
		_state_set.set_val_nat (desc_set, base + ParamOsc_PF   , 2000);

		auto &         pcf_voice = _param_change_flag_osc_arr [osc];
		_state_set.add_observer (base + ParamOsc_PULSE, pcf_voice);
		_state_set.add_observer (base + ParamOsc_LVL  , pcf_voice);
		_state_set.add_observer (base + ParamOsc_PF   , pcf_voice);

		pcf_voice.add_observer (_param_change_flag);
	}

	dsp::mix::Align::setup ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DistoPwm2::do_get_state () const
{
	return _state;
}



double	DistoPwm2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DistoPwm2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const float    b2_s [2] = { 0, 1 };
	const float    a2_s [2] = { 1, 1 };
	float          a2_z [2];
	float          b2_z [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole (
		b2_z, a2_z, b2_s, a2_s, 30, _sample_freq
	);

	// Base: order 1, HPF 30 Hz, order 1, cutoff 1500 Hz
	const float    b3_s [3] = { 1,                   0, 0 };
	const float    a3_s [3] = { 1, float (fstb::SQRT2), 1 };
	float          a3_z [3];
	float          b3_z [3];
	dsp::iir::TransSZBilin::map_s_to_z (
		b3_z, a3_z, b3_s, a3_s, 1500, _sample_freq
	);
	_filter_in->set_z_eq_one (0, b3_z, a3_z);
	_filter_in->set_z_eq_one (1, b3_z, a3_z);

	update_prefilter ();

	const int      mbl_align = max_buf_len & -4;
	_buf_tmp.resize (mbl_align * _max_nbr_chn);
	for (auto &vcinf : _voice_arr)
	{
		vcinf._buf_gen.resize (mbl_align);
	}
	for (auto &buf : _buf_mix_arr)
	{
		buf.resize (mbl_align);
	}

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	DistoPwm2::do_clean_quick ()
{
	clear_buffers ();
}



void	DistoPwm2::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_in =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_out =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	assert (nbr_chn_in <= nbr_chn_out);
	const int      nbr_chn_proc = std::min (nbr_chn_in, nbr_chn_out);

	// Events
	for (int evt_cnt = 0; evt_cnt < proc._nbr_evt; ++evt_cnt)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [evt_cnt]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();

	// Signal processing
	const int      nbr_spl = proc._nbr_spl;

	// Interleave samples for low-pass filtering
	const int      chn_aux = (nbr_chn_proc == 2) ? 1 : 0;
	dsp::mix::Align::copy_2_2i (
		&_buf_tmp [0],
		proc._src_arr [0],
		proc._src_arr [chn_aux],
		nbr_spl
	);

	// Low-pass pre-filtering
	_filter_in->process_block_2x2_latency (
		&_buf_tmp [0],
		&_buf_tmp [0],
		nbr_spl
	);

	for (auto &vcinf : _voice_arr)
	{
		vcinf._vol_beg = float (vcinf._vol.get_val ());
		vcinf._vol.tick (nbr_spl);
		vcinf._vol_end = float (vcinf._vol.get_val ());
		vcinf._active_flag =
			(vcinf._vol_beg != 0 || vcinf._vol_beg != vcinf._vol_end);
	}

	// Zero-crossing detection and main voice generation
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		float *        tmp_ptr = &_buf_tmp [chn_index]; // 2 interleaved channels
		Channel &      chn     = _chn_arr [chn_index];

		for (int pos = 0; pos < nbr_spl; ++pos)
		{
			const float    x = tmp_ptr [pos * 2];
			const bool     positive_flag = ((chn._period_cnt & 1) != 0);
			bool           trig_flag     = false;
			if (   (  positive_flag && x <= -_threshold)
			    || (! positive_flag && x >= +_threshold))
			{
				trig_flag       = true;
				++ chn._period_cnt;
			}

			if (trig_flag)
			{
				// We need these checks because everything could go wrong when
				// _threshold is changing.
				float          zc_pos = 0;
				if (x != chn._spl_prev)
				{
					zc_pos = fstb::limit (
						(x - std::copysign (_threshold, x)) / (x - chn._spl_prev),
						0.f,
						0.99999f
					);
				}
				chn._voice_arr [OscType_OCT ].sync (zc_pos);
				if ((chn._period_cnt & 1) == 0)
				{
					chn._voice_arr [OscType_STD ].sync (zc_pos);
				}
				if ((chn._period_cnt & 3) == 1)
				{
					chn._voice_arr [OscType_SUB1].sync (zc_pos);
				}
				if ((chn._period_cnt & 7) == 3)
				{
					chn._voice_arr [OscType_SUB2].sync (zc_pos);
				}
			}

			for (int vc_index = 0; vc_index < OscType_NBR_ELT; ++vc_index)
			{
				Voice &        voice = chn._voice_arr [vc_index];
				VoiceInfo &    vcinf = _voice_arr [vc_index];
				vcinf._buf_gen [pos] = voice.process_sample ();
			}

			chn._spl_prev = x;
		}

		float *        dst_ptr = &_buf_mix_arr [chn_index] [0];
		bool           mix_flag = false;
		for (int vc_index = 0; vc_index < OscType_NBR_ELT; ++vc_index)
		{
			VoiceInfo &    vcinf = _voice_arr [vc_index];
			if (vcinf._active_flag)
			{
				const float *  src_ptr = &vcinf._buf_gen [0];
				if (mix_flag)
				{
					dsp::mix::Align::mix_1_1_vlrauto (
						dst_ptr,
						src_ptr,
						nbr_spl,
						vcinf._vol_beg,
						vcinf._vol_end
					);
				}
				else
				{
					dsp::mix::Align::copy_1_1_vlrauto (
						dst_ptr,
						src_ptr,
						nbr_spl,
						vcinf._vol_beg,
						vcinf._vol_end
					);
					mix_flag = true;
				}
			}
		}

		if (! mix_flag)
		{
			dsp::mix::Align::clear (dst_ptr, nbr_spl);
		}
	}

	// Removes the DC component
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];
		const float *  src_ptr = &_buf_mix_arr [chn_index] [0];
		float *        dst_ptr = proc._dst_arr [chn_index];
		chn._hpf_out.process_block (dst_ptr, src_ptr, proc._nbr_spl);
	}

	// Duplicates the remaining output channels
	for (int chn_index = 0; chn_index < nbr_chn_out; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoPwm2::clear_buffers ()
{
	_filter_in->clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._hpf_out.clear_buffers ();
		for (auto &voice : chn._voice_arr)
		{
			voice.clear_buffers ();
		}
		chn._period_cnt = 0;
		chn._spl_prev   = 0;
	}
	for (auto &vcinf : _voice_arr)
	{
		vcinf._active_flag = false;
		vcinf._vol.clear_buffers ();
	}
}



void	DistoPwm2::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_prefilter = PreFilterType (fstb::round_int (
				_state_set.get_val_tgt_nat (Param_LPF)
			));
			_threshold = float (_state_set.get_val_tgt_nat (Param_THR));

			update_prefilter ();
		}

		for (int vc_index = 0; vc_index < OscType_NBR_ELT; ++vc_index)
		{
			if (_param_change_flag_osc_arr [vc_index] (true) || force_flag)
			{
				const int     base  =
					Param_OSC_BASE + vc_index * ParamOsc_NBR_ELT;
				const PulseType pt  = PulseType (fstb::round_int (
					_state_set.get_val_tgt_nat (base + ParamOsc_PULSE)
				));
				const float   pf_hz =
					float (_state_set.get_val_tgt_nat (base + ParamOsc_PF));
				const float   pw    = 0.5f * _sample_freq / pf_hz;
				const float   lvl   =
					float (_state_set.get_val_end_nat (base + ParamOsc_LVL));

				VoiceInfo &   vcinf = _voice_arr [vc_index];
				vcinf._vol.set_val (lvl);

				for (auto &chn : _chn_arr)
				{
					Voice &        voice = chn._voice_arr [vc_index];
					voice.set_pulse_type (pt);
					voice.set_pulse_width (pw);
				}
			}
		}
	}
}



void	DistoPwm2::update_prefilter ()
{
	float          f_lpf    = 500;
	float          b3_s [3] = { 1, 2, 1 };
	float          a3_s [3] = { 1, 2, 1 };

	if (_prefilter >= PreFilterType_MILD)
	{
		if (_prefilter == PreFilterType_STRONG)
		{
			f_lpf = 50;
		}
		const float    f_rel_inv = f_lpf / 5.f;
		b3_s [0] = 0;
		b3_s [1] = f_rel_inv;
		b3_s [2] = 0;
		a3_s [1] = f_rel_inv + 1;
		a3_s [2] = f_rel_inv;
	}
	
	float          a3_z [3];
	float          b3_z [3];
	dsp::iir::TransSZBilin::map_s_to_z (
		b3_z, a3_z, b3_s, a3_s, f_lpf, _sample_freq
	);

	_filter_in->set_z_eq_one (2, b3_z, a3_z);
	_filter_in->set_z_eq_one (3, b3_z, a3_z);
}



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
