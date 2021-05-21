/*****************************************************************************

        DiodeClipper.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dclip/Param.h"
#include "mfx/pi/dclip/DiodeClipper.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace dclip
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DiodeClipper::DiodeClipper (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_chn_arr (_max_nbr_chn)
,	_gain (1)
,	_buf_tmp (_max_blk_size)
,	_buf_ovr (_max_blk_size * _ovrspl)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_GAIN   , 1);
	_state_set.set_val_nat (desc_set, Param_CUTOFF , 20480);
	_state_set.set_val_nat (desc_set, Param_SHAPE  , Shape_SYM_H);
	_state_set.set_val_nat (desc_set, Param_PRECLIP, 100);

	_state_set.add_observer (Param_GAIN   , _param_change_flag);
	_state_set.add_observer (Param_CUTOFF , _param_change_flag);
	_state_set.add_observer (Param_SHAPE  , _param_change_flag);
	_state_set.add_observer (Param_PRECLIP, _param_change_flag);

	_state_set.set_ramp_time (Param_GAIN, 0.010);

	init_ovrspl_coef ();
	for (auto &chn : _chn_arr)
	{
		chn._upspl.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._dwspl.set_coefs (&_coef_42 [0], &_coef_21 [0]);
		chn._dist.set_d1_is (0.1e-15f);
		chn._dist.set_d2_is (0.1e-15f);
		chn._dist.set_d1_n (1 * _diode_scale);
		chn._dist.set_d2_n (1 * _diode_scale);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	DiodeClipper::do_get_state () const
{
	return _state;
}



double	DiodeClipper::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	DiodeClipper::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	const double   fs_ovr  = sample_freq * _ovrspl;

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &chn : _chn_arr)
	{
		chn._dist.set_sample_freq (fs_ovr);
		chn._dckill.set_sample_freq (sample_freq);
		chn._dckill.set_cutoff_freq (5);
	}

	const double   f_fs   = 1000.0 / sample_freq;
	const double   dly_42 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_42 [0], _nbr_coef_42, f_fs * 0.25f, false
	);
	const double   dly_21 = hiir::PolyphaseIir2Designer::compute_group_delay (
		&_coef_21 [0], _nbr_coef_21, f_fs * 0.5f , false
	);
	double         latency_f = (0.5f * dly_21 + 0.25f * dly_42) * 2;
	latency_f += 3 * 0.25f; // Hard-clipper
	latency = fstb::round_int (latency_f);

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	DiodeClipper::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	const float    time_step = fstb::rcp_uint <float> (proc._nbr_spl);
	_gain.set_time (proc._nbr_spl, time_step);

	// Signal processing
	int            pos_blk = 0;
	do
	{
		const int      len_blk =
			std::min (proc._nbr_spl - pos_blk, int (_max_blk_size));
		const int      len_blk_ovr = len_blk * _ovrspl;

		_gain.tick (len_blk);
		const auto     gains_beg { compute_gain_pre_post (_gain.get_beg ()) };
		const auto     gains_end { compute_gain_pre_post (_gain.get_end ()) };

		for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
		{
			Channel &      chn = _chn_arr [chn_index];

			const float *  src_ptr = &proc._src_arr [chn_index] [pos_blk];
			float *        dst_ptr = &proc._dst_arr [chn_index] [pos_blk];

			// Gain, pre
			dsp::mix::Align::copy_1_1_vlr (
				_buf_tmp.data (),
				src_ptr,
				len_blk,
				gains_beg.first,
				gains_end.first
			);

			// Upsampling
			chn._upspl.process_block (
				_buf_ovr.data (), _buf_tmp.data (), len_blk
			);

			// Pre-clipping
			chn._preclip.process_block (
				_buf_ovr.data (), _buf_ovr.data (), len_blk_ovr
			);

			// Main distortion processing
			chn._dist.process_block (
				_buf_ovr.data (), _buf_ovr.data (), len_blk_ovr
			);

			// Downsampling
			chn._dwspl.process_block (dst_ptr, _buf_ovr.data (), len_blk);

			// Gain, post
			dsp::mix::Align::scale_1_vlrauto (
				dst_ptr, len_blk, gains_beg.second, gains_end.second
			);

			// DC removal
			chn._dckill.process_block (dst_ptr, dst_ptr, len_blk);
		}

		pos_blk += len_blk;
	}
	while (pos_blk < proc._nbr_spl);

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipper::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn._upspl.clear_buffers ();
		chn._dwspl.clear_buffers ();
		chn._dist.clear_buffers ();
		chn._dckill.clear_buffers ();
	}

	_gain.clear_buffers ();
}



void	DiodeClipper::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const float    freq = float (_state_set.get_val_end_nat (Param_CUTOFF));
		for (auto &chn : _chn_arr)
		{
			chn._dist.set_cutoff_freq (freq);
		}

		const float    gain = float (_state_set.get_val_end_nat (Param_GAIN));
		_gain.set_val (gain);

		const Shape    shape = static_cast <Shape> (fstb::round_int (
			_state_set.get_val_tgt_nat (Param_SHAPE)
		));
		const DiodeParam &   par = _diode_param_arr [shape];
		for (auto &chn : _chn_arr)
		{
			chn._dist.set_d1_is (par._is1);
			chn._dist.set_d1_n ( par._n1 );
			chn._dist.set_d2_is (par._is2);
			chn._dist.set_d2_n ( par._n2 );
		}

		const float    pcl = float (_state_set.get_val_end_nat (Param_PRECLIP));
		for (auto &chn : _chn_arr)
		{
			chn._preclip.set_clip_lvl (pcl);
		}
	}
}



std::pair <float, float>	DiodeClipper::compute_gain_pre_post (float gain)
{
	const float    gain_fix  = (gain < 1) ? 1 / gain : 1;
	const float    gain_pre  = gain * (1 / _sig_scale);
	const float    gain_post = gain_fix * _sig_scale;

	return std::make_pair (gain_pre, gain_post);
}



void	DiodeClipper::init_ovrspl_coef ()
{
	if (! _coef_init_flag)
	{
		const double   tbw = 0.01;
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_42 [0], _nbr_coef_42, tbw * 0.5 + 0.25
		);
		hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
			&_coef_21 [0], _nbr_coef_21, tbw
		);

		_coef_init_flag = true;
	}
}



bool	DiodeClipper::_coef_init_flag = false;
std::array <double, DiodeClipper::_nbr_coef_42>	DiodeClipper::_coef_42;
std::array <double, DiodeClipper::_nbr_coef_21>	DiodeClipper::_coef_21;

// Signal nominal level
const float	DiodeClipper::_sig_scale = 0.125f;

// For 1 V input, output clips around 0.725 V for the hard symmetric clipper,
// with a current of 0.125 mA. This is used as reference for the other levels.
// https://www.desmos.com/calculator/pv5j8tvbpw
const float	DiodeClipper::_diode_scale = 1.0f / 0.725f;
const std::array <DiodeClipper::DiodeParam, Shape_NBR_ELT> DiodeClipper::_diode_param_arr =
{{
	{  0.1e-15f,  1 * _diode_scale,  0.1e-15f,  1 * _diode_scale },
	{  0.1e-15f,  1 * _diode_scale,  0.1e-06f,  4 * _diode_scale },
	{  8.0e-06f, 10 * _diode_scale,  8.0e-06f, 10 * _diode_scale },
	{  5.0e-06f, 10 * _diode_scale, 10.0e-06f, 20 * _diode_scale }
}};


}  // namespace dclip
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
