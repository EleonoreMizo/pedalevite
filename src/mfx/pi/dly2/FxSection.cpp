/*****************************************************************************

        FxSection.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/FxSection.h"
#include "mfx/pi/dly2/Param.h"
#include "mfx/pi/ParamStateSet.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxSection::init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set)
{
	assert (line_index >= 0);
	assert (line_index < Cst::_nbr_lines);
	assert (_desc_set_ptr  == 0);
	assert (_state_set_ptr == 0);

	_line_index      = line_index;
	_desc_set_ptr    = &desc_set;
	_state_set_ptr   = &state_set;

	const int      base = Delay2Desc::get_line_base (_line_index);

	state_set.set_val_nat (desc_set, base + ParamLine_FX_FLT_T   , FilterType_RESO);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_FLT_F   , 640);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_FLT_R   , 1);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_FLT_Q   , 1);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_DIST_A  , 0);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_DIST_F  , 0);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_SHLF_F  , 4000);
	state_set.set_val_nat (desc_set, base + ParamLine_FX_SHLF_L  , 1);

	state_set.add_observer (base + ParamLine_FX_FLT_T   , _param_change_flag_filter);
	state_set.add_observer (base + ParamLine_FX_FLT_F   , _param_change_flag_filter);
	state_set.add_observer (base + ParamLine_FX_FLT_R   , _param_change_flag_filter);
	state_set.add_observer (base + ParamLine_FX_FLT_Q   , _param_change_flag_filter);
	state_set.add_observer (base + ParamLine_FX_DIST_A  , _param_change_flag_dist);
	state_set.add_observer (base + ParamLine_FX_DIST_F  , _param_change_flag_dist);
	state_set.add_observer (base + ParamLine_FX_SHLF_F  , _param_change_flag_shelf);
	state_set.add_observer (base + ParamLine_FX_SHLF_L  , _param_change_flag_shelf);

	_param_change_flag_filter.add_observer (_param_change_flag);
	_param_change_flag_dist  .add_observer (_param_change_flag);
	_param_change_flag_shelf .add_observer (_param_change_flag);

	state_set.set_ramp_time (base + ParamLine_FX_FLT_F   , 0.010f);
	state_set.set_ramp_time (base + ParamLine_FX_FLT_R   , 0.010f);
	state_set.set_ramp_time (base + ParamLine_FX_DIST_A  , 0.010f);
	state_set.set_ramp_time (base + ParamLine_FX_DIST_F  , 0.010f);
	state_set.set_ramp_time (base + ParamLine_FX_SHLF_L  , 0.010f);

	_shelf_hi.neutralise ();
	_filter.neutralise ();
}



void	FxSection::reset (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq     = float (    sample_freq);
	_inv_fs          = float (1 / sample_freq);

	_disto.set_sample_freq (sample_freq);

	update_param (true);

	clear_buffers ();
}



void	FxSection::clear_buffers ()
{
	_filter.clear_buffers ();
	_disto.clear_buffers ();
	_shelf_hi.clear_buffers ();
}



void	FxSection::process_block (float data_ptr [], int nbr_spl)
{
	assert (_sample_freq > 0);
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	update_param (false);

	_filter.process_block (data_ptr, data_ptr, nbr_spl);
	_disto.process_block (data_ptr, nbr_spl);
	_shelf_hi.process_block (data_ptr, data_ptr, nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	FxSection::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const int      base = Delay2Desc::get_line_base (_line_index);

		if (_param_change_flag_filter (true) || force_flag)
		{
			_filter_type = FilterType (fstb::round_int (
				_state_set_ptr->get_val_tgt_nat (base + ParamLine_FX_FLT_T)
			));
			_filter_freq = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_FLT_F)
			);
			_filter_reso = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_FLT_R)
			);
			_filter_q   = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_FLT_Q)
			);

			update_filter ();
		}

		if (_param_change_flag_dist (true) || force_flag)
		{
			const float    amt  = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_DIST_A)
			);
			_disto.set_amount (amt);
			const float    fold = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_DIST_F)
			);
			_disto.set_foldback (fold);
		}

		if (_param_change_flag_shelf (true) || force_flag)
		{
			_shelf_freq = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_SHLF_F)
			);
			_shelf_lvl  = float (
				_state_set_ptr->get_val_end_nat (base + ParamLine_FX_SHLF_L)
			);

			update_shelf ();
		}
	}
}



void	FxSection::update_filter ()
{
	float          bs [3];
	float          as [3];

	as [2] = 1;
	as [0] = 1;

	switch (_filter_type)
	{
	case FilterType_RESO:
		{
			const float   inv_q = 1 / _filter_q;
			bs [2] = 1;
			bs [1] = _filter_reso * inv_q;
			bs [0] = 1;
			as [1] = inv_q;
		}
		break;

	case FilterType_LP:
		bs [2] = 0;
		bs [1] = 0;
		bs [0] = 1;
		as [1] = 1 / _filter_reso;
		break;

	case FilterType_BP:
		{
			const float   inv_q = 1 / _filter_q;
			bs [2] = 0;
			bs [1] = _filter_reso * inv_q;
			bs [0] = 0;
			as [1] = inv_q;
		}
		break;

	case FilterType_HP:
		bs [2] = 1;
		bs [1] = 0;
		bs [0] = 0;
		as [1] = 1 / _filter_reso;
		break;

	default:	
		assert (false);
		break;
	}

	const float    f = _filter_freq * _inv_fs;
	const float    k = dsp::iir::TransSZBilin::compute_k_approx (f);
	float          bz [3];
	float          az [3];
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	_filter.set_z_eq (bz, az);
}



void	FxSection::update_shelf ()
{
	const float    bs [2] = { 1, _shelf_lvl };
	const float    as [2] = { 1,          1 };

	const float    f = _shelf_freq * _inv_fs;
	const float    k = dsp::iir::TransSZBilin::compute_k_approx (f);
	float          bz [2];
	float          az [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (bz, az, bs, as, k);
	_shelf_hi.set_z_eq (bz, az);
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
