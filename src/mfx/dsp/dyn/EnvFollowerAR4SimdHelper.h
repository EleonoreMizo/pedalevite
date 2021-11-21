/*****************************************************************************

        EnvFollowerAR4SimdHelper.h
        Author: Laurent de Soras, 2016

Processes 4 envelope detectors on 4 channels simultaneously.
Takes only positive values as input (signals must have been rectified before).

Template parameters:

- VD: class writing and reading memory with SIMD vectors (destination access).
	Typically, the fstb::DataAlign classes for aligned and unaligned data.
	Requires:
	static bool VD::check_ptr (const void *ptr) noexcept;
	static fstb::Vf32 VD::load_f32 (const void *ptr) noexcept;
	static void VD::store_f32 (void *ptr, const fstb::Vf32 val) noexcept;

- VS: same as VD, but for reading only (source access)
	Requires:
	static bool VS::check_ptr (const void *ptr) noexcept;
	static fstb::Vf32 VS::load_f32 (const void *ptr) noexcept;

- VP: same as VD, but for parametering and internal data.
	Requires: same as VD.

- ORD: filter order. >= 1

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dyn_EnvFollowerAR4SimdHelper_HEADER_INCLUDED)
#define mfx_dsp_dyn_EnvFollowerAR4SimdHelper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



template <class VD, class VS, class VP, int ORD>
class EnvFollowerAR4SimdHelper
{

	static_assert ((ORD >= 1), "Filter order must be between 1 and 8.");
	static_assert ((ORD <= 8), "Filter order must be between 1 and 8.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _filter_order = ORD;
	static constexpr int _nbr_env = 4;  // Number of processing units

	typedef VD V128Dest;
	typedef VS V128Src;
	typedef VP V128Par;

	               EnvFollowerAR4SimdHelper () noexcept;
	               ~EnvFollowerAR4SimdHelper () = default;

	void           set_atk_coef (int env, float coef) noexcept;
	void           set_rls_coef (int env, float coef) noexcept;

	fstb_FORCEINLINE fstb::Vf32
	               process_sample (const fstb::Vf32 &in) noexcept;
	void           process_block (fstb::Vf32 out_ptr [], const fstb::Vf32 in_ptr [], int nbr_spl) noexcept;
	fstb::Vf32
	               analyse_block (const fstb::Vf32 in_ptr [], int nbr_spl) noexcept;
	void           process_block_1_chn (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	float	VectFloat4 [4];

	fstb_FORCEINLINE static bool
	               test_ge_0 (const fstb::Vf32 &in) noexcept;

	alignas (fstb_SIMD128_ALIGN) VectFloat4
	               _state [ORD];
	alignas (fstb_SIMD128_ALIGN) VectFloat4
	               _coef_atk;
	alignas (fstb_SIMD128_ALIGN) VectFloat4
	               _coef_rls;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EnvFollowerAR4SimdHelper (const EnvFollowerAR4SimdHelper <VD, VS, VP, ORD> &other) = delete;
	EnvFollowerAR4SimdHelper <VD, VS, VP, ORD> &
	               operator = (const EnvFollowerAR4SimdHelper <VD, VS, VP, ORD> &other)               = delete;
	bool           operator == (const EnvFollowerAR4SimdHelper <VD, VS, VP, ORD> &other) const        = delete;
	bool           operator != (const EnvFollowerAR4SimdHelper <VD, VS, VP, ORD> &other) const        = delete;

}; // class EnvFollowerAR4SimdHelper



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dyn/EnvFollowerAR4SimdHelper.hpp"



#endif   // mfx_dsp_dyn_EnvFollowerAR4SimdHelper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
