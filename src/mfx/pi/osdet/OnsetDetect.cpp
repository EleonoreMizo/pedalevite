/*****************************************************************************

        OnsetDetect.cpp
        Author: Laurent de Soras, 2018

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

/*******************************************************************************************/
#define NOMINMAX
#include <Windows.h>
/*******************************************************************************************/

#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvHelper.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/osdet/Param.h"
#include "mfx/pi/osdet/OnsetDetect.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include "mfx/dsp/wnd/CoefGenBHMinLobe.h"
#include "mfx/dsp/wnd/Generic.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace osdet
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OnsetDetect::OnsetDetect ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_buf_env_main ()
,	_buf_tmp ()
,	_buf_freq ()
,	_buf_mag_arr ()
,	_buf_psp (_fft_len / 2)
,	_env_main ()
,	_onset_prefilter ()
,	_frame_len (32)
,	_frame_pos (0)
,	_vol_sq (0)
,	_velo_gain (8)
,	_thr_off (1e-3f)
,	_velo_clip_flag (false)
,	_note_on_flag (false)
,	_stft ()
,	_buf_index (0)
,	_odf_val_post_old (0)
,	_dist_min (5)
,	_dist_cur (0)
,	_relax_coef (0)
,	_relax_time (0)
,	_psp_floor (0.1f)
,	_odf_mem_arr ()
,	_odf_mem_pos ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_VELO_CLIP    ,  0);
	_state_set.set_val_nat (desc_set, Param_REL_THR      ,  1e-2);

	_state_set.add_observer (Param_VELO_CLIP    , _param_change_flag);
	_state_set.add_observer (Param_REL_THR      , _param_change_flag);

	std::vector <float>  win_data (_fft_len);
	dsp::wnd::Generic <float, dsp::wnd::CoefGenBHMinLobe> win;
	win.make_win (&win_data [0], _fft_len);
	_stft.set_win (&win_data [0]);
	_stft.set_hop_size (_hop_size);

	for (auto &buf : _buf_mag_arr)
	{
		buf.resize (_fft_len / 2);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	OnsetDetect::do_get_state () const
{
	return _state;
}



double	OnsetDetect::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	OnsetDetect::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_align = (max_buf_len + 3) & ~3;
	_buf_env_main.resize (mbs_align);
	_buf_tmp.resize (mbs_align);
	_buf_freq.resize (_fft_len);

	set_relax_coef (1, _hop_size);

	const float    bs [2] = { 0.125f, 1 };
	const float    as [2] = { 1     , 1 };
	float          bz [2];
	float          az [2];
	mfx::dsp::iir::TransSZBilin::map_s_to_z_one_pole (
		bz, az, bs, as, 1000, sample_freq
	);
	_onset_prefilter.set_z_eq (bz, az);

	static const double  ana_frame_duration = 0.001;   // s
	_frame_len = fstb::round_int (sample_freq * ana_frame_duration);

	_env_main.set_times (0.0001f, 0.010f);

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return Err_OK;
}



void	OnsetDetect::do_clean_quick ()
{
	clear_buffers ();
}



void	OnsetDetect::do_process_block (ProcInfo &proc)
{
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

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Signal processing

	const int      nbr_spl = proc._nbr_spl;

	// Emphasis high-mid frequencies and lowers the bass frequencies
	_onset_prefilter.process_block (&_buf_tmp [0], proc._src_arr [0], nbr_spl);

	_env_main.process_block_no_sqrt (
		&_buf_env_main [0], &_buf_tmp [0], nbr_spl
	);

	float          ret_onset  = 0;
	float          ret_offset = 0;

	int            pos_r      = 0;
	do
	{
		float          vol2 = _buf_env_main [pos_r];

		// Note On
		int            work_len;
		bool           trans_flag;
		_stft.process_block (
			&_buf_tmp [pos_r],
			&_buf_freq [0],
			nbr_spl - pos_r,
			trans_flag,
			work_len
		);

		if (trans_flag)
		{
			// Magnitude extraction
			compute_magnitudes ();

			// Whitening
			whiten ();

			// Onset detection function
			float          val = compute_mkl ();

			const bool     onset_flag = detect_onset (val);
			if (onset_flag)
			{
				_dist_cur     = _dist_min;
				_note_on_flag = true;
				_vol_sq       = vol2;
				float          final_velo = sqrt (_vol_sq) * _velo_gain;
				if (_velo_clip_flag)
				{
					final_velo = std::min (final_velo, 1.0f);
				}
				ret_onset     = final_velo;
			}			

			_buf_index = 1 - _buf_index;
		}

		// Note Off
		if (_note_on_flag)
		{
			if (vol2 < _vol_sq * (_thr_off * _thr_off))
			{
				ret_offset    = 1;
				_note_on_flag = false;
			}
		}

		pos_r += work_len;
	}
	while (pos_r < nbr_spl);

	proc._sig_arr [0] [0] = ret_onset;
	proc._sig_arr [1] [0] = ret_offset;

/*******************************************************************************************/
	char           txt_0 [255+1];
	if (ret_onset != 0)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0), "On: %.3f\n", ret_onset);
		::OutputDebugStringA (txt_0);
	}
	if (ret_offset != 0)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0), "Off\n");
		::OutputDebugStringA (txt_0);
	}
/*******************************************************************************************/
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	OnsetDetect::clear_buffers ()
{
	_onset_prefilter.clear_buffers ();
	_frame_pos = 0;
	_vol_sq    = 0;
	_note_on_flag = false;

	_stft.clear_buffers ();
	_buf_index = 0;
	for (auto &buf : _buf_mag_arr)
	{
		dsp::mix::Align::clear (&buf [0], int (buf.size ()));
	}
	dsp::mix::Align::clear (&_buf_psp [0], int (_buf_psp.size ()));
	_odf_val_post_old = 0;
	_dist_cur = 0;
	for (auto &val : _odf_mem_arr)
	{
		val = 0;
	}
	_odf_mem_pos = 0;
}



void	OnsetDetect::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		_velo_clip_flag = (_state_set.get_val_tgt_nat (Param_VELO_CLIP) >= 0.5f);
		_thr_off        = float (
			_state_set.get_val_tgt_nat (Param_REL_THR)
		);
	}
}



float	OnsetDetect::compute_coef (double t) const
{
	assert (_sample_freq > 0);

	return float (
		dsp::dyn::EnvHelper::compute_env_coef_simple (t, _sample_freq)
	);
}



void	OnsetDetect::set_relax_coef (float t, int hop_size)
{
	_relax_time = t;
	_relax_coef = 0;
	if (t > 0)
	{
		_relax_coef = float (exp ((log (0.1) * hop_size) / (t * _sample_freq)));
	}
}



void	OnsetDetect::compute_magnitudes ()
{
	const int      ofs_imag = _fft_len / 2;
	BufAlign &     buf_cur  = _buf_mag_arr [_buf_index];
#if 0
	for (int bin = 1; bin < _nbr_bins; ++bin)
	{
		const float    real = _buf_freq [bin           ];
		const float    imag = _buf_freq [bin + ofs_imag];
		const float    mag  = sqrt (real * real + imag * imag);
		buf_cur [bin] = mag;
	}
#else
	for (int bin = 0; bin < _nbr_bins; bin += 4)
	{
		const auto     real =
			fstb::ToolsSimd::load_f32 (&_buf_freq [bin           ]);
		const auto     imag =
			fstb::ToolsSimd::load_f32 (&_buf_freq [bin + ofs_imag]);
		const auto     mag  = fstb::ToolsSimd::sqrt (real * real + imag * imag);
		fstb::ToolsSimd::store_f32 (&buf_cur [bin], mag);
	}
	buf_cur [0] = 0;  // Makes sure DC is 0
#endif
}



void	OnsetDetect::whiten ()
{
	BufAlign &     buf_cur  = _buf_mag_arr [_buf_index];
#if 0
	for (int bin = 1; bin < _nbr_bins; ++bin)
	{
		const float    mag     = buf_cur [bin];
		float          psp_cur = mag;
		const float    psp_old = _buf_psp [bin];
		if (psp_cur < psp_old)
		{
			psp_cur += (psp_old - psp_cur) * _relax_coef;
		}
		_buf_psp [bin] = psp_cur;

		buf_cur [bin] = mag / std::max (psp_cur, _psp_floor);
	}
#else
	const auto     coef      = fstb::ToolsSimd::set1_f32 (_relax_coef);
	const auto     psp_floor = fstb::ToolsSimd::set1_f32 (_psp_floor);
	for (int bin = 0; bin < _nbr_bins; bin += 4)
	{
		auto           mag     = fstb::ToolsSimd::load_f32 (&buf_cur [bin]);
		auto           psp_cur = mag;
		const auto     psp_old = fstb::ToolsSimd::load_f32 (&_buf_psp [bin]);
		const auto     c_inf_o = fstb::ToolsSimd::cmp_lt_f32 (psp_cur, psp_old);
		auto           lerp    = (psp_old - psp_cur) * coef;
		lerp = fstb::ToolsSimd::and_f32 (lerp, c_inf_o);
		psp_cur += lerp;

		fstb::ToolsSimd::store_f32 (&_buf_psp [bin], psp_cur);

		const auto     mx     = fstb::ToolsSimd::max_f32 (psp_cur, psp_floor);
		const auto     mx_inv = fstb::ToolsSimd::rcp_approx2 (mx);
		mag *= mx_inv;

		fstb::ToolsSimd::store_f32 (&buf_cur [bin], mag);
	}
#endif
}



// Computes the modified Kullback-Liebler distance (eq. 2.9)
float	OnsetDetect::compute_mkl ()
{
	BufAlign &     buf_cur  = _buf_mag_arr [_buf_index];
	BufAlign &     buf_old  = _buf_mag_arr [1 - _buf_index];
	const float    eps      = 1e-2f;
	float          val      = 0;
#if 0
	for (int bin = 1; bin < _nbr_bins; ++bin)
	{
		const float    mag_cur = buf_cur [bin];
		const float    mag_old = buf_old [bin];
		const float    ratio   = mag_cur / (mag_old + eps);
		const float    term    = log (1 + ratio);
		val += term ;
	}
#else
	const auto     eps_v    = fstb::ToolsSimd::set1_f32 (eps);
	const auto     one      = fstb::ToolsSimd::set1_f32 (1.f);
	auto           val_v    = fstb::ToolsSimd::set_f32_zero ();
	for (int bin = 0; bin < _nbr_bins; bin += 4)
	{
		const auto     mag_cur = fstb::ToolsSimd::load_f32 (&buf_cur [bin]);
		const auto     mag_old = fstb::ToolsSimd::load_f32 (&buf_old [bin]);
		const auto     den     = fstb::ToolsSimd::rcp_approx2 (mag_old + eps_v);
		const auto     ratio   = mag_cur * den;
		const auto     term    = fstb::ToolsSimd::log2_approx (one + ratio);
		val_v += term;
	}

	val  = fstb::ToolsSimd::sum_h_flt (val_v);
	val *= float (fstb::LN2);
#endif

	val *= 7.68f * 0.125f / _nbr_bins;

	return val;
}



bool	OnsetDetect::detect_onset (float odf_val)
{
	bool           onset_flag = false;

	_odf_mem_arr [_odf_mem_pos] = odf_val;

	std::array <float, _med_span> odf_val_sorted = _odf_mem_arr;
	std::sort (odf_val_sorted.begin (), odf_val_sorted.end ());

	const float    val_post = odf_val - odf_val_sorted [(_med_span - 1) >> 1];
	if (_dist_cur > 0)
	{
		-- _dist_cur;
	}
	else
	{
		const float    thr = 0.5f; // Threshold is arbirary.
		if (val_post > thr && _odf_val_post_old <= thr)
		{
			onset_flag = true;
			_dist_cur  = _dist_min;
		}
	}			

	_odf_mem_pos = (_odf_mem_pos + 1) % _med_span;
	_odf_val_post_old = val_post;

	return onset_flag;
}



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
