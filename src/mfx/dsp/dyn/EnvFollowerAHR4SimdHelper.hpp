/*****************************************************************************

        EnvFollowerAHR4SimdHelper.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
EnvFollowerAHR4SimdHelper <VD, VS, VP>::EnvFollowerAHR4SimdHelper ()
/*:	_state ()
,	_coef_atk ()
,	_coef_rls ()
,	_hold_time ()
,	_hold_counter ()*/
{
	const auto     one  = fstb::ToolsSimd::set1_f32 (1);
	const auto     zero = fstb::ToolsSimd::set_f32_zero ();
	V128Par::store_f32 (_coef_atk , one );
	V128Par::store_f32 (_hold_time, zero);
	V128Par::store_f32 (_coef_rls , one );

	clear_buffers ();
}



template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::set_atk_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk [env] = coef;
}



template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::set_hold_time (int env, int nbr_spl)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (nbr_spl >= 0);

	_hold_time [env] = float (nbr_spl);
}



template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::set_rls_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls [env] = coef;
}



// in must contain only positive values!
template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	EnvFollowerAHR4SimdHelper <VD, VS, VP>::process_sample (const fstb::ToolsSimd::VectF32 &in)
{
	assert (test_ge_0 (in));

	auto           state      = V128Par::load_f32 (_state);
	auto           hold_c     = V128Par::load_f32 (_hold_counter);

	const auto     zero       = fstb::ToolsSimd::set_f32_zero ();
	const auto     one        = fstb::ToolsSimd::set1_f32 (1);
	const auto     delta      = in - state;
	const auto     delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);
	const auto     hold_t = V128Par::load_f32 (_hold_time);

	const auto     hc_gt_0    = fstb::ToolsSimd::cmp_gt_f32 (hold_c, zero);

	// delta >  0 (attack)       ---> coef = _coef_atk
	// delta <= 0 (release/hold) ---> coef = _coef_rls
	const auto     coef_a     = V128Par::load_f32 (_coef_atk);
	const auto     coef_r     = V128Par::load_f32 (_coef_rls);
	const auto     coef_r_cur =
		fstb::ToolsSimd::select (hc_gt_0, zero, coef_r);
	const auto     coef       =
		fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

	// state += coef * (in - state)
	const auto     delta_coef = delta * coef;
	state += delta_coef;

	const auto     hcm1 = fstb::ToolsSimd::max_f32 (hold_c - one, zero);
	hold_c = fstb::ToolsSimd::select (delta_gt_0, hold_t, hcm1);

	V128Par::store_f32 (_state, state);

	return state;
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::process_block (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	auto           state  = V128Par::load_f32 (_state);
	auto           hold_c = V128Par::load_f32 (_hold_counter);

	const auto     zero   = fstb::ToolsSimd::set_f32_zero ();
	const auto     one    = fstb::ToolsSimd::set1_f32 (1);
	const auto     coef_a = V128Par::load_f32 (_coef_atk);
	const auto     coef_r = V128Par::load_f32 (_coef_rls);
	const auto     hold_t = V128Par::load_f32 (_hold_time);

	long				pos = 0;
	do
	{
		const auto     in = V128Src::load_f32 (in_ptr + pos);
		assert (test_ge_0 (in));

		const auto     hc_gt_0    = fstb::ToolsSimd::cmp_gt_f32 (hold_c, zero);
		const auto     coef_r_cur =
			fstb::ToolsSimd::select (hc_gt_0, zero, coef_r);

		const auto     delta      = in - state;
		const auto     delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls or 0
		const auto     coef       =
			fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

		// state += coef * (in - state)
		const auto     delta_coef = delta * coef;
		state += delta_coef;

		const auto     hcm1 = fstb::ToolsSimd::max_f32 (hold_c - one, zero);
		hold_c = fstb::ToolsSimd::select (delta_gt_0, hold_t, hcm1);

		V128Dest::store_f32 (out_ptr + pos, state);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_state       , state);
	V128Par::store_f32 (_hold_counter, hold_c);
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::process_block_1_chn (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	float          state  = _state [0];
	float          hold_c = _hold_counter [0];

	long           pos = 0;
	do
	{
		const float    in    = in_ptr [pos];
		assert (in >= 0);

		const float    delta = in - state;
		float          coef  = 0;
		if (delta > 0)
		{
			coef   = _coef_atk [0];
			hold_c = _hold_time [0];
		}
		else
		{
			if (hold_c > 0)
			{
				-- hold_c;
			}
			else
			{
				coef = _coef_rls [0];
			}
		}
		state += delta * coef;

		out_ptr [pos] = state;

		++ pos;
	}
	while (pos < nbr_spl);

	_state [0]        = state;
	_hold_counter [0] = hold_c;
}



template <class VD, class VS, class VP>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP>::clear_buffers ()
{
	V128Par::store_f32 (_state       , fstb::ToolsSimd::set_f32_zero ());
	V128Par::store_f32 (_hold_counter, fstb::ToolsSimd::set_f32_zero ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
bool	EnvFollowerAHR4SimdHelper <VD, VS, VP>::test_ge_0 (const fstb::ToolsSimd::VectF32 &in)
{
	return (
		   fstb::ToolsSimd::Shift <0>::extract (in) >= 0
		&& fstb::ToolsSimd::Shift <1>::extract (in) >= 0
		&& fstb::ToolsSimd::Shift <2>::extract (in) >= 0
		&& fstb::ToolsSimd::Shift <3>::extract (in) >= 0
	);
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
