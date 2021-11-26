/*****************************************************************************

        Compex.cpp
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

#include "fstb/Approx.h"
#include "fstb/DataAlign.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/pi/cpx/Compex.h"
#include "mfx/pi/cpx/Param.h"
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
namespace cpx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Compex::Compex (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_param_change_flag ()
,	_param_change_flag_ar ()
,	_param_change_flag_vol_curves_ss ()
,	_nbr_chn (1)
,	_nbr_chn_in (1)
,	_nbr_chn_ana (1)
,	_env_fol_xptr ()
,	_smoother_xptr ()
,	_use_side_chain_flag (false)
,	_sc_power_1 ()
,	_sc_power_2 ()
,	_buf_tmp ()
{
	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_RATIO_L   ,  1);
	_state_set.set_val_nat (desc_set, Param_RATIO_H   ,  1);
	_state_set.set_val_nat (desc_set, Param_KNEE_LVL  , -2);
	_state_set.set_val_nat (desc_set, Param_KNEE_SHAPE,  0);
	_state_set.set_val_nat (desc_set, Param_ATTACK    ,  0.0001);
	_state_set.set_val_nat (desc_set, Param_RELEASE   ,  0.150);
	_state_set.set_val_nat (desc_set, Param_GAIN      ,  0);

	_state_set.add_observer (Param_RATIO_L   , _param_change_flag_vol_curves_ss);
	_state_set.add_observer (Param_RATIO_H   , _param_change_flag_vol_curves_ss);
	_state_set.add_observer (Param_KNEE_LVL  , _param_change_flag_vol_curves_ss);
	_state_set.add_observer (Param_KNEE_SHAPE, _param_change_flag_vol_curves_ss);
	_state_set.add_observer (Param_ATTACK    , _param_change_flag_ar);
	_state_set.add_observer (Param_RELEASE   , _param_change_flag_ar);
	_state_set.add_observer (Param_GAIN      , _param_change_flag_vol_curves_ss);

	_param_change_flag_ar			  .add_observer (_param_change_flag);
	_param_change_flag_vol_curves_ss.add_observer (_param_change_flag);

	static const float  abz [3] = { 1, 0, 0 };
	usew (_smoother_xptr).set_z_eq_same (abz, abz);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Compex::do_get_state () const
{
	return _state;
}



double	Compex::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Compex::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

#if defined (fstb_HAS_SIMD)
	const int      buf_len = std::min (
		(max_buf_len + 3) & ~3,
		int (_update_resol)
	);
	_buf_tmp.resize (buf_len);
#else
	fstb::unused (max_buf_len);
#endif

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_ar			  .set ();
	_param_change_flag_vol_curves_ss.set ();

	update_param (true);
	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Compex::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;

	_nbr_chn_in  = nbr_chn_in;
	_nbr_chn_ana = nbr_chn_in;
	_nbr_chn     = nbr_chn_out;

	// Events
	_param_proc.handle_msg (proc);
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	int            pos = 0;
	do
	{
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		update_param ();

		// Signal processing
		process_block_part (
			proc._dst_arr,
			proc._src_arr,
			proc._src_arr + nbr_chn_in,
			pos,
			pos + work_len
		);

		pos += work_len;
	}
	while (pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
constexpr float	Compex::AddProc <NC>::process_scalar (float in)
{
	return in;
}

template <>
constexpr float	Compex::AddProc <2>::process_scalar (float in)
{
	return in * 0.5f;
}



template <int NC>
fstb::Vf32	Compex::AddProc <NC>::process_vect (const fstb::Vf32 &in)
{
	return in;
}

template <>
fstb::Vf32	Compex::AddProc <2>::process_vect (const fstb::Vf32 &in)
{
	return in * fstb::Vf32 (0.5f);
}



#if ! defined (fstb_HAS_SIMD)



void	Compex::Smoother::set_z_eq_same (const float bz [3], const float az [3])
{
	for (auto &biq : _biq_arr)
	{
		biq.set_z_eq (bz, az);
	}
}

void	Compex::Smoother::clear_buffers ()
{
	for (auto &biq : _biq_arr)
	{
		biq.clear_buffers ();
	}
}

void	Compex::Smoother::process_block_serial_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	for (auto &biq : _biq_arr)
	{
		biq.process_block (dst_ptr, src_ptr, nbr_spl);
		src_ptr = dst_ptr;
	}
}



#endif // fstb_HAS_SIMD



void	Compex::clear_buffers ()
{
	usew (_env_fol_xptr).clear_buffers ();
	usew (_smoother_xptr).clear_buffers ();

	_gain_fnc.clear_buffers ();
}



void	Compex::update_param (bool force_flag)
{
	if (_param_change_flag_ar (true) || force_flag)
	{
		update_param_ar ();
	}

	if (_param_change_flag_vol_curves_ss (true) || force_flag)
	{
		update_param_vol_curves_ss ();
	}
}



void	Compex::update_param_ar ()
{
	const float    atk_t = float (_state_set.get_val_tgt_nat (Param_ATTACK));
	const float    atk_coef = compute_env_coef (atk_t);
	usew (_env_fol_xptr).set_atk_coef (atk_coef);

	const float    rls_t = float (_state_set.get_val_tgt_nat (Param_RELEASE));
	const float    rls_coef = compute_env_coef (rls_t);
	usew (_env_fol_xptr).set_rls_coef (rls_coef);

	// Smoother
	const float    min_time = std::min (atk_t, rls_t);
	assert (min_time > 0);
	float          f0       = 1.0f / min_time;
	// We limit the smoothing to 2000 Hz. This should be enough to let pass
	// most transcients with the smallest attack time. 
	f0 = fstb::limit (f0, 1.0f, 2000.0f);
	static const float   bs [3] = { 1, 0    , 0 };
	static const float   as [3] = { 1, 1.85f, 1 };
	float				bz [3];
	float				az [3];
	const float    k = dsp::iir::TransSZBilin::compute_k_approx (float (
		f0 / _sample_freq
	));
	dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	usew (_smoother_xptr).set_z_eq_same (bz, az);
}



void	Compex::update_param_vol_curves_ss ()
{
	// Ratios
	const float    rl = float (_state_set.get_val_tgt_nat (Param_RATIO_L));
	const float    rh = float (_state_set.get_val_tgt_nat (Param_RATIO_H));

	// Knee shape
	const float    knee_shape_l2 = float (_state_set.get_val_tgt_nat (Param_KNEE_SHAPE));

	const float    threshold_l2  =
		float (_state_set.get_val_tgt_nat (Param_KNEE_LVL));
	const float    gain_l2       =
		float (_state_set.get_val_tgt_nat (Param_GAIN));

	bool           autofix_flag  = true;

#if 0
	const float    param_sc = float (_state_set.get_val_tgt_nat (Param_SIDECHAIN));
	_use_side_chain_flag = (param_sc >= 0.5f);
#endif

	_gain_fnc.update_curve (
		rl, rh, threshold_l2, gain_l2, knee_shape_l2,
		autofix_flag && ! _use_side_chain_flag
	);
}



float	Compex::compute_env_coef (float t) const
{
	assert (t >= 0);

	float          coef = 1;
	const float    t_spl = t * float (_sample_freq);

	if (t_spl > 1)
	{
		coef = 1.0f - fstb::Approx::exp2 (float (-fstb::LN2) / t_spl);
	}

	return (coef);
}



void	Compex::process_block_part (float * const out_ptr_arr [], const float * const in_ptr_arr [], const float * const sc_ptr_arr [], int pos_beg, int pos_end)
{
	const float * const *	analyse_ptr_arr =
		(_use_side_chain_flag) ? sc_ptr_arr : in_ptr_arr;
	int            nbr_chn_analyse = _nbr_chn_ana;

	if (nbr_chn_analyse == 2)
	{
		_sc_power_2.prepare_env_input (
			&_buf_tmp [0],
			analyse_ptr_arr,
			nbr_chn_analyse,
			pos_beg,
			pos_end
		);
	}
	else
	{
		// Works also for nbr_chn_analyse > 2, but scans only the first channel.
		_sc_power_1.prepare_env_input (
			&_buf_tmp [0],
			analyse_ptr_arr,
			1,
			pos_beg,
			pos_end
		);
	}

	float *        tmp_ptr = &_buf_tmp [0];
	const int      nbr_spl = pos_end - pos_beg;
	usew (_env_fol_xptr).process_block (tmp_ptr, tmp_ptr, nbr_spl);
	usew (_smoother_xptr).process_block_serial_immediate (tmp_ptr, tmp_ptr, nbr_spl);
	conv_env_to_log (nbr_spl);

	const int      pos_block_end = (nbr_spl + 3) & ~3;
	int            pos = 0;

	// Special case for the first group of sample: we store the gain change.
	{
		const auto     x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
		const auto     y = _gain_fnc.compute_gain <true> (x);
		fstb::ToolsSimd::store_f32 (tmp_ptr + pos, y);

		pos += 4;
	}
	// Next groups of samples
	while (pos < pos_block_end)
	{
		const auto     x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
		const auto     y = _gain_fnc.compute_gain <false> (x);
		fstb::ToolsSimd::store_f32 (tmp_ptr + pos, y);

		pos += 4;
	}

	int            chn_in_index = 0;
	int            chn_in_step = (_nbr_chn <= _nbr_chn_in) ? 1 : 0;
	for (int chn_index = 0; chn_index < _nbr_chn; ++chn_index)
	{
		dsp::mix::Simd <
			fstb::DataAlign <false>,
			fstb::DataAlign <false>
		>::mult_1_1 (
			out_ptr_arr [chn_index  ] + pos_beg,
			in_ptr_arr [chn_in_index] + pos_beg,
			tmp_ptr,
			nbr_spl
		);

		chn_in_index += chn_in_step;
	}
}



void	Compex::conv_env_to_log (int nbr_spl)
{
	assert (nbr_spl > 0);

	const int      block_bnd = nbr_spl & ~3;
	int            pos       = 0;
	while (pos < block_bnd)
	{
		auto           val = fstb::ToolsSimd::load_f32 (&_buf_tmp [pos]);
		val = fstb::Approx::log2 (val);
		fstb::ToolsSimd::store_f32 (&_buf_tmp [pos], val);

		pos += 4;
	}

	while (pos < nbr_spl)
	{
		_buf_tmp [pos] = fstb::Approx::log2 (_buf_tmp [pos]);
		++ pos;
	}

	const int      clean_end = (nbr_spl + 3) & ~3;
	while (pos < clean_end)
	{
		_buf_tmp [pos] = 0;
		++ pos;
	}
}



template <class T>
T &	Compex::usew (WrapperAlign <T> &wrap)
{
#if defined (fstb_HAS_SIMD)
	return *wrap;
#else
	return wrap;
#endif
}



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
