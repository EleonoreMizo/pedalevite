/*****************************************************************************

        ToolsAvx2.h
        Author: Laurent de Soras, 2015

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_ToolsAvx2_HEADER_INCLUDED)
#define	fstb_ToolsAvx2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <immintrin.h>

#include <cstdint>



namespace fstb
{



class ToolsAvx2
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class T>
	static fstb_FORCEINLINE bool
	               check_ptr_align (T *ptr);

	static fstb_FORCEINLINE __m256i
	               load_16_16ml (const void *msb_ptr, const void *lsb_ptr);
	static fstb_FORCEINLINE __m256i
	               load_16_16m (const void *msb_ptr);
	static fstb_FORCEINLINE __m256i
	               load_16_16l (const void *lsb_ptr);
	static fstb_FORCEINLINE __m256i
	               load_16_16ml_partial (const void *msb_ptr, const void *lsb_ptr, int len);
	static fstb_FORCEINLINE __m256i
	               load_16_16m_partial (const void *msb_ptr, int len);
	static fstb_FORCEINLINE __m256i
	               load_16_16l_partial (const void *lsb_ptr, int len);
	static fstb_FORCEINLINE void
	               store_16_16ml (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb);
	static fstb_FORCEINLINE void
	               store_16_16ml_perm (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, __m256i permute);
	static fstb_FORCEINLINE void
	               store_16_16m (void *msb_ptr, __m256i val, __m256i mask_lsb);
	static fstb_FORCEINLINE void
	               store_16_16l (void *lsb_ptr, __m256i val, __m256i mask_lsb);
	static fstb_FORCEINLINE void
	               store_16_16ml_partial (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, int len);
	static fstb_FORCEINLINE void
	               store_16_16ml_perm_partial (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, __m256i permute, int len);
	static fstb_FORCEINLINE void
	               store_16_16m_partial (void *msb_ptr, __m256i val, __m256i mask_lsb, int len);
	static fstb_FORCEINLINE void
	               store_16_16l_partial (void *lsb_ptr, __m256i val, __m256i mask_lsb, int len);

	static fstb_FORCEINLINE __m256
	               load_ps_partial (const void *ptr, int len);
	static fstb_FORCEINLINE __m256i
	               load_si256_partial (const void *ptr, int len);
	static fstb_FORCEINLINE void
	               store_ps_partial (void *ptr, __m256 val, int len);
	static fstb_FORCEINLINE void
	               store_si256_partial (void *ptr, __m256i val, int len);

	static fstb_FORCEINLINE void
	               mac_s16_s16_s32 (__m256i &dst0, __m256i &dst1, __m256i src, __m256i coef);
	static fstb_FORCEINLINE __m256i
	               mullo_epi32 (const __m256i &a, const __m256i &b);
	static fstb_FORCEINLINE __m256i
	               pack_epi16 (__m256i a, __m256i b);
	static fstb_FORCEINLINE __m256i
	               select (const __m256i &cond, const __m256i &v_t, const __m256i &v_f);
	static fstb_FORCEINLINE __m256
	               select (const __m256 &cond, const __m256 &v_t, const __m256 &v_f);
	static fstb_FORCEINLINE __m256i
	               select_16_equ (const __m256i &lhs, const __m256i &rhs, const __m256i &v_t, const __m256i &v_f);
	static fstb_FORCEINLINE __m256i
	               limit_epi16 (const __m256i &x, const __m256i &mi, const __m256i &ma);
	static fstb_FORCEINLINE __m256i
	               abs_dif_epu16 (const __m256i &a, const __m256i &b);
	static fstb_FORCEINLINE __m256i
	               abs_dif_epi16 (const __m256i &a, const __m256i &b);

	fstb_TYPEDEF_ALIGN (32, uint8_t , VectI08 [32]);
	fstb_TYPEDEF_ALIGN (32, uint32_t, VectI32 [ 8]);
	fstb_TYPEDEF_ALIGN (32, float   , VectF32 [ 8]);

	static const VectI32
	               _zero;
	static const VectI32
	               _c16_8000;
	static const VectI32
	               _c32_00008000;
	static const VectI32
	               _c32_0000ffff;

	static const VectI32
	               _mask_abs;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToolsAvx2 ()                               = delete;
	               ToolsAvx2 (const ToolsAvx2 &other)         = delete;
	virtual        ~ToolsAvx2 ()                              = delete;
	ToolsAvx2 &    operator = (const ToolsAvx2 &other)        = delete;
	bool           operator == (const ToolsAvx2 &other) const = delete;
	bool           operator != (const ToolsAvx2 &other) const = delete;

};	// class ToolsAvx2



}	// namespace fstb



#include "fstb/ToolsAvx2.hpp"



#endif	// fstb_ToolsAvx2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
