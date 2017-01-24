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



template <class VD, class VS, class VP, int ORD>
EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::EnvFollowerAHR4SimdHelper ()
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



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_atk_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk [env] = coef;
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_hold_time (int env, int nbr_spl)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (nbr_spl >= 0);

	_hold_time [env] = float (nbr_spl);
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_rls_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls [env] = coef;
}



// in must contain only positive values!
template <class VD, class VS, class VP, int ORD>
fstb::ToolsSimd::VectF32	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_sample (const fstb::ToolsSimd::VectF32 &in)
{
	assert (test_ge_0 (in));

	const auto     zero       = fstb::ToolsSimd::set_f32_zero ();
	const auto     one        = fstb::ToolsSimd::set1_f32 (1);
	const auto     coef_a     = V128Par::load_f32 (_coef_atk);
	const auto     coef_r     = V128Par::load_f32 (_coef_rls);

	auto           state      = V128Par::load_f32 (_state [0]);
	auto           hold_c     = V128Par::load_f32 (_hold_counter);

	auto           delta      = in - state;
	auto           delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);
	const auto     hold_t     = V128Par::load_f32 (_hold_time);

	// delta >  0 (attack)       ---> coef = _coef_atk
	// delta <= 0 (release/hold) ---> coef = _coef_rls
	const auto     hc_gt_0    = fstb::ToolsSimd::cmp_gt_f32 (hold_c, zero);
	const auto     coef_r_cur =
		fstb::ToolsSimd::select (hc_gt_0, zero, coef_r);
	auto           coef       =
		fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

	// state += coef * (in - state)
	fstb::ToolsSimd::mac (state, delta, coef);

	const auto     hcm1 = fstb::ToolsSimd::max_f32 (hold_c - one, zero);
	hold_c = fstb::ToolsSimd::select (delta_gt_0, hold_t, hcm1);

	V128Par::store_f32 (_state [0], state);
	V128Par::store_f32 (_hold_counter, hold_c);

	for (int flt = 1; flt < ORD; ++flt)
	{
		const auto     prev = state;
		state = V128Par::load_f32 (_state [flt]);
		delta = prev - state;

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls
		delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);
		coef       = fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r);

		// state += coef * (in - state)
		fstb::ToolsSimd::mac (state, delta, coef);
		V128Par::store_f32 (_state [flt], state);
	}

	return state;
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_block (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const auto     zero   = fstb::ToolsSimd::set_f32_zero ();
	const auto     one    = fstb::ToolsSimd::set1_f32 (1);
	const auto     coef_a = V128Par::load_f32 (_coef_atk);
	const auto     coef_r = V128Par::load_f32 (_coef_rls);
	const auto     hold_t = V128Par::load_f32 (_hold_time);

	std::array <fstb::ToolsSimd::VectF32, ORD + 1>   state;
	for (int flt = 0; flt < ORD; ++flt)
	{
		state [flt + 1] = V128Par::load_f32 (_state [flt]);
	}
	auto           hold_c = V128Par::load_f32 (_hold_counter);

	long				pos = 0;
	do
	{
		state [0] = V128Src::load_f32 (in_ptr + pos);
		assert (test_ge_0 (state [0]));

		const auto     hc_gt_0    = fstb::ToolsSimd::cmp_gt_f32 (hold_c, zero);
		const auto     coef_r_cur =
			fstb::ToolsSimd::select (hc_gt_0, zero, coef_r);

		auto           delta      = state [0] - state [1];

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls or 0
		auto           delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);
		auto           coef       =
			fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

		// state += coef * (in - state)
		fstb::ToolsSimd::mac (state [1], delta, coef);

		const auto     hcm1 = fstb::ToolsSimd::max_f32 (hold_c - one, zero);
		hold_c = fstb::ToolsSimd::select (delta_gt_0, hold_t, hcm1);

		for (int flt = 1; flt < ORD; ++flt)
		{
			delta      = state [flt] - state [flt + 1];

			// delta >  0 (attack)       ---> coef = _coef_atk
			// delta <= 0 (release/hold) ---> coef = _coef_rls
			delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);
			coef       = fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r);

			// state += coef * (in - state)
			fstb::ToolsSimd::mac (state [flt + 1], delta, coef);
		}

		V128Dest::store_f32 (out_ptr + pos, state [ORD]);

		++ pos;
	}
	while (pos < nbr_spl);

	for (int flt = 0; flt < ORD; ++flt)
	{
		V128Par::store_f32 (_state [flt], state [flt + 1]);
	}
	V128Par::store_f32 (_hold_counter, hold_c);
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_block_1_chn (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	std::array <float, ORD + 1> state;
	for (int flt = 0; flt < ORD; ++flt)
	{
		state [flt + 1] = _state [flt] [0];
	}
	float          hold_c = _hold_counter [0];

	long           pos = 0;
	do
	{
		state [0] = in_ptr [pos];
		assert (state [0] >= 0);

		const float    delta = state [0] - state [1];
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
		state [1] += delta * coef;

		for (int flt = 1; flt < ORD; ++flt)
		{
			const float    delta = state [flt] - state [flt + 1];
			const float    coef  = (delta > 0) ? _coef_atk [0] : _coef_rls [0];
			state [flt + 1] += delta * coef;
		}

		out_ptr [pos] = state [ORD];

		++ pos;
	}
	while (pos < nbr_spl);

	for (int flt = 0; flt < ORD; ++flt)
	{
		_state [flt] [0] = state [flt + 1];
	}
	_hold_counter [0] = hold_c;
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::clear_buffers ()
{
	for (int flt = 0; flt < ORD; ++flt)
	{
		V128Par::store_f32 (_state [flt], fstb::ToolsSimd::set_f32_zero ());
	}
	V128Par::store_f32 (_hold_counter, fstb::ToolsSimd::set_f32_zero ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP, int ORD>
bool	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::test_ge_0 (const fstb::ToolsSimd::VectF32 &in)
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
