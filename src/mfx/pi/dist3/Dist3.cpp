/*****************************************************************************

        Dist3.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/dist3/Param.h"
#include "mfx/pi/dist3/Dist3.h"
#include "mfx/pi/dist3/Dist3Desc.h"
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
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Dist3::Dist3 (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_in ()
,	_param_change_flag_out ()
,	_param_change_flag_bias ()
,	_param_change_flag_psu ()
,	_param_change_flag_misc ()
,	_proc ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_HPF_PRE ,     30);
	_state_set.set_val_nat (desc_set, Param_GAIN    ,      1);
	_state_set.set_val_nat (desc_set, Param_SRL_PRE ,  64000);
	_state_set.set_val_nat (desc_set, Param_CLASS   , Dist3Desc::Class_A);
	_state_set.set_val_nat (desc_set, Param_BIAS_S  ,      0);
	_state_set.set_val_nat (desc_set, Param_BIAS_D  ,      0);
	_state_set.set_val_nat (desc_set, Param_BIAS_LPF,    512);
	_state_set.set_val_nat (desc_set, Param_BIAS_SRC, Dist3Desc::BiasSrc_DIRECT);
	_state_set.set_val_nat (desc_set, Param_TYPE    , Shaper::Type_ASINH);
	_state_set.set_val_nat (desc_set, Param_SRL_POST,  16000);
	_state_set.set_val_nat (desc_set, Param_PSU_OVRL,  false);
	_state_set.set_val_nat (desc_set, Param_PSU_LOAD,      1.0);
	_state_set.set_val_nat (desc_set, Param_PSU_FREQ,     55);
	_state_set.set_val_nat (desc_set, Param_LPF_POST,  20480);

	_state_set.add_observer (Param_HPF_PRE , _param_change_flag_in);
	_state_set.add_observer (Param_GAIN    , _param_change_flag_misc);
	_state_set.add_observer (Param_SRL_PRE , _param_change_flag_in);
	_state_set.add_observer (Param_CLASS   , _param_change_flag_misc);
	_state_set.add_observer (Param_BIAS_S  , _param_change_flag_bias);
	_state_set.add_observer (Param_BIAS_D  , _param_change_flag_bias);
	_state_set.add_observer (Param_BIAS_LPF, _param_change_flag_bias);
	_state_set.add_observer (Param_BIAS_SRC, _param_change_flag_bias);
	_state_set.add_observer (Param_TYPE    , _param_change_flag_misc);
	_state_set.add_observer (Param_SRL_POST, _param_change_flag_out);
	_state_set.add_observer (Param_PSU_OVRL, _param_change_flag_psu);
	_state_set.add_observer (Param_PSU_LOAD, _param_change_flag_psu);
	_state_set.add_observer (Param_PSU_FREQ, _param_change_flag_psu);
	_state_set.add_observer (Param_LPF_POST, _param_change_flag_out);

	_param_change_flag_in.add_observer (_param_change_flag);
	_param_change_flag_out.add_observer (_param_change_flag);
	_param_change_flag_bias.add_observer (_param_change_flag);
	_param_change_flag_psu.add_observer (_param_change_flag);
	_param_change_flag_misc.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_HPF_PRE , 0.010);
	_state_set.set_ramp_time (Param_GAIN    , 0.010);
	_state_set.set_ramp_time (Param_SRL_PRE , 0.010);
	_state_set.set_ramp_time (Param_BIAS_S  , 0.010);
	_state_set.set_ramp_time (Param_BIAS_D  , 0.010);
	_state_set.set_ramp_time (Param_BIAS_LPF, 0.010);
	_state_set.set_ramp_time (Param_SRL_POST, 0.010);
	_state_set.set_ramp_time (Param_PSU_LOAD, 0.010);
	_state_set.set_ramp_time (Param_PSU_FREQ, 0.010);
	_state_set.set_ramp_time (Param_LPF_POST, 0.010);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Dist3::do_get_state () const
{
	return _state;
}



double	Dist3::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Dist3::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	double         lat_flt = 0;
	_proc->reset (sample_freq, max_buf_len, lat_flt);

	_param_change_flag_in.set ();
	_param_change_flag_out.set ();
	_param_change_flag_bias.set ();
	_param_change_flag_psu.set ();
	_param_change_flag_misc.set ();

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state  = State_ACTIVE;
	latency = fstb::round_int (lat_flt);

	return piapi::Err_OK;
}



void	Dist3::do_process_block (piapi::ProcInfo &proc)
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

	// Signal processing
	_proc->process_block (
		proc._dst_arr, proc._src_arr, proc._nbr_spl, nbr_chn_proc
	);

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



void	Dist3::clear_buffers ()
{
	_proc->clear_buffers ();
}



void	Dist3::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_in (true) || force_flag)
		{
			const float    freq =
				float (_state_set.get_val_end_nat (Param_HPF_PRE));
			const float    srl  =
				float (_state_set.get_val_end_nat (Param_SRL_PRE));

			_proc->set_freq_hpf_pre (freq);
			_proc->set_slew_rate_pre (srl);
		}

		if (_param_change_flag_out (true) || force_flag)
		{
			const float    freq =
				float (_state_set.get_val_end_nat (Param_LPF_POST));
			const float    srl  =
				float (_state_set.get_val_end_nat (Param_SRL_POST));

			_proc->set_freq_lpf_post (freq);
			_proc->set_slew_rate_post (srl);
		}

		if (_param_change_flag_bias (true) || force_flag)
		{
			const float    bias_s = 
				float (_state_set.get_val_end_nat (Param_BIAS_S));
			const float    bias_d = 
				float (_state_set.get_val_end_nat (Param_BIAS_D));
			const float    freq =
				float (_state_set.get_val_end_nat (Param_BIAS_LPF));
			const bool     fdbk_flag = (
				   fstb::round_int (_state_set.get_val_tgt_nat (Param_BIAS_SRC))
				== Dist3Desc::BiasSrc_FDBK
			);

			_proc->set_bias_s (bias_s);
			_proc->set_bias_d (bias_d);
			_proc->set_freq_lpf_bias (freq);
			_proc->set_bias_fdbk (fdbk_flag);
		}

		if (_param_change_flag_psu (true) || force_flag)
		{
			const bool     flag =
				(_state_set.get_val_tgt_nat (Param_PSU_OVRL) >= 0.5f);
			const float    load = 
				float (_state_set.get_val_end_nat (Param_PSU_LOAD));
			const float    freq = 
				float (_state_set.get_val_end_nat (Param_PSU_FREQ));
			_proc->enable_psu (flag);
			_proc->set_psu_load (load);
			_proc->set_psu_ac_freq (freq);
		}

		if (_param_change_flag_misc (true) || force_flag)
		{
			const float    gain = 
				float (_state_set.get_val_end_nat (Param_GAIN));
			const bool     class_b_flag = (
				   fstb::round_int (_state_set.get_val_tgt_nat (Param_CLASS))
				== Dist3Desc::Class_B
			);
			const Shaper::Type   type = static_cast <Shaper::Type> (
				fstb::round_int (_state_set.get_val_tgt_nat (Param_TYPE))
			);

			_proc->set_gain (gain);
			_proc->set_class_b (class_b_flag);
			_proc->set_type (type);
		}
	}
}



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
