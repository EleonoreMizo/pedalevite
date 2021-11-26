/*****************************************************************************

        EnvFollowerAHR1LrSimdHelper.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <utility>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VP, int ORD>
EnvFollowerAHR1LrSimdHelper <VP, ORD>::EnvFollowerAHR1LrSimdHelper () noexcept
:	_state ()
,	_hold_state ()
,	_reset_mask ()
,	_coef_atk (1)
,	_coef_rls (1)
,	_hold_time (0)
,	_hold_t_q (0)
,	_hold_counter (0)
{
	clear_buffers ();
}



template <class VP, int ORD>
EnvFollowerAHR1LrSimdHelper <VP, ORD>::EnvFollowerAHR1LrSimdHelper (const EnvFollowerAHR1LrSimdHelper <VP, ORD> &other) noexcept
:	_state ()
,	_hold_state ()
,	_reset_mask ()
,	_coef_atk (other._coef_atk)
,	_coef_rls (other._coef_rls)
,	_hold_time (other._hold_time)
,	_hold_t_q (other._hold_t_q)
,	_hold_counter (other._hold_counter)
{
	copy_vect_data (other);
}



template <class VP, int ORD>
EnvFollowerAHR1LrSimdHelper <VP, ORD>::EnvFollowerAHR1LrSimdHelper (EnvFollowerAHR1LrSimdHelper <VP, ORD> &&other) noexcept
:	_state ()
,	_hold_state ()
,	_reset_mask ()
,	_coef_atk (std::move (other._coef_atk))
,	_coef_rls (std::move (other._coef_rls))
,	_hold_time (std::move (other._hold_time))
,	_hold_t_q (std::move (other._hold_t_q))
,	_hold_counter (std::move (other._hold_counter))
{
	copy_vect_data (other);
}



template <class VP, int ORD>
EnvFollowerAHR1LrSimdHelper <VP, ORD> &	EnvFollowerAHR1LrSimdHelper <VP, ORD>::operator = (const EnvFollowerAHR1LrSimdHelper <VP, ORD> &other) noexcept
{
	if (this != &other)
	{
		copy_vect_data (other);
		_coef_atk     = other._coef_atk;
		_coef_rls     = other._coef_rls;
		_hold_time    = other._hold_time;
		_hold_t_q     = other._hold_t_q;
		_hold_counter = other._hold_counter;
	}

	return *this;
}



template <class VP, int ORD>
EnvFollowerAHR1LrSimdHelper <VP, ORD> &	EnvFollowerAHR1LrSimdHelper <VP, ORD>::operator = (EnvFollowerAHR1LrSimdHelper <VP, ORD> &&other) noexcept
{
	if (this != &other)
	{
		copy_vect_data (other);
		_coef_atk     = std::move (other._coef_atk);
		_coef_rls     = std::move (other._coef_rls);
		_hold_time    = std::move (other._hold_time);
		_hold_t_q     = std::move (other._hold_t_q);
		_hold_counter = std::move (other._hold_counter);
	}

	return *this;
}



template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::set_atk_coef (float coef) noexcept
{
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk = coef;
}



// 4-samples accuracy
template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::set_hold_time (int nbr_spl) noexcept
{
	assert (nbr_spl >= 0);

	_hold_time = nbr_spl;
	_hold_t_q  = (nbr_spl + (_nbr_sub >> 1)) / _nbr_sub;
}



template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::set_rls_coef (float coef) noexcept
{
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls = coef;
}



// in must contain only positive values!
template <class VP, int ORD>
float	EnvFollowerAHR1LrSimdHelper <VP, ORD>::process_sample (float in) noexcept
{
	assert (in >= 0);

	auto           x      = fstb::Vf32 (in);

	const auto     zero   = fstb::Vf32::zero ();
	const auto     coef_a = fstb::Vf32 (_coef_atk);
	const auto     coef_r = fstb::Vf32 (_coef_rls);

	auto           state      = V128Par::load_f32 (_state [0]);
	auto           hold_state = V128Par::load_f32 (_hold_state);
	auto           reset_mask = V128Par::load_f32 (_reset_mask);

	auto           delta      = x - state;
	auto           delta_gt_0 = (delta > zero);

	hold_state |= delta_gt_0;

	// delta >  0 (attack)       ---> coef = _coef_atk
	// delta <= 0 (release/hold) ---> coef = _coef_rls
	const auto     coef_r_cur =
		fstb::ToolsSimd::select (hold_state, zero, coef_r);
	auto           coef       =
		fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

	// state += coef * (x - state)
	state.mac (delta, coef);

	V128Par::store_f32 (_state [0], state);

	check_and_reset (hold_state, 1);
	V128Par::store_f32 (_hold_state, hold_state);

	for (int flt = 1; flt < ORD; ++flt)
	{
		const auto     prev = state;
		state = V128Par::load_f32 (_state [flt]);
		delta = prev - state;

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls
		const auto     delta_lt_0 = delta.is_lt_0 ();
		coef = fstb::ToolsSimd::select (delta_lt_0, coef_r, coef_a);

		// state += coef * (x - state)
		state.mac (delta, coef);
		V128Par::store_f32 (_state [flt], state);
	}

	return state.max_h ();
}



#define mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD( fltn) \
	fstb::Vf32     state##fltn; \
	if (fltn - 1 < ORD) \
	{ \
		state##fltn = V128Par::load_f32 (_state [fltn - 1]); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC( flt, fltn) \
	if (flt < ORD) \
	{ \
		delta = state##flt - state##fltn; \
		const auto     delta_lt_0 = delta.is_lt_0 (); \
		coef  = fstb::ToolsSimd::select (delta_lt_0, coef_r, coef_a); \
		state##fltn.mac (delta, coef); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT( ord) \
	if (ord == ORD) \
	{ \
		out_ptr [pos] = state##ord.max_h (); \
	}
#define mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE( fltn) \
	if (fltn - 1 < ORD) \
	{ \
		V128Par::store_f32 (_state [fltn - 1],state##fltn); \
	}

// Input data must contain only positive values!
// Can work in-place.
template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::process_block (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_spl > 0);

	const auto     zero   = fstb::Vf32::zero ();
	const auto     coef_a = fstb::Vf32 (_coef_atk);
	const auto     coef_r = fstb::Vf32 (_coef_rls);

	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (1)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (2)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (3)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (4)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (5)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (6)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (7)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD (8)
	auto           hold_state = V128Par::load_f32 (_hold_state);

	int            pos = 0;
	do
	{
		int            work_len = nbr_spl - pos;
		int            rem_spl  = std::max (_hold_t_q - _hold_counter, 1);
		work_len = std::min (work_len, rem_spl);
		const int      stop_pos = pos + work_len;

		do
		{
			const auto     state0 = fstb::Vf32 (in_ptr [pos]);
			assert (test_ge_0 (state0));

			const auto     coef_r_cur =
				fstb::ToolsSimd::select (hold_state, zero, coef_r);

			auto           delta      = state0 - state1;

			// delta >  0 (attack)       ---> coef = _coef_atk
			// delta <= 0 (release/hold) ---> coef = _coef_rls or 0
			auto           delta_gt_0 = (delta > zero);
			hold_state |= delta_gt_0;
			auto           coef       =
				fstb::ToolsSimd::select (delta_gt_0, coef_a, coef_r_cur);

			// state += coef * (in - state)
			state1.mac (delta, coef);

			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (1, 2)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (2, 3)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (3, 4)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (4, 5)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (5, 6)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (6, 7)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC (7, 8)

			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (1)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (2)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (3)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (4)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (5)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (6)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (7)
			mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT (8)

			++ pos;
		}
		while (pos < stop_pos);

		check_and_reset (hold_state, work_len);
	}
	while (pos < nbr_spl);

	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (1)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (2)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (3)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (4)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (5)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (6)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (7)
	mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE (8)

	V128Par::store_f32 (_hold_state, hold_state);
}

#undef mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_LOAD
#undef mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_PROC
#undef mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_RESULT
#undef mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_SAVE



template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::clear_buffers () noexcept
{
	for (int flt = 0; flt < ORD; ++flt)
	{
		V128Par::store_f32 (_state [flt], fstb::Vf32::zero ());
	}
	V128Par::store_f32 (_hold_state, fstb::Vf32::zero ());
	V128Par::store_f32 (
		_reset_mask,
		fstb::Vf32::set_mask (false, true, true, true)
	);
	_hold_counter = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::copy_vect_data (const EnvFollowerAHR1LrSimdHelper <VP, ORD> &other) noexcept
{
	for (int flt = 0; flt < ORD; ++flt)
	{
		V128Par::store_f32 (_state [flt], V128Par::load_f32 (other._state [flt]));
	}
	V128Par::store_f32 (_hold_state, V128Par::load_f32 (other._hold_state));
	V128Par::store_f32 (_reset_mask, V128Par::load_f32 (other._reset_mask));
}



template <class VP, int ORD>
void	EnvFollowerAHR1LrSimdHelper <VP, ORD>::check_and_reset (fstb::Vf32 &hold_state, int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_hold_counter += nbr_spl;
	if (_hold_counter >= _hold_t_q)
	{
		_hold_counter = 0;

		auto           mask = V128Par::load_f32 (_reset_mask);
		hold_state &= mask;
		mask        = fstb::ToolsSimd::Shift <1>::rotate (mask);
		V128Par::store_f32 (_reset_mask, mask);
	}
}



template <class VP, int ORD>
bool	EnvFollowerAHR1LrSimdHelper <VP, ORD>::test_ge_0 (const fstb::Vf32 &in) noexcept
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



#endif   // mfx_dsp_dyn_EnvFollowerAHR1LrSimdHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
