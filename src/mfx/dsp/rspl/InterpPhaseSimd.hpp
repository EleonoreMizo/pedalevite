/*****************************************************************************

        InterpPhaseSimd.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpPhaseSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpPhaseSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/ToolsSimd.h"



#if defined (_MSC_VER)
	#pragma inline_depth (255)
#endif



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <int REM>
class InterpPhaseSimd_Util
{
public:
	static fstb_FORCEINLINE void
						sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr []);
	static fstb_FORCEINLINE void
						lerp_imp (float lerp_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr []);
	static fstb_FORCEINLINE void
						sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const float lerp_ptr []);
};



template <int REM>
inline void	InterpPhaseSimd_Util <REM>::sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr [])
{
	const int      offset = (nbr_blocks - REM) * 4;
	auto           tmp    = fstb::ToolsSimd::load_f32 (&imp_ptr [offset]);
	const auto     dif    = fstb::ToolsSimd::load_f32 (&dif_ptr [offset]);
	const auto     val    = fstb::ToolsSimd::loadu_f32 (&data_ptr [offset]);
	fstb::ToolsSimd::mac (tmp, dif, q_v);
	fstb::ToolsSimd::mac (sum_v, val, tmp);

	InterpPhaseSimd_Util <REM - 1>::sum_rec (
		nbr_blocks,
		sum_v,
		data_ptr,
		q_v,
		imp_ptr,
		dif_ptr
	);
}

template <>
inline void	InterpPhaseSimd_Util <0>::sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr [])
{
	fstb::unused (nbr_blocks, sum_v, data_ptr, q_v, imp_ptr, dif_ptr);
	// Nothing, stops the recursion
}



template <int REM>
inline void	InterpPhaseSimd_Util <REM>::lerp_imp (float lerp_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr [])
{
	InterpPhaseSimd_Util <REM - 1>::lerp_imp (
		lerp_ptr,
		q_v,
		imp_ptr,
		dif_ptr
	);

	const int      offset = (REM - 1) * 4;
	auto           tmp    = fstb::ToolsSimd::load_f32 (&imp_ptr [offset]);
	const auto     dif    = fstb::ToolsSimd::load_f32 (&dif_ptr [offset]);
	fstb::ToolsSimd::mac (tmp, dif, q_v);
	fstb::ToolsSimd::store_f32 (&lerp_ptr [offset], tmp);
}

template <>
inline void	InterpPhaseSimd_Util <0>::lerp_imp (float lerp_ptr [], const fstb::ToolsSimd::VectF32 &q_v, const float imp_ptr [], const float dif_ptr [])
{
	fstb::unused (lerp_ptr, q_v, imp_ptr, dif_ptr);
	// Nothing, stops the recursion
}



template <int REM>
inline void	InterpPhaseSimd_Util <REM>::sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const float lerp_ptr [])
{
	const int      offset = (nbr_blocks - REM) * 4;
	const auto     val    = fstb::ToolsSimd::loadu_f32 (&data_ptr [offset]);
	const auto     pulse  = fstb::ToolsSimd::loadu_f32 (&lerp_ptr [offset]);

	fstb::ToolsSimd::mac (sum_v, val, pulse);

	InterpPhaseSimd_Util <REM - 1>::sum_rec (
		nbr_blocks,
		sum_v,
		data_ptr,
		lerp_ptr
	);
}

template <>
inline void	InterpPhaseSimd_Util <0>::sum_rec (const int nbr_blocks, fstb::ToolsSimd::VectF32 &sum_v, const float data_ptr [], const float lerp_ptr [])
{
	fstb::unused (nbr_blocks, sum_v, data_ptr, lerp_ptr);
	// Nothing, stops the recursion
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int PL>
void	InterpPhaseSimd <PL>::set_data (int pos, float imp, float dif)
{
	assert (pos >= 0);
	assert (pos < PHASE_LEN);

	_imp [pos] = imp;
	_dif [pos] = dif;
}



template <int PL>
void	InterpPhaseSimd <PL>::precompute_impulse (Buffer &imp, float q) const
{
	assert (&imp != 0);
	assert (q >= 0);
	assert (q <= 1);

	const auto     q_v = fstb::ToolsSimd::set1_f32 (q);
	InterpPhaseSimd_Util <PHASE_LEN_V128>::lerp_imp (&imp [0], q_v, &_imp [0], &_dif [0]);
}



template <int PL>
float	InterpPhaseSimd <PL>::convolve (const float data_ptr [], const Buffer &imp) const
{
	assert (data_ptr != 0);
	assert (&imp != 0);

	// First vector
	const auto     val   = fstb::ToolsSimd::loadu_f32 (&data_ptr [0]);
	const auto     pulse = fstb::ToolsSimd::load_f32 (&imp [0]);
	auto           sum_v = val * pulse;

	// Other vectors
	InterpPhaseSimd_Util <PHASE_LEN_V128 - 1>::sum_rec (
		PHASE_LEN_V128,
		sum_v,
		data_ptr,
		&imp [0]
	);

	return fstb::ToolsSimd::sum_h_flt (sum_v);
}



template <int PL>
float	InterpPhaseSimd <PL>::convolve (const float data_ptr [], float q) const
{
	assert (_imp [0] != CHK_IMPULSE_NOT_SET);
	assert (data_ptr != 0);
	assert (q >= 0);
	assert (q <= 1);

	const auto     q_v = fstb::ToolsSimd::set1_f32 (q);
	fstb::ToolsSimd::VectF32   sum_v;

#if 1

	// First vector
	auto           tmp = fstb::ToolsSimd::load_f32 (&_imp [0]);
	const auto     dif = fstb::ToolsSimd::load_f32 (&_dif [0]);
	const auto     val = fstb::ToolsSimd::loadu_f32 (&data_ptr [0]);
	fstb::ToolsSimd::mac (tmp, dif, q_v);
	sum_v = val * tmp;

	// Other vectors
	InterpPhaseSimd_Util <PHASE_LEN_V128 - 1>::sum_rec (
		PHASE_LEN_V128,
		sum_v,
		data_ptr,
		q_v,
		&_imp [0],
		&_dif [0]
	);

#else

	// Naive implementation (slow)
	sum_v = fstb::ToolsSimd::set_f32_zero ();
	int				k = 0;
	do
	{
		auto           imp = fstb::ToolsSimd::load_f32 (&_imp [k]);
		const __m128	dif = fstb::ToolsSimd::load_f32 (&_dif [k]);
		const __m128	val = fstb::ToolsSimd::loadu_f32 (&data_ptr [k]);
		fstb::ToolsSimd::mac (tmp, dif, q_v);
		fstb::ToolsSimd::mac (sum_v, val, tmp);
		k += 4;
	}
	while (k < ARRAY_LEN);

#endif

	return fstb::ToolsSimd::sum_h_flt (sum_v);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpPhaseSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
