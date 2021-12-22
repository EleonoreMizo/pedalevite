/*****************************************************************************

        Upsampler8xSimd.h
        Author: Laurent de Soras, 2016

This object must be aligned on a 16-byte boundary!

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Upsampler8xSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_Upsampler8xSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include "hiir/Upsampler2xFpu.h"
#if defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_X86
	#include "hiir/Upsampler2xSse.h"
#elif defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_ARM
	#include "hiir/Upsampler2xNeon.h"
	#include "hiir/Upsampler2xNeonOld.h"
#endif

#include <type_traits>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int NC84, int NC42, int NC21>
class Upsampler8xSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_coefs (const double coef_84 [NC84], const double coef_42 [NC42], const double coef_21 [NC21]) noexcept;

	inline void    process_sample (float dst_ptr [8], float src) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	inline void    process_sample_4x (float dst_ptr [4], float src) noexcept;
	void           process_block_4x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	inline void    process_sample_2x (float dst_ptr [2], float src) noexcept;
	void           process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_X86
	template <int NC>
	using Upspl = typename std::conditional <
		(NC >= 1) // Current SSE version is always faster than the FPU
	,	hiir::Upsampler2xSse <NC>
	,	hiir::Upsampler2xFpu <NC>
	>::type;
#elif defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_ARM && (defined (__clang__) || fstb_WORD_SIZE == 64)
	template <int NC>
	using Upspl = typename std::conditional <
		(NC >= 14)
	,	hiir::Upsampler2xNeonOld <NC>
	,	hiir::Upsampler2xNeon <NC>
	>::type;
#else
	template <int NC>
	using Upspl = hiir::Upsampler2xFpu <NC>;
#endif

	using Upspl84 = Upspl <NC84>;
	using Upspl42 = Upspl <NC42>;
	using Upspl21 = Upspl <NC21>;

	Upspl84        _us_84;
	Upspl42        _us_42;
	Upspl21        _us_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Upsampler8xSimd &other) const = delete;
	bool           operator != (const Upsampler8xSimd &other) const = delete;

}; // class Upsampler8xSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Upsampler8xSimd.hpp"



#endif   // mfx_dsp_iir_Upsampler8xSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
