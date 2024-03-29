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

#include "fstb/ToolsSimd.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP, int ORD>
EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::EnvFollowerAHR4SimdHelper () noexcept
/*:	_state ()
,	_coef_atk ()
,	_coef_rls ()
,	_hold_time ()
,	_hold_counter ()*/
{
	const auto     one  = fstb::Vf32 (1);
	const auto     zero = fstb::Vf32::zero ();
	V128Par::store_f32 (_coef_atk , one );
	V128Par::store_f32 (_hold_time, zero);
	V128Par::store_f32 (_coef_rls , one );

	clear_buffers ();
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_atk_coef (int env, float coef) noexcept
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk [env] = coef;
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_hold_time (int env, int nbr_spl) noexcept
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (nbr_spl >= 0);

	_hold_time [env] = float (nbr_spl);
}



template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::set_rls_coef (int env, float coef) noexcept
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls [env] = coef;
}



// in must contain only positive values!
template <class VD, class VS, class VP, int ORD>
fstb::Vf32	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_sample (const fstb::Vf32 &in) noexcept
{
	assert (in >= fstb::Vf32::zero ());

	const auto     zero       = fstb::Vf32::zero ();
	const auto     one        = fstb::Vf32 (1);
	const auto     coef_a     = V128Par::load_f32 (_coef_atk);
	const auto     coef_r     = V128Par::load_f32 (_coef_rls);

	auto           state      = V128Par::load_f32 (_state [0]);
	auto           hold_c     = V128Par::load_f32 (_hold_counter);

	auto           delta      = in - state;
	auto           delta_lt_0 = delta.is_lt_0 ();
	const auto     hold_t     = V128Par::load_f32 (_hold_time);

	// delta >  0 (attack)       ---> coef = _coef_atk
	// delta <= 0 (release/hold) ---> coef = _coef_rls
	const auto     hc_lt_0    = hold_c.is_lt_0 ();
	const auto     coef_r_cur = coef_r & hc_lt_0;
	auto           coef       = fstb::select (delta_lt_0, coef_r_cur, coef_a);

	// state += coef * (in - state)
	state.mac (delta, coef);

	const auto     hcm1 = fstb::max (hold_c - one, zero);
	hold_c = fstb::select (delta_lt_0, hcm1, hold_t);

	V128Par::store_f32 (_state [0], state);
	V128Par::store_f32 (_hold_counter, hold_c);

	for (int flt = 1; flt < ORD; ++flt)
	{
		const auto     prev = state;
		state = V128Par::load_f32 (_state [flt]);
		delta = prev - state;

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls
		delta_lt_0 = delta.is_lt_0 ();
		coef       = fstb::select (delta_lt_0, coef_r, coef_a);

		// state += coef * (in - state)
		state.mac (delta, coef);
		V128Par::store_f32 (_state [flt], state);
	}

	return state;
}



#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD( fltn) \
	fstb::Vf32      state##fltn; \
	if (fltn - 1 < ORD) \
	{ \
		state##fltn = V128Par::load_f32 (_state [fltn - 1]); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC( flt, fltn) \
	if (flt < ORD) \
	{ \
		delta      = state##flt - state##fltn; \
		delta_lt_0 = delta.is_lt_0 (); \
		coef       = fstb::select (delta_lt_0, coef_r, coef_a); \
		state##fltn.mac (delta, coef); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT( ord) \
	if (ord == ORD) \
	{ \
		V128Dest::store_f32 (out_ptr + pos, state##ord); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN( ord) \
	if (ord == ORD) \
	{ \
		return state##ord; \
	}
#define mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE( fltn) \
	if (fltn - 1 < ORD) \
	{ \
		V128Par::store_f32 (_state [fltn - 1],state##fltn); \
	}

// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_block (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const auto     zero   = fstb::Vf32::zero ();
	const auto     one    = fstb::Vf32 (1);
	const auto     coef_a = V128Par::load_f32 (_coef_atk);
	const auto     coef_r = V128Par::load_f32 (_coef_rls);
	const auto     hold_t = V128Par::load_f32 (_hold_time);

	auto           hold_c = V128Par::load_f32 (_hold_counter);
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (1)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (2)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (3)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (4)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (5)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (6)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (7)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (8)

	int            pos = 0;
	do
	{
		const auto     state0 = V128Src::load_f32 (in_ptr + pos);
		assert (state0 >= fstb::Vf32::zero ());

		const auto     hc_lt_0    = hold_c.is_lt_0 ();
		const auto     coef_r_cur = coef_r & hc_lt_0;

		auto           delta      = state0 - state1;

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls or 0
		auto           delta_lt_0 = delta.is_lt_0 ();
		auto           coef       = fstb::select (delta_lt_0, coef_r_cur, coef_a);

		// state += coef * (in - state)
		state1.mac (delta, coef);

		const auto     hcm1 = fstb::max (hold_c - one, zero);
		hold_c = fstb::select (delta_lt_0, hcm1, hold_t);

		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (1, 2)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (2, 3)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (3, 4)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (4, 5)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (5, 6)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (6, 7)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (7, 8)

		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (1)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (2)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (3)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (4)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (5)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (6)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (7)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT (8)

		++ pos;
	}
	while (pos < nbr_spl);

	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (1)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (2)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (3)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (4)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (5)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (6)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (7)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (8)

	V128Par::store_f32 (_hold_counter, hold_c);
}



// Input data must contain only positive values!
template <class VD, class VS, class VP, int ORD>
fstb::Vf32	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::analyse_block (const fstb::Vf32 in_ptr [], int nbr_spl) noexcept
{
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	const auto     zero   = fstb::Vf32::zero ();
	const auto     one    = fstb::Vf32 (1);
	const auto     coef_a = V128Par::load_f32 (_coef_atk);
	const auto     coef_r = V128Par::load_f32 (_coef_rls);
	const auto     hold_t = V128Par::load_f32 (_hold_time);

	auto           hold_c = V128Par::load_f32 (_hold_counter);
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (1)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (2)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (3)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (4)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (5)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (6)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (7)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD (8)

	int            pos = 0;
	do
	{
		const auto     state0 = V128Src::load_f32 (in_ptr + pos);
		assert (state0 >= fstb::Vf32::zero ());

		const auto     hc_lt_0    = hold_c.is_lt_0 ();
		const auto     coef_r_cur = coef_r & hc_lt_0;

		auto           delta      = state0 - state1;

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls or 0
		auto           delta_lt_0 = delta.is_lt_0 ();
		auto           coef       = fstb::select (delta_lt_0, coef_r_cur, coef_a);

		// state += coef * (in - state)
		state1.mac (delta, coef);

		const auto     hcm1 = fstb::max (hold_c - one, zero);
		hold_c = fstb::select (delta_lt_0, hcm1, hold_t);

		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (1, 2)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (2, 3)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (3, 4)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (4, 5)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (5, 6)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (6, 7)
		mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC (7, 8)

		++ pos;
	}
	while (pos < nbr_spl);

	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (1)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (2)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (3)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (4)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (5)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (6)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (7)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE (8)

	V128Par::store_f32 (_hold_counter, hold_c);

	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (1)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (2)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (3)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (4)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (5)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (6)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (7)
	mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN (8)
}

#undef mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_LOAD
#undef mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_PROC
#undef mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RESULT
#undef mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_RETURN
#undef mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_SAVE



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP, int ORD>
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::process_block_1_chn (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	float          state [ORD + 1];
	for (int flt = 0; flt < ORD; ++flt)
	{
		state [flt + 1] = _state [flt] [0];
	}
	float          hold_c = _hold_counter [0];

	int            pos = 0;
	do
	{
		state [0] = in_ptr [pos];
		assert (state [0] >= 0);

		float          delta = state [0] - state [1];
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
			delta = state [flt] - state [flt + 1];
			coef  = (delta > 0) ? _coef_atk [0] : _coef_rls [0];
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
void	EnvFollowerAHR4SimdHelper <VD, VS, VP, ORD>::clear_buffers () noexcept
{
	for (int flt = 0; flt < ORD; ++flt)
	{
		V128Par::store_f32 (_state [flt], fstb::Vf32::zero ());
	}
	V128Par::store_f32 (_hold_counter, fstb::Vf32::zero ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_EnvFollowerAHR4SimdHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
