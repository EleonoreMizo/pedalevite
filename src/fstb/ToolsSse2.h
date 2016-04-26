/*****************************************************************************

        ToolsSse2.h
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_ToolsSse2_HEADER_INCLUDED)
#define	fstb_ToolsSse2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <emmintrin.h>

#include <cstdint>



namespace fstb
{



class ToolsSse2
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class T>
	static fstb_FORCEINLINE bool
	               check_ptr_align (T *ptr);

	static fstb_FORCEINLINE __m128i
	               load_8_16ml (const void *msb_ptr, const void *lsb_ptr);
	static fstb_FORCEINLINE __m128i
	               load_8_16m (const void *msb_ptr, __m128i zero);
	static fstb_FORCEINLINE __m128i
	               load_8_16l (const void *lsb_ptr, __m128i zero);
	static fstb_FORCEINLINE __m128i
	               load_8_16ml_partial (const void *msb_ptr, const void *lsb_ptr, int len);
	static fstb_FORCEINLINE __m128i
	               load_8_16m_partial (const void *msb_ptr, __m128i zero, int len);
	static fstb_FORCEINLINE __m128i
	               load_8_16l_partial (const void *lsb_ptr, __m128i zero, int len);
	static fstb_FORCEINLINE void
	               store_8_16ml (void *msb_ptr, void *lsb_ptr, __m128i val, __m128i mask_lsb);
	static fstb_FORCEINLINE void
	               store_8_16m (void *msb_ptr, __m128i val, __m128i mask_lsb);
	static fstb_FORCEINLINE void
	               store_8_16l (void *lsb_ptr, __m128i val, __m128i mask_lsb);
	static fstb_FORCEINLINE void
	               store_8_16ml_partial (void *msb_ptr, void *lsb_ptr, __m128i val, __m128i mask_lsb, int len);
	static fstb_FORCEINLINE void
	               store_8_16m_partial (void *msb_ptr, __m128i val, __m128i mask_lsb, int len);
	static fstb_FORCEINLINE void
	               store_8_16l_partial (void *lsb_ptr, __m128i val, __m128i mask_lsb, int len);

	static fstb_FORCEINLINE __m128
	               load_ps_partial (const void *ptr, int len);
	static fstb_FORCEINLINE __m128i
	               load_si128_partial (const void *ptr, int len);
	static fstb_FORCEINLINE __m128i
	               load_epi64_partial (const void *ptr, int len);
	static fstb_FORCEINLINE void
	               store_ps_partial (void *ptr, __m128 val, int len);
	static fstb_FORCEINLINE void
		            store_si128_partial (void *ptr, __m128i val, int len);
	static fstb_FORCEINLINE void
		            store_epi64_partial (void *ptr, __m128i val, int len);

	static fstb_FORCEINLINE void
	               add_x16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i msw);
	static fstb_FORCEINLINE void
	               mul_s16_s16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i coef);
	static fstb_FORCEINLINE __m128i
	               mul_s32_s15_s16 (__m128i src0, __m128i src1, __m128i coef);
	static fstb_FORCEINLINE void
	               mac_s16_s16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i coef);
	static fstb_FORCEINLINE __m128i
	               mullo_epi32 (const __m128i &a, const __m128i &b);
	static fstb_FORCEINLINE __m128i
	               pack_epi16 (__m128i a, __m128i b);
	static fstb_FORCEINLINE __m128i
	               select (const __m128i &cond, const __m128i &v_t, const __m128i &v_f);
	static fstb_FORCEINLINE __m128
	               select (const __m128 &cond, const __m128 &v_t, const __m128 &v_f);
	static fstb_FORCEINLINE __m128i
	               select_16_equ (const __m128i &lhs, const __m128i &rhs, const __m128i &v_t, const __m128i &v_f);
	static fstb_FORCEINLINE __m128i
	               limit_epi16 (const __m128i &x, const __m128i &mi, const __m128i &ma);
	static fstb_FORCEINLINE __m128i
	               abs_dif_epu16 (const __m128i &a, const __m128i &b);
	static fstb_FORCEINLINE __m128i
	               abs_dif_epi16 (const __m128i &a, const __m128i &b);

	fstb_TYPEDEF_ALIGN (16, uint8_t , VectI08 [16]);
	fstb_TYPEDEF_ALIGN (16, uint16_t, VectI16 [ 8]);
	fstb_TYPEDEF_ALIGN (16, uint32_t, VectI32 [ 4]);
	fstb_TYPEDEF_ALIGN (16, uint64_t, VectI64 [ 2]);
	fstb_TYPEDEF_ALIGN (16, float   , VectF32 [ 4]);

	static const VectI32
	               _zero;
	static const VectI32
	               _c16_8000;
	static const VectI32
	               _c32_00008000;
	static const VectI32
	               _c32_0000ffff;
	static const VectI32
	               _mask_lo64;

	static const VectI32
	               _mask_abs;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToolsSse2 ()                               = delete;
	               ToolsSse2 (const ToolsSse2 &other)         = delete;
	virtual        ~ToolsSse2 ()                              = delete;
	ToolsSse2 &    operator = (const ToolsSse2 &other)        = delete;
	bool           operator == (const ToolsSse2 &other) const = delete;
	bool           operator != (const ToolsSse2 &other) const = delete;

};	// class ToolsSse2



}	// namespace fstb



#include "fstb/ToolsSse2.hpp"



#endif	// fstb_ToolsSse2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
