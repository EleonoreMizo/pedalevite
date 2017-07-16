/*****************************************************************************

        ColorMe.cpp
        Author: Laurent de Soras, 2017

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

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/iir/Svf2p.h"
#include "mfx/pi/colorme/Param.h"
#include "mfx/pi/colorme/ColorMe.h"
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
namespace colorme
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ColorMe::ColorMe ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_vow ()
,	_param_change_flag_vow_misc ()
,	_param_change_flag_vow_type ()
,	_chn_arr (_max_nbr_chn)
,	_vow_morph (0)
,	_vow_reso (10)
,	_vow_q (10)
,	_vow_transp (1)
,	_vow_nbr_vow (2)
,	_vow_nbr_formants (Cst::_nbr_formants)
,	_vow_desc_arr ()
,	_vow_vol_fix (1)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_VOW_MORPH   , 0);
	_state_set.set_val_nat (desc_set, Param_VOW_RESO    , 10);
	_state_set.set_val_nat (desc_set, Param_VOW_Q       , 10);
	_state_set.set_val_nat (desc_set, Param_VOW_TRANSP  , 0);
	_state_set.set_val_nat (desc_set, Param_VOW_NBR_FORM, Cst::_nbr_formants);
	_state_set.set_val_nat (desc_set, Param_VOW_NBR_VOW , 2);

	_state_set.add_observer (Param_VOW_MORPH   , _param_change_flag_vow_misc);
	_state_set.add_observer (Param_VOW_RESO    , _param_change_flag_vow_misc);
	_state_set.add_observer (Param_VOW_Q       , _param_change_flag_vow_misc);
	_state_set.add_observer (Param_VOW_TRANSP  , _param_change_flag_vow_misc);
	_state_set.add_observer (Param_VOW_NBR_FORM, _param_change_flag_vow_misc);
	_state_set.add_observer (Param_VOW_NBR_VOW , _param_change_flag_vow_type);

	_state_set.set_ramp_time (Param_VOW_MORPH , 0.010);
	_state_set.set_ramp_time (Param_VOW_RESO  , 0.010);
	_state_set.set_ramp_time (Param_VOW_Q     , 0.010);
	_state_set.set_ramp_time (Param_VOW_TRANSP, 0.010);

	for (int index = 0; index < Cst::_nbr_vow_morph; ++index)
	{
		const int      base = Param_VOW_LIST + index * ParamVowel_NBR_ELT;
		_state_set.set_val_nat (desc_set, base + ParamVowel_TYPE, index);
		_state_set.set_val_nat (desc_set, base + ParamVowel_SET ,     0);
		_state_set.add_observer (base + ParamVowel_TYPE, _param_change_flag_vow_type);
		_state_set.add_observer (base + ParamVowel_SET , _param_change_flag_vow_type);
	}

	_param_change_flag_vow_misc.add_observer (_param_change_flag_vow);
	_param_change_flag_vow_type.add_observer (_param_change_flag_vow);

	_param_change_flag_vow.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	ColorMe::do_get_state () const
{
	return _state;
}



double	ColorMe::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	ColorMe::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &chn : _chn_arr)
	{
		chn._formant_filter.neutralise ();
	}

	_param_change_flag_vow_misc.set ();
	_param_change_flag_vow_type.set ();

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	ColorMe::do_clean_quick ()
{
	clear_buffers ();
}



void	ColorMe::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_src =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_dst =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

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
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn = _chn_arr [chn_index];

		chn._formant_filter.process_block_ser_imm (
			proc._dst_arr [chn_index],
			proc._src_arr [chn_index],
			proc._nbr_spl
		);
		dsp::mix::Align::scale_1_v (
			proc._dst_arr [chn_index],
			proc._nbr_spl,
			_vow_vol_fix
		);
	}

	// Duplicates the remaining output channels
	for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ColorMe::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._formant_filter.clear_buffers ();
	}
}



void	ColorMe::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_vow (true) || force_flag)
		{
			if (_param_change_flag_vow_misc (true) || force_flag)
			{
				_vow_morph = float (_state_set.get_val_end_nat (Param_VOW_MORPH));
				_vow_reso  = float (_state_set.get_val_end_nat (Param_VOW_RESO));
				_vow_q     = float (_state_set.get_val_end_nat (Param_VOW_Q));
				const float   transp =
					float (_state_set.get_val_end_nat (Param_VOW_TRANSP));
				_vow_transp = fstb::Approx::exp2 (transp * (1.0f / 12));
				_vow_nbr_formants = fstb::round_int (
					_state_set.get_val_tgt_nat (Param_VOW_NBR_FORM)
				);
			}

			if (_param_change_flag_vow_type (true) || force_flag)
			{
				_vow_nbr_vow = fstb::round_int (
					_state_set.get_val_tgt_nat (Param_VOW_NBR_VOW)
				);
				for (int index = 0; index < Cst::_nbr_vow_morph; ++index)
				{
					const int      base = Param_VOW_LIST + index * ParamVowel_NBR_ELT;
					VowelDesc &    desc = _vow_desc_arr [index];

					desc._type = fstb::round_int (
						_state_set.get_val_tgt_nat (base + ParamVowel_TYPE)
					);
					desc._set  = fstb::round_int (
						_state_set.get_val_tgt_nat (base + ParamVowel_SET )
					);
				}
			}

			update_formants ();
		}
	}
}



void	ColorMe::update_formants ()
{
	const float    k   = 1.0f / _vow_q;
	const float    v1m = (_vow_reso - 1) * k;

	const float    morph  = _vow_morph * (_vow_nbr_vow - 1);
	const int      v0_idx = fstb::floor_int (morph);
	const float    m_pos  = morph - v0_idx;

	const int         idx_max = Cst::_nbr_vow_morph - 1;
	const int         v1_idx  = std::min (v0_idx + 1, idx_max);
	const VowelDesc & desc_0  = _vow_desc_arr [v0_idx];
	const VowelDesc & desc_1  = _vow_desc_arr [v1_idx];
	const Vowel &     vow_0   = _voice_tone_arr [desc_0._set] [desc_0._type];
	const Vowel &     vow_1   = _voice_tone_arr [desc_1._set] [desc_1._type];

	for (int f_index = 0; f_index < _vow_nbr_formants; ++f_index)
	{
		const float    f0 = vow_0 [f_index];
		const float    f1 = vow_1 [f_index];
		const float    f  = fstb::lerp (f0, f1, m_pos) * _vow_transp;

		float          g0;
		float          g1;
		float          g2;
		dsp::iir::Svf2p::conv_poles (g0, g1, g2, f * _inv_fs, k);

		for (auto &chn : _chn_arr)
		{
			chn._formant_filter.set_coefs_one (f_index, g0, g1, g2);
			chn._formant_filter.set_mix_one (f_index, 1, v1m, 0);
		}
	}
	for (int f_index = _vow_nbr_formants
	;	f_index < Cst::_nbr_formants
	;	++f_index)
	{
		for (auto &chn : _chn_arr)
		{
			chn._formant_filter.set_mix_one (f_index, 1, 0, 0);
		}
	}

#if 1 // Simplified, only one div
	//   (1 + (q - 1) * (r - 1) / r) / (r + (r * r - 1) * (1 - (r - 1) * (r - 1) / (r * r)))
	// = r * (q * r - q + 1) / (r * (r - 1) * (3 * r + 2) + 1)
	_vow_vol_fix =
		  _vow_reso * (_vow_q * _vow_reso - _vow_q + 1)
		/ (_vow_reso * (_vow_reso - 1) * (3 * _vow_reso + 2) + 1);
#else // Original formula
	const float    q_inf = (_vow_reso - 1) / _vow_reso;
	_vow_vol_fix =
		  (1 + (_vow_q - 1) * q_inf)
		/ (_vow_reso + (_vow_reso * _vow_reso - 1) * (1 - q_inf * q_inf));
#endif
}



const ColorMe::VoiceToneArray	ColorMe::_voice_tone_arr =
{{		//   f1,   f2,   f3
	{{	// Call
		{{  306, 2456, 3389 }},
		{{  417, 2351, 3128 }},
		{{  660, 2080, 2954 }},
		{{  788, 1503, 2737 }},
		{{  305, 2046, 2535 }},
		{{  469, 1605, 2581 }},
		{{  647, 1690, 2753 }},
		{{  311,  804, 2485 }},
		{{  461,  855, 2756 }},
		{{  634, 1180, 2690 }}
	}},
	{{	// GD
		{{  275, 2585, 3815 }},
		{{  405, 2553, 3346 }},
		{{  614, 2306, 3137 }},
		{{  830, 1438, 2900 }},
		{{  276, 2091, 2579 }},
		{{  409, 1599, 2703 }},
		{{  599, 1678, 2843 }},
		{{  291,  779, 2648 }},
		{{  415,  842, 2862 }},
		{{  595, 1144, 2907 }}
	}},
	{{	// G&A
		{{  348, 2365, 3130 }},
		{{  423, 2176, 2860 }},
		{{  526, 2016, 2800 }},
		{{  685, 1677, 2735 }},
		{{  371, 2063, 2745 }},
		{{  420, 1693, 2687 }},
		{{  436, 1643, 2715 }},
		{{  404, 1153, 2742 }},
		{{  438, 1140, 2790 }},
		{{  528, 1347, 2743 }}
	}}
}};



}  // namespace colorme
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
