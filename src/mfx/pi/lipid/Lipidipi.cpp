/*****************************************************************************

        Lipidipi.cpp
        Author: Laurent de Soras, 2021

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
#include "mfx/pi/lipid/Cst.h"
#include "mfx/pi/lipid/Param.h"
#include "mfx/pi/lipid/Lipidipi.h"
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
namespace lipid
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



constexpr int	Cst::_max_image_pairs;



Lipidipi::Lipidipi (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		_coef_list.data (), _nbr_coef, 1 / 1000.0
	);

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FAT   , 1);
	_state_set.set_val_nat (desc_set, Param_GREASE, 1);

	_state_set.add_observer (Param_FAT   , _param_change_flag);
	_state_set.add_observer (Param_GREASE, _param_change_flag);

	for (auto &chn : _ali->_chn_arr)
	{
		chn._ssb.set_coefs (_coef_list.data ());
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Lipidipi::do_get_state () const
{
	return _state;
}



double	Lipidipi::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Lipidipi::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	fstb::unused (max_buf_len);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	for (auto &chn : _ali->_chn_arr)
	{
		fstb::unused (chn);
		// Nothing at the moment
	}

	_param_change_flag.set ();

	update_param (true);
	_param_proc.req_steady ();

	latency = fstb::round_int (hiir::PolyphaseIir2Designer::compute_group_delay (
		_coef_list.data (), _nbr_coef, 1000 * _inv_fs, true
	));

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Lipidipi::do_process_block (piapi::ProcInfo &proc)
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
	const int      nbr_spl = proc._nbr_spl;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		// Computes the oscillator values
		alignas (fstb_SIMD128_ALIGN) OscArrBuf cos_arr;
		alignas (fstb_SIMD128_ALIGN) OscArrBuf sin_arr;
		for (int vec_idx = 0; vec_idx < _nbr_osc_vec; ++vec_idx)
		{
			const auto     sc { _ali->_osc_arr [vec_idx].step () };
			const auto     sca_idx = vec_idx << fstb::Vf32::_len_l2;
			std::get <0> (sc).store (&cos_arr [sca_idx]);
			std::get <1> (sc).store (&sin_arr [sca_idx]);
		}

		for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
		{
			Channel &      chn = _ali->_chn_arr [chn_idx];

			const auto     in  = proc._src_arr [chn_idx] [pos];
			auto           sum = in;

			float          x;
			float          y;
			chn._ssb.process_sample (x, y, in);
			for (int k = 0; k < _nbr_osc; ++k)
			{
				const auto     val = x * cos_arr [k] + y * sin_arr [k];
				sum += val;
			}

			proc._dst_arr [chn_idx] [pos] = sum;
		}
	}

	// Duplicates the remaining output channels
	for (int chn_idx = nbr_chn_proc; chn_idx < nbr_chn_dst; ++chn_idx)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_idx],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Lipidipi::clear_buffers ()
{
	for (auto &chn : _ali->_chn_arr)
	{
		chn._ssb.clear_buffers ();
	}
	for (auto &osc : _ali->_osc_arr)
	{
		osc.clear_buffers ();
	}
}



void	Lipidipi::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const auto     fat    = float (_state_set.get_val_end_nat (Param_FAT));
		const auto     grease = float (_state_set.get_val_end_nat (Param_GREASE));

		const auto     nbr_pairs = fstb::ceil_int (fat);
		_nbr_osc     = nbr_pairs * 2;
		_nbr_osc_vec = fstb::div_ceil (_nbr_osc, fstb::Vf32::_length);

		// Frequency difference between two consecutive pairs, Hz
		if (_nbr_osc > 0)
		{
			assert (_nbr_osc > 1);
			const auto     osc_e_mul  = fstb::rcp_uint <float> (_nbr_osc - 1);
			const auto     angle_dif  = float (fstb::PI * 2) * grease * _inv_fs;
			constexpr auto ratio_end  = 0.5f;
			const auto     ratio_step =
				fstb::Approx::exp2 (fstb::Approx::log2 (ratio_end) * osc_e_mul);
			const auto     rs2        = ratio_step * ratio_step;
			const auto     rs4        = rs2 * rs2;
			const auto     a_mul      = fstb::Vf32 (rs4);
			auto           a_cur      = fstb::Vf32 (angle_dif);
			a_cur *= fstb::Vf32 (1, -ratio_step, rs2, -ratio_step * rs2);

			for (int vec_idx = 0; vec_idx < _nbr_osc_vec; ++vec_idx)
			{
				_ali->_osc_arr [vec_idx].set_step (a_cur);
				a_cur *= a_mul;
			}
		}
	}
}



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
