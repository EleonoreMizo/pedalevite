/*****************************************************************************

        Upsampler4xSimd.h
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
#if ! defined (mfx_dsp_iir_Upsampler4xSimd_HEADER_INCLUDED)
#define mfx_dsp_iir_Upsampler4xSimd_HEADER_INCLUDED

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



template <int NC42, int NC21>
class Upsampler4xSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_coefs (const double coef_42 [NC42], const double coef_21 [NC21]);

	inline void    process_sample (float dst_ptr [4], float src);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

	inline void    process_sample_2x (float dst_ptr [2], float src);
	void           process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_X86
	using Upspl42 = typename std::conditional <
		(NC42 >= 4)
	,	hiir::Upsampler2xSse <NC42>
	,	hiir::Upsampler2xFpu <NC42>
	>::type;
	using Upspl21 = typename std::conditional <
		(NC21 >= 4)
	,	hiir::Upsampler2xSse <NC21>
	,	hiir::Upsampler2xFpu <NC21>
	>::type;
#elif defined (fstb_HAS_SIMD) && fstb_ARCHI == fstb_ARCHI_ARM && (defined (__clang__) || fstb_WORD_SIZE == 64)
	using Upspl42 = typename std::conditional <
		(NC42 >= 12)
	,	hiir::Upsampler2xNeonOld <NC42>
	,	hiir::Upsampler2xNeon <NC42>
	>::type;
	using Upspl21 = typename std::conditional <
		(NC21 >= 12)
	,	hiir::Upsampler2xNeonOld <NC42>
	,	hiir::Upsampler2xNeon <NC42>
	>::type;
#else
	typedef hiir::Upsampler2xFpu <NC42> Upspl42;
	typedef hiir::Upsampler2xFpu <NC21> Upspl21;
#endif

	Upspl42        _us_42;
	Upspl21        _us_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Upsampler4xSimd &other) const = delete;
	bool           operator != (const Upsampler4xSimd &other) const = delete;

}; // class Upsampler4xSimd



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Upsampler4xSimd.hpp"



#endif   // mfx_dsp_iir_Upsampler4xSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
