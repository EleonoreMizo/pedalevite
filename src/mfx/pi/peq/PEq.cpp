/*****************************************************************************

        PEq.cpp
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
#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/pi/peq/PEq.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace peq
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PEq::PEq ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_nbr_chn (0)
,	_biq_pack ()
,	_band_info_arr ()
,	_stage_to_band_arr ()
,	_param_change_flag ()
,	_neutral_time (4096)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	for (int band = 0; band < _nbr_bands; ++band)
	{
		const int      base = PEqDesc::compute_param_base (band);

		_state_set.set_val_nat (desc_set, base + Param_TYPE  , PEqType_PEAK);
		_state_set.set_val (base + Param_FREQ, (band + 0.5) / _nbr_bands);
		_state_set.set_val_nat (desc_set, base + Param_Q     ,  1);
		_state_set.set_val_nat (desc_set, base + Param_GAIN  ,  1);
		_state_set.set_val_nat (desc_set, base + Param_BYPASS,  0);

		BandInfo &     band_info    = _band_info_arr [band];

		band_info._neutral_duration = _neutral_time;
		band_info._stage_index      = -1;

		_state_set.add_observer (base + Param_TYPE  , band_info._change_flag);
		_state_set.add_observer (base + Param_FREQ  , band_info._change_flag);
		_state_set.add_observer (base + Param_Q     , band_info._change_flag);
		_state_set.add_observer (base + Param_GAIN  , band_info._change_flag);
		_state_set.add_observer (base + Param_BYPASS, band_info._change_flag);

		band_info._change_flag.add_observer (_param_change_flag);

		_stage_to_band_arr [band] = -1;
	}

	_biq_pack.reserve (_nbr_bands, _max_nbr_chn);
	_biq_pack.set_ramp_time (_update_resol);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	PEq::do_get_state () const
{
	return _state;
}



double	PEq::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	PEq::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &b_inf : _band_info_arr)
	{
		b_inf._change_flag.set ();
	}

	_nbr_chn      = 0; // Force update
	_neutral_time = max_buf_len * 2;

	_state = State_ACTIVE;

	return Err_OK;
}



void	PEq::do_process_block (ProcInfo &proc)
{
	const int      nbr_chn_in =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_IN ];
	const int      nbr_chn_out =
		proc._nbr_chn_arr [piapi::PluginInterface::Dir_OUT];
	if (nbr_chn_out != _nbr_chn)
	{
		_nbr_chn = nbr_chn_out;
		const int		nbr_stages = count_nbr_stages ();
		_biq_pack.adapt_config (nbr_stages, _nbr_chn);
		cook_all_bands ();
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
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

	int            pos = 0;
	do
	{
		// We need this intermediate varaible because for some reason GCC
		// fails to link when _update_resol is directly used in std::min.
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		if (_param_change_flag (true))
		{
			for (int band = 0; band < _nbr_bands; ++band)
			{
				BandInfo &        band_info = _band_info_arr [band];
				if (band_info._change_flag (true))
				{
					collect_parameters (band);

					if (! band_info.is_active () && ! band_info.is_bypass ())
					{
						activate_band (band);
					}

					if (band_info.is_active ())
					{
						update_filter_eq (band);
						if (! band_info.is_bypass ())
						{
							band_info._neutral_duration = 0;
						}
					}
				}
			}
		}

		// Signal processing
		_biq_pack.process_block (proc._dst_arr, proc._src_arr, pos, pos + work_len);

		pos += work_len;
	}
	while (pos < proc._nbr_spl);

	// Checks if some bands can now be deactivated
	for (int band = 0; band < _nbr_bands; ++band)
	{
		BandInfo &     band_info = _band_info_arr [band];
		if (   band_info.is_active ()
		    && band_info.is_bypass ())
		{
			band_info._neutral_duration += proc._nbr_spl;
			if (band_info._neutral_duration >= _neutral_time)
			{
				deactivate_band (band);
			}
		}
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	PEq::BandInfo::is_active () const
{
	return (_stage_index >= 0);
}



bool PEq::BandInfo::is_bypass () const
{
	bool ret_val = _bypass_flag;
	
	if (_type != PEqType_LP && _type != PEqType_HP)
	{
		ret_val |= is_unit_gain (_gain);
	}
	
	return ret_val;
}



int	PEq::find_empty_stage () const
{
	int            empty_stage = 0;
	while (_stage_to_band_arr [empty_stage] >= 0)
	{
		++ empty_stage;
		assert (empty_stage < _nbr_bands);
	}

	return (empty_stage);
}



int	PEq::count_nbr_stages () const
{
	int            nbr_stages = 0;
	for (int stage = 0; stage < _nbr_bands; ++stage)
	{
		if (_stage_to_band_arr [stage] >= 0)
		{
			nbr_stages = stage + 1;
		}
	}

	return (nbr_stages);
}



void	PEq::neutralise_stage_immediate (int stage_index)
{
	assert (stage_index >= 0);
	assert (stage_index < _nbr_bands);

	static const float   neutral_ab [3] = { 1, 0, 0 };

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.set_biquad (stage_index, chn, neutral_ab, neutral_ab, false);
	}
}



void	PEq::clear_buffers_stage (int stage_index)
{
	assert (stage_index >= 0);
	assert (stage_index < _nbr_bands);

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.clear_buffers_one (stage_index, chn);
	}
}



bool	PEq::is_band_active (int band) const
{
	return (_band_info_arr [band].is_active ());
}



void	PEq::activate_band (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);

	BandInfo &		band_info = _band_info_arr [band];

	if (! band_info.is_active ())
	{
		const int		stage_index    = find_empty_stage ();
		const int		nbr_stages_old = count_nbr_stages ();
		const int		nbr_stages_new =
			std::max (nbr_stages_old, stage_index + 1);
		if (nbr_stages_new > nbr_stages_old)
		{
			_biq_pack.adapt_config (nbr_stages_new, _nbr_chn);
		}

		band_info._neutral_duration      = 0;
		band_info._stage_index           = stage_index;
		_stage_to_band_arr [stage_index] = band;

		clear_buffers_stage (stage_index);
	}

	assert (band_info.is_active ());
}



void	PEq::deactivate_band (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);

	BandInfo &		band_info = _band_info_arr [band];

	if (band_info.is_active ())
	{
		const int		stage_index = band_info._stage_index;
		const int		nbr_stages_old = count_nbr_stages ();

		neutralise_stage_immediate (stage_index);

		_stage_to_band_arr [stage_index] = -1;
		band_info._stage_index           = -1;
		band_info._neutral_duration      = _neutral_time;

		const int		nbr_stages_new = count_nbr_stages ();
		if (nbr_stages_new < nbr_stages_old)
		{
			_biq_pack.adapt_config (nbr_stages_new, _nbr_chn);
		}
	}

	assert (! band_info.is_active ());
}



void	PEq::cook_all_bands ()
{
	for (int band = 0; band < _nbr_bands; ++band)
	{
		if (_band_info_arr [band]._stage_index >= 0)
		{
			cook_band (band);
		}
	}
}



void	PEq::collect_parameters (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);

	BandInfo &     b_info = _band_info_arr [band];

	const int      base   = PEqDesc::compute_param_base (band);

	b_info._freq = float (_state_set.get_val_end_nat (base + Param_FREQ));
	b_info._q    = float (_state_set.get_val_end_nat (base + Param_Q));
	b_info._gain = float (_state_set.get_val_end_nat (base + Param_GAIN));
	b_info._type = static_cast <PEqType> (fstb::round_int (
		_state_set.get_val_tgt_nat (base + Param_TYPE)
	));
	b_info._bypass_flag =
		(_state_set.get_val_tgt_nat (base + Param_BYPASS) >= 0.5f);
}



void	PEq::update_filter_eq (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);
	assert (is_band_active (band));

	BandInfo &     b_info = _band_info_arr [band];

	float          bs [3];
	float          as [3];
	float          bz [3];
	float          az [3];
	bool           z_flag = false;

	switch (b_info._type)
	{
	case	PEqType_PEAK:
#if 1	// This design requires more calculations
		dsp::iir::DesignEq2p::make_nyq_peak (
			bz,
			az,
			b_info._q,
			b_info._gain,
			b_info._freq,
			_sample_freq
		);
		z_flag = true;
#else
		dsp::iir::DesignEq2p::make_mid_peak (bs, as, b_info._q, b_info._gain);
#endif
		break;
	case	PEqType_SHELF_LO:
		dsp::iir::DesignEq2p::make_mid_shelf_lo (bs, as, b_info._q, b_info._gain);
		break;
	case	PEqType_HP:
		dsp::iir::DesignEq2p::make_hi_pass (bs, as, b_info._q);
		break;
	case	PEqType_SHELF_HI:
		dsp::iir::DesignEq2p::make_mid_shelf_hi (bs, as, b_info._q, b_info._gain);
		break;
	case	PEqType_LP:
		dsp::iir::DesignEq2p::make_low_pass (bs, as, b_info._q);
		break;
	default:
		assert (false);
		break;
	}

	if (! z_flag)
	{
		const float    k =
			dsp::iir::TransSZBilin::compute_k_approx (b_info._freq * _inv_fs);
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	}

	const int		stage_index = b_info._stage_index;
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.set_biquad (stage_index, chn, bz, az, true);
	}
}



void	PEq::cook_band (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);
	assert (is_band_active (band));

	collect_parameters (band);
	update_filter_eq (band);
}



bool	PEq::is_unit_gain (float gain)
{
	assert (gain > 0);

	return (fstb::is_eq (gain, 1.0f, 1e-2f));
}



}  // namespace peq
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
