/*****************************************************************************

        Downsampler4xSimd.h
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
#if ! defined (mfx_dsp_iir_Downsampler4xSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_Downsampler4xSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include "hiir/Downsampler2xFpu.h"
#if defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_X86
	#include "hiir/Downsampler2xSse.h"
#elif defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_ARM
	#include "hiir/Downsampler2xNeon.h"
	#include "hiir/Downsampler2xNeonOld.h"
#endif

#include <type_traits>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <int NC42, int NC21>
class Downsampler4xSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_coefs (const double coef_42 [NC42], const double coef_21 [NC21]) noexcept;

	inline float   process_sample (const float src_ptr [4]) noexcept;
	void           process_block (float data_ptr [], int nbr_spl) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	inline float   process_sample_2x (const float src_ptr [2]) noexcept;
	void           process_block_2x (float data_ptr [], int nbr_spl) noexcept;
	void           process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _buf_len = 64;

#if defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_X86
	template <int NC>
	using Dwnspl = typename std::conditional <
		(NC >= 1) // Current SSE version is always faster than the FPU
	,	hiir::Downsampler2xSse <NC>
	,	hiir::Downsampler2xFpu <NC>
	>::type;
#elif defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_ARM && (defined (__clang__) || fstb_WORD_SIZE == 64)
	template <int NC>
	using Dwnspl = typename std::conditional <
		(NC >= 16)
	,	hiir::Downsampler2xNeonOld <NC>
	,	hiir::Downsampler2xNeon <NC>
	>::type;
#else
	template <int NC>
	using Dwnspl = hiir::Downsampler2xFpu <NC>;
#endif

	using Dwnspl42 = Dwnspl <NC42>;
	using Dwnspl21 = Dwnspl <NC21>;

	Dwnspl42       _ds_42;
	Dwnspl21       _ds_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Downsampler4xSimd &other) const = delete;
	bool           operator != (const Downsampler4xSimd &other) const = delete;

}; // class Downsampler4xSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Downsampler4xSimd.hpp"



#endif   // mfx_dsp_iir_Downsampler4xSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
