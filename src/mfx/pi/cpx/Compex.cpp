/*****************************************************************************

        Compex.cpp
        Author: Laurent de Soras, 2016

Knee formula calculation
------------------------

hard knee is supposed to be at (0, 0)
rl = ratio, low, expressed as a slope (0 = infinite ratio)
rh = ratio, high (slope too)
k = knee radius, relative to the input level. Knee operates in [-k ; +k]

f(x) = a*x^3 + b*x^2 + c*x + d
f'(x) = 3*a*x^2 + 2*b*x + c

f(k) = k * rh  	<=>	a*k^3 + b*k^2 + c*k + d = k * rh
f(-k) = -k * rl	<=>	-a*k^3 + b*k^2 - c*k + d = -k * rl
f'(k) = rh     	<=>	3*a*k^2 + 2*b*k + c = rh
f'(-k) = rl    	<=>	3*a*k^2 - 2*b*k + c = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
  -k^3 * a +   k^2 * b -   k * c + d = -k * rl
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
 2*k^3 * a             + 2*k * c     = k * (rh + rl)		[l1 - l2]
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
 2*k^2 * a             + 2   * c     = rh + rl      		[l2 / k]
 3*k^2 * a + 2*k   * b +       c     = rh
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
-4*k^2 * a + 4*k   * b               = rh - rl      		[l2 - 2 * l4]
             4*k   * b               = rh - rl   			[l3 - l4]
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
-4*k^2 * a + 4*k   * b               = rh - rl
                     b               = (rh - rl) / (4*k)
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
         a                           = 0
                     b               = (rh - rl) / (4*k)
 3*k^2 * a - 2*k   * b +       c     = rl

   k^3 * a +   k^2 * b +   k * c + d = k * rh
         a                           = 0
                     b               = (rh - rl) / (4*k)
                               c     = (rh + rl) / 2

                                   d = (rh - rl) * k / 4
         a                           = 0
                     b               = (rh - rl) / (4*k)
                               c     = (rh + rl) / 2

Conclusion:
a = 0
b = (rh - rl) / (k * 4)
c = (rh + rl) / 2
d = (rh - rl) * k / 4
2nd order polynomial is enough.

Example:
rl = 3, rh = 1/5, k = 2
b = -7/20
c = 8/5
d = -7/5
f(x) = -7/20*x^2 + 8/5*x - 7/5

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



namespace mfx
{
namespace pi
{
namespace cpx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Compex::Compex ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
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
,	_vol_offset_pre (-2)
,	_vol_offset_post (-2)
,	_ratio_hi (1)
,	_ratio_lo (1)
,	_knee_coef_arr ({{ 0, 1, 0 }})
,	_knee_th_abs (-0.5f)
,	_sc_power_1 ()
,	_sc_power_2 ()
,	_buf_tmp ()
,	_cur_gain (0)
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
	_smoother_xptr->set_z_eq_same (abz, abz);
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

	const int      buf_len = (max_buf_len + 3) & ~3;
	_buf_tmp.resize (buf_len);

	_sample_freq = float (sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_ar			  .set ();
	_param_change_flag_vol_curves_ss.set ();

	update_param (true);
	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Compex::do_clean_quick ()
{
	clear_buffers ();
}



void	Compex::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_in  = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_out = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;

	_nbr_chn_in  = nbr_chn_in;
	_nbr_chn_ana = nbr_chn_in;
	_nbr_chn     = nbr_chn_out;

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
		const int      max_len  = _update_resol;
		const int      work_len = std::min (proc._nbr_spl - pos, max_len);

		// Parameters
		_state_set.process_block (work_len);
		update_param ();

		// Signal processing
		process_block_part (
			proc._dst_arr,
			proc._src_arr,
			proc._src_arr,
			pos,
			pos + work_len
		);

		pos += work_len;
	}
	while (pos < proc._nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
float	Compex::AddProc <NC>::process_scalar (float in)
{
	return (in);
}

template <>
float	Compex::AddProc <2>::process_scalar (float in)
{
	return (in * 0.5f);
}



template <int NC>
fstb::ToolsSimd::VectF32	Compex::AddProc <NC>::process_vect (const fstb::ToolsSimd::VectF32 &in)
{
	return (in);
}

template <>
fstb::ToolsSimd::VectF32	Compex::AddProc <2>::process_vect (const fstb::ToolsSimd::VectF32 &in)
{
	return (in * fstb::ToolsSimd::set1_f32 (0.5f));
}


void	Compex::clear_buffers ()
{
	_env_fol_xptr->clear_buffers ();
	_smoother_xptr->clear_buffers ();

	_cur_gain = 0;
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
	_env_fol_xptr->set_atk_coef (0,atk_coef);

	const float    rls_t = float (_state_set.get_val_tgt_nat (Param_RELEASE));
	const float    rls_coef = compute_env_coef (rls_t);
	_env_fol_xptr->set_rls_coef (0, rls_coef);

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
	_smoother_xptr->set_z_eq_same (bz, az);
}



void	Compex::update_param_vol_curves_ss ()
{
	// Ratios
	const float    rl = float (_state_set.get_val_tgt_nat (Param_RATIO_L));
	const float    rh = float (_state_set.get_val_tgt_nat (Param_RATIO_H));

	// Knee shape
	float          knee_shape_l2 = float (_state_set.get_val_tgt_nat (Param_KNEE_SHAPE));
	knee_shape_l2 = std::max (knee_shape_l2, 0.01f);

	const float    k = knee_shape_l2 * 0.5f;  // 0.5 for diameter to radius conversion

	_knee_th_abs = k;
	_ratio_lo    = rl;
	_ratio_hi    = rh;

	const float    c2 = (rh - rl) / (k * 4);
	const float    c1 = (rh + rl) * 0.5f;
	const float    c0 = (rh - rl) * k * 0.25f;

	_knee_coef_arr [0] = c0;
	_knee_coef_arr [1] = c1;
	_knee_coef_arr [2] = c2;

	const float    threshold_l2 =
		float (_state_set.get_val_tgt_nat (Param_KNEE_LVL));

	const float    gain_l2      =
		float (_state_set.get_val_tgt_nat (Param_GAIN));

	_vol_offset_pre = threshold_l2;

#if 0
	const float    param_sc = float (_state_set.get_val_tgt_nat (Param_SIDECHAIN));
	_use_side_chain_flag = (param_sc >= 0.5f);
#endif

	float          fixed_gain = gain_l2;
	if (! _use_side_chain_flag)
	{
		const float    thr_dist     = threshold_l2 - _lvl_ref_l2;
		const float    ratio_at_ref = (thr_dist < 0) ? rh : rl;
		const float    gain_at_ref  = thr_dist * (1 - ratio_at_ref);
		fixed_gain -= gain_at_ref;
	}

	_vol_offset_post = fixed_gain;
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
	_env_fol_xptr->process_block_1_chn (tmp_ptr, tmp_ptr, nbr_spl);
	_smoother_xptr->process_block_serial_immediate (tmp_ptr, tmp_ptr, nbr_spl);
	conv_env_to_log (nbr_spl);

	const int      pos_block_end = (nbr_spl + 3) & ~3;
	int            pos = 0;

	// Special case for the first group of sample: we store the gain change.
	{
		const auto     x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
		const auto     y = compute_gain <true> (x);
		fstb::ToolsSimd::store_f32 (tmp_ptr + pos, y);

		pos += 4;
	}
	// Next groups of samples
	while (pos < pos_block_end)
	{
		const auto     x = fstb::ToolsSimd::load_f32 (tmp_ptr + pos);
		const auto     y = compute_gain <false> (x);
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
		fstb::ToolsSimd::VectF32   val =
			fstb::ToolsSimd::load_f32 (&_buf_tmp [pos]);
		val = fstb::ToolsSimd::log2_approx (val);
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



// Input: env_2l2 = 2 * log2 (linear volume envelope).
// An arbitrary negative value corresponds to zero.
template <bool store_flag>
fstb::ToolsSimd::VectF32	Compex::compute_gain (const fstb::ToolsSimd::VectF32 env_2l2)
{
	// Shifts volume in order to have the threshold at 0
	const auto     half = fstb::ToolsSimd::set1_f32 (0.5f);
	const auto     env_l2 = env_2l2 * half;
	const auto     vol_offset_pre = fstb::ToolsSimd::set1_f32 (_vol_offset_pre);
	const auto     el2 = env_l2 - vol_offset_pre;

	// Knee polynomial
	const auto     c2 = fstb::ToolsSimd::set1_f32 (_knee_coef_arr [2]);
	auto           poly = el2 * c2;
	const auto     c1 = fstb::ToolsSimd::set1_f32 (_knee_coef_arr [1]);
	poly += c1;
	poly *= el2;
	const auto     c0 = fstb::ToolsSimd::set1_f32 (_knee_coef_arr [0]);
	poly += c0;

	// Linear ratios
	const auto     zero     = fstb::ToolsSimd::set_f32_zero ();
	const auto     tst_pos  = fstb::ToolsSimd::cmp_lt_f32 (el2, zero);
	auto           ratio_lo = fstb::ToolsSimd::set1_f32 (_ratio_lo);
	auto           ratio_hi = fstb::ToolsSimd::set1_f32 (_ratio_hi);
	const auto     ratio    = fstb::ToolsSimd::select (tst_pos, ratio_lo, ratio_hi);
	auto           linear   = el2 * ratio;

	// Selects result
	const auto     knee_th_abs = fstb::ToolsSimd::set1_f32 (_knee_th_abs);
	const auto     el2_abs     = fstb::ToolsSimd::abs (el2);
	const auto     tst_knee    = fstb::ToolsSimd::cmp_lt_f32 (el2_abs, knee_th_abs);
	const auto     vl2         = fstb::ToolsSimd::select (tst_knee, poly, linear);

	// Computes and limits gain
	auto           gain_l2     = vl2 - el2;
	const auto     gain_min_l2 = fstb::ToolsSimd::set1_f32 (_gain_min_l2);
	const auto     gain_max_l2 = fstb::ToolsSimd::set1_f32 (_gain_max_l2);
	gain_l2 = fstb::ToolsSimd::min_f32 (gain_l2, gain_max_l2);	// Another solution would be mirroring before minimising: gain_top_l2 - abs (gain_l2 - gain_top_l2) with gain_top_l2 > gain_max_l2.
	gain_l2 = fstb::ToolsSimd::max_f32 (gain_l2, gain_min_l2);	// Quickest solution but may generate very low signals turning later into denormals.

	// Handles the case where detected volume is close to 0 (-oo dB).
	// Smoothly fades the gain to 0 dB.
	// g = g * limit ((v - v0) / (v1 - v0), 0, 1)
	const auto     active_thr_l2 = fstb::ToolsSimd::set1_f32 (_active_thr_l2);
	const auto     active_mul    = fstb::ToolsSimd::set1_f32 (_active_mul);
	const auto     one           = fstb::ToolsSimd::set1_f32 (1);
	auto           active_rate   = env_l2 - active_thr_l2;
	active_rate *= active_mul;
	active_rate  = fstb::ToolsSimd::min_f32 (active_rate, one);
	active_rate  = fstb::ToolsSimd::max_f32 (active_rate, zero);
	gain_l2     *= active_rate;

	// Stores the gain adjustment if requested
	if (store_flag)
	{
		_cur_gain = fstb::ToolsSimd::Shift <0>::extract (gain_l2);
	}

	// Additional gain (manual + auto)
	const auto     vol_offset_post = fstb::ToolsSimd::set1_f32 (_vol_offset_post);
	gain_l2 += vol_offset_post;

	// Conversion to linear, multiplicative volume
	const auto     gain = fstb::ToolsSimd::exp2_approx (gain_l2);

	return gain;
}



const float	Compex::_gain_min_l2   = -16;
const float	Compex::_gain_max_l2   =   8;
const float	Compex::_active_thr_l2 = -14;
const float	Compex::_lvl_ref_l2    =  -4; // -24 dB
const float	Compex::_active_mul    = 1.0f / (-10 - -14);	// -60 -> -84 dB;



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
