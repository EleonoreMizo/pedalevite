/*****************************************************************************

        PEq.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_peq_PEq_CODEHEADER_INCLUDED)
#define mfx_pi_peq_PEq_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/pi/peq/Param.h"
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
namespace peq
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NB>
PEq <NB>::PEq ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_nbr_chn (0)
,	_biq_pack ()
,	_band_info_arr ()
,	_stage_to_band_arr ()
,	_param_change_flag ()
,	_neutral_time (4096)
,	_ramp_flag (true)
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	for (int band = 0; band < _nbr_bands; ++band)
	{
		const int      base = DescType::compute_param_base (band);

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



template <int NB>
piapi::PluginInterface::State	PEq <NB>::do_get_state () const
{
	return _state;
}



template <int NB>
double	PEq <NB>::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



template <int NB>
int	PEq <NB>::do_reset (double sample_freq, int max_buf_len, int &latency)
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
	_ramp_flag = false;
	update_param (true);

	_nbr_chn      = 0; // Force update
	_neutral_time = max_buf_len * 2;
	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



template <int NB>
void	PEq <NB>::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	if (nbr_chn_out != _nbr_chn)
	{
		_nbr_chn = nbr_chn_out;
		const int		nbr_stages = count_nbr_stages ();
		_biq_pack.adapt_config (nbr_stages, _nbr_chn);
		cook_all_bands ();
		_param_proc.req_all ();
	}

	std::array <const float *, _max_nbr_chn> src_chn_arr;
	const float * const *   src_ptr_arr = proc._src_arr;
	if (nbr_chn_in < nbr_chn_out)
	{
		for (int chn = 0; chn < nbr_chn_out; ++chn)
		{
			src_chn_arr [chn] = proc._src_arr [0];
		}
		src_ptr_arr = &src_chn_arr [0];
	}

	// Events
	_param_proc.handle_msg (proc);
	if (_param_proc.is_full_reset ())
	{
		_biq_pack.clear_buffers ();
	}

	int            pos = 0;
	do
	{
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		update_param (false);

		// Signal processing
		_biq_pack.process_block (proc._dst_arr, src_ptr_arr, pos, pos + work_len);

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



template <int NB>
bool	PEq <NB>::BandInfo::is_active () const
{
	return (_stage_index >= 0);
}



template <int NB>
bool PEq <NB>::BandInfo::is_bypass () const
{
	bool           ret_val = _param.is_bypass ();
	const PEqType  type    = _param.get_type ();

	if (type != PEqType_LP && type != PEqType_HP)
	{
		const float    gain = _param.get_gain ();
		ret_val |= is_unit_gain (gain);
	}

	return ret_val;
}



template <int NB>
void	PEq <NB>::clear_buffers ()
{
	_ramp_flag = false;
	_biq_pack.clear_buffers ();
}



template <int NB>
void	PEq <NB>::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		for (int band = 0; band < _nbr_bands; ++band)
		{
			BandInfo &        band_info = _band_info_arr [band];
			if (band_info._change_flag (true) || force_flag)
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

		_ramp_flag = ! _param_proc.is_req_steady_state ();
	}
}



template <int NB>
int	PEq <NB>::find_empty_stage () const
{
	int            empty_stage = 0;
	while (_stage_to_band_arr [empty_stage] >= 0)
	{
		++ empty_stage;
		assert (empty_stage < _nbr_bands);
	}

	return (empty_stage);
}



template <int NB>
int	PEq <NB>::count_nbr_stages () const
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



template <int NB>
void	PEq <NB>::neutralise_stage_immediate (int stage_index)
{
	assert (stage_index >= 0);
	assert (stage_index < _nbr_bands);

	static const float   neutral_ab [3] = { 1, 0, 0 };

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.set_biquad (stage_index, chn, neutral_ab, neutral_ab, false);
	}
}



template <int NB>
void	PEq <NB>::clear_buffers_stage (int stage_index)
{
	assert (stage_index >= 0);
	assert (stage_index < _nbr_bands);

	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.clear_buffers_one (stage_index, chn);
	}
}



template <int NB>
bool	PEq <NB>::is_band_active (int band) const
{
	return (_band_info_arr [band].is_active ());
}



template <int NB>
void	PEq <NB>::activate_band (int band)
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



template <int NB>
void	PEq <NB>::deactivate_band (int band)
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



template <int NB>
void	PEq <NB>::cook_all_bands ()
{
	for (int band = 0; band < _nbr_bands; ++band)
	{
		if (_band_info_arr [band]._stage_index >= 0)
		{
			cook_band (band);
		}
	}
}



template <int NB>
void	PEq <NB>::collect_parameters (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);

	BandParam &    b_param = _band_info_arr [band]._param;

	const int      base    = DescType::compute_param_base (band);

	b_param.set_freq (float (_state_set.get_val_end_nat (base + Param_FREQ)));
	b_param.set_q    (float (_state_set.get_val_end_nat (base + Param_Q)));
	b_param.set_gain (float (_state_set.get_val_end_nat (base + Param_GAIN)));
	b_param.set_type (static_cast <PEqType> (fstb::round_int (
		_state_set.get_val_tgt_nat (base + Param_TYPE)
	)));
	b_param.set_bypass (
		(_state_set.get_val_tgt_nat (base + Param_BYPASS) >= 0.5f)
	);
}



template <int NB>
void	PEq <NB>::update_filter_eq (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);
	assert (is_band_active (band));

	BandInfo &     b_info = _band_info_arr [band];

	float          bz [3];
	float          az [3];
	b_info._param.create_filter (bz, az, _sample_freq, _inv_fs);

	const int		stage_index = b_info._stage_index;
	bool           ramp_flag   = _ramp_flag;
	if (ramp_flag)
	{
		assert (_nbr_chn > 0);
		float          bz2 [3];
		float          az2 [3];
		_biq_pack.get_biquad_target (stage_index, 0, bz2, az2);
		ramp_flag = is_ramping_ok (az2 [1], az2 [2], az [1], az [2]);
	}
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		_biq_pack.set_biquad (stage_index, chn, bz, az, _ramp_flag);
	}
}



template <int NB>
void	PEq <NB>::cook_band (int band)
{
	assert (band >= 0);
	assert (band <= _nbr_bands);
	assert (is_band_active (band));

	collect_parameters (band);
	update_filter_eq (band);
}



template <int NB>
constexpr bool	PEq <NB>::is_unit_gain (float gain)
{
	assert (gain > 0);

	return (fstb::is_eq (gain, 1.0f, 1e-2f));
}



template <int NB>
constexpr bool	PEq <NB>::is_ramping_ok (float a1d, float a2d, float a1s, float a2s)
{
	const float    dd = compute_pole_delta (a1d, a2d);
	const float    ds = compute_pole_delta (a1s, a2s);

	return (dd * ds >= 0);
}



template <int NB>
constexpr float	PEq <NB>::compute_pole_delta (float a1, float a2)
{
	return a1 * a1 - 4 * a2;
}



}  // namespace peq
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_peq_PEq_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
