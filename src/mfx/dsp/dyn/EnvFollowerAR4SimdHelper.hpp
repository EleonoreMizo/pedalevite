/*****************************************************************************

        EnvFollowerAR4SimdHelper.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_EnvFollowerAR4SimdHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerAR4SimdHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
EnvFollowerAR4SimdHelper <VD, VS, VP>::EnvFollowerAR4SimdHelper ()
/*:	_state ()
,	_coef_atk ()
,	_coef_rls ()*/
{
	const auto     one = fstb::ToolsSimd::set1_f32 (1);
	V128Par::store_f32 (_coef_atk, one);
	V128Par::store_f32 (_coef_rls, one);

	clear_buffers ();
}



template <class VD, class VS, class VP>
void	EnvFollowerAR4SimdHelper <VD, VS, VP>::set_atk_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_atk [env] = coef;
}



template <class VD, class VS, class VP>
void	EnvFollowerAR4SimdHelper <VD, VS, VP>::set_rls_coef (int env, float coef)
{
	assert (env >= 0);
	assert (env < _nbr_env);
	assert (coef > 0);
	assert (coef <= 1);

	_coef_rls [env] = coef;
}



// in must contain only positive values!
template <class VD, class VS, class VP>
fstb::ToolsSimd::VectF32	EnvFollowerAR4SimdHelper <VD, VS, VP>::process_sample (const fstb::ToolsSimd::VectF32 &in)
{
	assert (test_ge_0 (in));

	auto           state      = V128Par::load_f32 (_state);

	const auto     zero       = fstb::ToolsSimd::set_f32_zero ();
	const auto     delta      = in - state;
	const auto     delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);

	// delta >  0 (attack)       ---> coef = _coef_atk
	// delta <= 0 (release/hold) ---> coef = _coef_rls
	const auto     coef_a     = V128Par::load_f32 (_coef_atk);
	const auto     coef_r     = V128Par::load_f32 (_coef_rls);
	const auto     coef       =
		fstb::ToolsSimd::select (delta_gt_0, coef_ax, coef_rx);

	// state += coef * (in - state)
	const auto     delta_coef = delta * coef;
	state += delta_coef;

	V128Par::store_f32 (_state, state);

	return state;
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP>
void	EnvFollowerAR4SimdHelper <VD, VS, VP>::process_block (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_address (out_ptr));
	assert (V128Src::check_address (in_ptr));
	assert (nbr_spl > 0);

	auto           state  = V128Par::load_f32 (_state);

	const auto     zero   = fstb::ToolsSimd::set_f32_zero ();
	const auto     coef_a = V128Par::load_f32 (_coef_atk);
	const auto     coef_r = V128Par::load_f32 (_coef_rls);

	long				pos = 0;
	do
	{
		const auto     in = V128Src::load_f32 (in_ptr + pos);
		assert (test_ge_0 (in));

		const auto     delta      = in - state;
		const auto     delta_gt_0 = fstb::ToolsSimd::cmp_gt_f32 (delta, zero);

		// delta >  0 (attack)       ---> coef = _coef_atk
		// delta <= 0 (release/hold) ---> coef = _coef_rls
		const auto     coef       =
			fstb::ToolsSimd::select (delta_gt_0, coef_ax, coef_rx);

		// state += coef * (in - state)
		const auto     delta_coef = delta * coef;
		state += delta_coef;

		V128Dest::store_f32 (out_ptr + pos, state);

		++ pos;
	}
	while (pos < nbr_spl);

	V128Par::store_f32 (_state, state);
}



// Input data must contain only positive values!
// Can work in-place.
template <class VD, class VS, class VP>
void	EnvFollowerAR4SimdHelper <VD, VS, VP>::process_block_1_chn (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (V128Dest::check_ptr (out_ptr));
	assert (V128Src::check_ptr (in_ptr));
	assert (nbr_spl > 0);

	float				state = _state [0];

	long				pos = 0;
	do
	{
		const float		in = in_ptr [pos];
		assert (in >= 0);

		const float		delta = in - state;
		const float		coef = (delta > 0) ? _coef_atk [0] : _coef_rls [0];
		state += delta * coef;

		out_ptr [pos] = state;

		++ pos;
	}
	while (pos < nbr_spl);

	_state [0] = state;
}



template <class VD, class VS, class VP>
void	EnvFollowerAR4SimdHelper <VD, VS, VP>::clear_buffers ()
{
	V128Par::store_f32 (_state, fstb::ToolsSimd::set_f32_zero ());
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
bool	EnvFollowerAR4SimdHelper <VD, VS, VP>::test_ge_0 (const fstb::ToolsSimd::VectF32 &in)
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



#endif   // mfx_dsp_dyn_EnvFollowerAR4SimdHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
