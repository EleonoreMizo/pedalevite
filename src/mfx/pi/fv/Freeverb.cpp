/*****************************************************************************

        Freeverb.cpp
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
#include "mfx/dsp/StereoLevel.h"
#include "mfx/pi/fv/Freeverb.h"
#include "mfx/pi/fv/Param.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Freeverb::Freeverb ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_lvl ()
,	_param_change_flag_flt ()
,	_param_change_flag_other ()
,	_fv ()
,	_chn_arr ()
,	_src_lvl (1)
,	_dry_lvl (1)
,	_wet_lvl_direct (FreeverbCore::_scalewet)
,	_wet_lvl_cross (0)
,	_flt_flag (false)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_ROOMSIZE, 0.75);
	_state_set.set_val_nat (desc_set, Param_DAMP    , 0.5);
	_state_set.set_val_nat (desc_set, Param_WET     , 0.5);
	_state_set.set_val_nat (desc_set, Param_DRY     , 0.75);
	_state_set.set_val_nat (desc_set, Param_WIDTH   , 0.5);
	_state_set.set_val_nat (desc_set, Param_MODE    , 0);
	_state_set.set_val (Param_LOCUT, 0);
	_state_set.set_val (Param_HICUT, 1);

	_state_set.add_observer (Param_ROOMSIZE, _param_change_flag_other);
	_state_set.add_observer (Param_DAMP    , _param_change_flag_other);
	_state_set.add_observer (Param_WET     , _param_change_flag_lvl);
	_state_set.add_observer (Param_DRY     , _param_change_flag_lvl);
	_state_set.add_observer (Param_WIDTH   , _param_change_flag_lvl);
	_state_set.add_observer (Param_MODE    , _param_change_flag_other);
	_state_set.add_observer (Param_LOCUT   , _param_change_flag_flt);
	_state_set.add_observer (Param_HICUT   , _param_change_flag_flt);

	_param_change_flag_lvl  .add_observer (_param_change_flag);
	_param_change_flag_flt  .add_observer (_param_change_flag);
	_param_change_flag_other.add_observer (_param_change_flag);

	for (auto &chn : _chn_arr)
	{
		chn._filter.neutralise ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Freeverb::do_get_state () const
{
	return _state;
}



double	Freeverb::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Freeverb::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = sample_freq;

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_lvl  .set ();
	_param_change_flag_flt  .set ();
	_param_change_flag_other.set ();

	_fv.reset (sample_freq, max_buf_len);

	for (int chn_cnt = 0; chn_cnt < int (_chn_arr.size ()); ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];
		for (auto &buf : chn._buf_arr)
		{
			buf.resize (max_buf_len);
		}
	}

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Freeverb::do_clean_quick ()
{
	clear_buffers ();
}



void	Freeverb::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._nbr_chn_arr [piapi::Dir_IN ];
	const int      nbr_chn_out = proc._nbr_chn_arr [piapi::Dir_OUT];

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
	assert (nbr_chn_in <= nbr_chn_out);
	const int      chn_in_step = (nbr_chn_in < nbr_chn_out) ? 0 : 1;
	int            chn_in_cnt  = 0;
	for (int chn_cnt = 0; chn_cnt < nbr_chn_out; ++chn_cnt)
	{
		Channel &      chn = _chn_arr [chn_cnt];

		// Input
		dsp::mix::Align::copy_1_1_v (
			&chn._buf_arr [0] [0],
			proc._src_arr [chn_in_cnt],
			proc._nbr_spl,
			_src_lvl
		);
		dsp::mix::Align::copy_1_1_v (
			proc._dst_arr [chn_cnt],
			proc._src_arr [chn_in_cnt],
			proc._nbr_spl,
			_dry_lvl
		);

		chn_in_cnt += chn_in_step;

		// Reverb
		_fv.process_block (
			&chn._buf_arr [1] [0],
			&chn._buf_arr [0] [0],
			proc._nbr_spl,
			chn_cnt
		);
	}

	// Filtering
	if (_flt_flag)
	{
		for (int chn_cnt = 0; chn_cnt < nbr_chn_out; ++chn_cnt)
		{
			Channel &      chn = _chn_arr [chn_cnt];
			chn._filter.process_block (
				&chn._buf_arr [1] [0],
				&chn._buf_arr [1] [0],
				proc._nbr_spl
			);
		}
	}

	// Final mix
	if (nbr_chn_out == 1)
	{
		dsp::mix::Align::mix_1_1_v (
			proc._dst_arr [0],
			&_chn_arr [0]._buf_arr [1] [0],
			proc._nbr_spl,
			_wet_lvl_direct
		);
	}
	else
	{
		assert (nbr_chn_out == 2);
		dsp::mix::Align::mix_mat_2_2_v (
			proc._dst_arr [0],
			proc._dst_arr [1],
			&_chn_arr [0]._buf_arr [1] [0],
			&_chn_arr [1]._buf_arr [1] [0],
			proc._nbr_spl,
			dsp::StereoLevel (
				_wet_lvl_direct,
				_wet_lvl_cross,
				_wet_lvl_cross,
				_wet_lvl_direct
			)
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Freeverb::clear_buffers ()
{
	_fv.clear_buffers ();
	for (auto &chn : _chn_arr)
	{
		chn._filter.clear_buffers ();
	}
}



void	Freeverb::update_param ()
{
	if (_param_change_flag_lvl (true))
	{
		const float    width   = float (_state_set.get_val_tgt_nat (Param_WIDTH));
		const float    hwidth = width * 0.5f;

		_dry_lvl = float (_state_set.get_val_tgt_nat (Param_DRY));

		float          wet_lvl = float (_state_set.get_val_tgt_nat (Param_WET));
		wet_lvl *= FreeverbCore::_scalewet;

		_wet_lvl_direct = wet_lvl * (0.5f + hwidth);
		_wet_lvl_cross  = wet_lvl * (0.5f - hwidth);
	}

	if (_param_change_flag_flt (true))
	{
		const float    f_lo = float (_state_set.get_val_tgt_nat (Param_LOCUT));
		const float    f_hi = float (_state_set.get_val_tgt_nat (Param_HICUT));
		const float    f_hr = float (
			dsp::iir::TransSZBilin::prewarp_freq_rel_1 (f_hi, f_lo, _sample_freq)
		);

		const float    bs [3] = {    0, f_hr    , 0 };
		const float    as [3] = { f_hr, f_hr + 1, 1 };
		const float    fk = f_lo / float (_sample_freq);
		const float    k  = dsp::iir::TransSZBilin::compute_k_approx (fk);
		float          bz [3];
		float          az [3];
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);

		for (auto &chn : _chn_arr)
		{
			chn._filter.set_z_eq (bz, az);
		}

		const float       margin = 0.001f;
		_flt_flag = (f_lo > 20*(1+margin) || f_hi < 20000*(1-margin));
	}

	if (_param_change_flag_other (true))
	{
		float          rsize = float (_state_set.get_val_tgt_nat (Param_ROOMSIZE));
		float          damp  = float (_state_set.get_val_tgt_nat (Param_DAMP));
		const bool     freeze_flag =
			(_state_set.get_val_tgt_nat (Param_MODE) >= 0.5f);

		if (freeze_flag)
		{
			_src_lvl = 0;
			rsize    = 1;
			damp     = 0;
		}
		else
		{
			_src_lvl = FreeverbCore::_scalein;
			damp = std::min (damp, 0.99f);
		}

		_fv.set_damp (damp);
		_fv.set_reflectivity (rsize);
	}
}



}  // namespace fv
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
