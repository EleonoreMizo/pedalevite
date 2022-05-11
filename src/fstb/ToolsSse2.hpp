/*****************************************************************************

        ToolsSse2.hpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ToolsSse2_CODEHEADER_INCLUDED)
#define	fstb_ToolsSse2_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cassert>
#include <cstddef>
#include <cstdint>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
inline bool	ToolsSse2::check_ptr_align (T *ptr)
{
	return (ptr != nullptr && (reinterpret_cast <intptr_t> (ptr) & 15) == 0);
}



__m128i	ToolsSse2::load_8_16ml (const void *msb_ptr, const void *lsb_ptr)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);

	const __m128i  val_msb = _mm_loadl_epi64 (
		reinterpret_cast <const __m128i *> (msb_ptr)
	);
	const __m128i  val_lsb = _mm_loadl_epi64 (
		reinterpret_cast <const __m128i *> (lsb_ptr)
	);
	const __m128i  val = _mm_unpacklo_epi8 (val_lsb, val_msb);

	return val;
}



__m128i	ToolsSse2::load_8_16m (const void *msb_ptr, __m128i zero)
{
	assert (msb_ptr != nullptr);

	const __m128i  val_msb = _mm_loadl_epi64 (
		reinterpret_cast <const __m128i *> (msb_ptr)
	);
	const __m128i  val = _mm_unpacklo_epi8 (zero, val_msb);

	return val;
}



__m128i	ToolsSse2::load_8_16l (const void *lsb_ptr, __m128i zero)
{
	assert (lsb_ptr != nullptr);

	const __m128i  val_lsb = _mm_loadl_epi64 (
		reinterpret_cast <const __m128i *> (lsb_ptr)
	);
	const __m128i  val = _mm_unpacklo_epi8 (val_lsb, zero);

	return val;
}



__m128i	ToolsSse2::load_8_16ml_partial (const void *msb_ptr, const void *lsb_ptr, int len)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	const __m128i  val_msb = load_epi64_partial (msb_ptr, len);
	const __m128i  val_lsb = load_epi64_partial (lsb_ptr, len);
	const __m128i  val = _mm_unpacklo_epi8 (val_lsb, val_msb);

	return val;
}



__m128i	ToolsSse2::load_8_16m_partial (const void *msb_ptr, __m128i zero, int len)
{
	assert (msb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	const __m128i  val_msb = load_epi64_partial (msb_ptr, len);
	const __m128i  val = _mm_unpacklo_epi8 (zero, val_msb);

	return val;
}



__m128i	ToolsSse2::load_8_16l_partial (const void *lsb_ptr, __m128i zero, int len)
{
	assert (lsb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	const __m128i  val_lsb = load_epi64_partial (lsb_ptr, len);
	const __m128i  val = _mm_unpacklo_epi8 (val_lsb, zero);

	return val;
}



// mask_lsb = 0x00FF00FF00FF00FF00FF00FF00FF00FF
void	ToolsSse2::store_8_16ml (void *msb_ptr, void *lsb_ptr, __m128i val, __m128i mask_lsb)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);

	const __m128i  lsb = _mm_and_si128 (mask_lsb, val);
	__m128i        msb = _mm_andnot_si128 (mask_lsb, val);
	msb = _mm_srli_si128 (msb, 1);

	__m128i        tmp = _mm_packus_epi16 (lsb, msb);
	_mm_storel_epi64 (static_cast <__m128i *> (lsb_ptr), tmp);

	tmp = _mm_unpackhi_epi64 (tmp, tmp);
	_mm_storel_epi64 (static_cast <__m128i *> (msb_ptr), tmp);
}



// mask_lsb = 0x00FF00FF00FF00FF00FF00FF00FF00FF
void	ToolsSse2::store_8_16m (void *msb_ptr, __m128i val, __m128i mask_lsb)
{
	assert (msb_ptr != nullptr);

	__m128i        msb = _mm_andnot_si128 (mask_lsb, val);
	msb = _mm_srli_si128 (msb, 1);
	msb = _mm_packus_epi16 (msb, msb);
	_mm_storel_epi64 (static_cast <__m128i *> (msb_ptr), msb);
}



// mask_lsb = 0x00FF00FF00FF00FF00FF00FF00FF00FF
void	ToolsSse2::store_8_16l (void *lsb_ptr, __m128i val, __m128i mask_lsb)
{
	assert (lsb_ptr != nullptr);

	__m128i        lsb = _mm_and_si128 (mask_lsb, val);
	lsb = _mm_packus_epi16 (lsb, lsb);
	_mm_storel_epi64 (static_cast <__m128i *> (lsb_ptr), lsb);
}



void	ToolsSse2::store_8_16ml_partial (void *msb_ptr, void *lsb_ptr, __m128i val, __m128i mask_lsb, int len)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);

	const __m128i  lsb = _mm_and_si128 (mask_lsb, val);
	__m128i        msb = _mm_andnot_si128 (mask_lsb, val);
	msb = _mm_srli_si128 (msb, 1);

	__m128i        tmp = _mm_packus_epi16 (lsb, msb);
	store_epi64_partial (lsb_ptr, tmp, len);

	tmp = _mm_unpackhi_epi64 (tmp, tmp);
	store_epi64_partial (msb_ptr, tmp, len);
}



void	ToolsSse2::store_8_16m_partial (void *msb_ptr, __m128i val, __m128i mask_lsb, int len)
{
	assert (msb_ptr != nullptr);

	__m128i        msb = _mm_andnot_si128 (mask_lsb, val);
	msb = _mm_srli_si128 (msb, 1);
	msb = _mm_packus_epi16 (msb, msb);
	store_epi64_partial (msb_ptr, msb, len);
}



void	ToolsSse2::store_8_16l_partial (void *lsb_ptr, __m128i val, __m128i mask_lsb, int len)
{
	assert (lsb_ptr != nullptr);

	__m128i        lsb = _mm_and_si128 (mask_lsb, val);
	lsb = _mm_packus_epi16 (lsb, lsb);
	store_epi64_partial (lsb_ptr, lsb, len);
}



__m128	ToolsSse2::load_ps_partial (const void *ptr, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 4);

	VectF32        tmp_arr = { 0 };
	while (len > 0)
	{
		-- len;
		tmp_arr [len] = static_cast <const float *> (ptr) [len];
	}
	const __m128   val = _mm_load_ps (tmp_arr);

	return val;
}



__m128i	ToolsSse2::load_si128_partial (const void *ptr, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	int            tmp = 0;
	if ((len & 1) != 0)
	{
		-- len;
		tmp = *(static_cast <const uint8_t *> (ptr) + len);
	}
	if ((len & 2) != 0)
	{
		len -= 2;
		tmp <<= 16;
		const int      ofs = len >> 1;
		tmp += *(static_cast <const uint16_t *> (ptr) + ofs);
	}
	__m128i        val;
	if (len >= 8)
	{
		const int      tmp0 = *(static_cast <const int32_t *> (ptr)    );
		const int      tmp1 = *(static_cast <const int32_t *> (ptr) + 1);
		if (len == 8)
		{
			val = _mm_set_epi32 (0, tmp, tmp1, tmp0);
		}
		else
		{
			const int      tmp2 = *(static_cast <const int32_t *> (ptr) + 2);
			val = _mm_set_epi32 (tmp, tmp2, tmp1, tmp0);
		}
	}
	else
	{
		if (len == 0)
		{
			val = _mm_set_epi32 (0, 0, 0, tmp);
		}
		else
		{
			const int      tmp0 = *static_cast <const int32_t *> (ptr);
			val = _mm_set_epi32 (0, 0, tmp, tmp0);
		}
	}

	return val;
}



__m128i	ToolsSse2::load_epi64_partial (const void *ptr, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	int            tmp = 0;
	if ((len & 1) != 0)
	{
		-- len;
		tmp = *(static_cast <const uint8_t *> (ptr) + len);
	}
	if ((len & 2) != 0)
	{
		len -= 2;
		tmp <<= 16;
		const int      ofs = len >> 1;
		tmp += *(static_cast <const uint16_t *> (ptr) + ofs);
	}
	__m128i        val;
	if ((len & 4) != 0)
	{
		const int      tmp2 = *static_cast <const int32_t *> (ptr);
		val = _mm_set_epi32 (0, 0, tmp, tmp2);
	}
	else
	{
		val = _mm_set_epi32 (0, 0, 0, tmp);
	}

	return val;
}



void	ToolsSse2::store_ps_partial (void *ptr, __m128 val, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 4);

	VectF32        tmp_arr;
	_mm_store_ps (tmp_arr, val);
	while (len > 0)
	{
		-- len;
		static_cast <float *> (ptr) [len] = tmp_arr [len];
	}
}



void	ToolsSse2::store_si128_partial (void *ptr, __m128i val, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	union
	{
		VectI08        v08;
		VectI16        v16;
		VectI32        v32;
		VectI64        v64;
		__m128i        m;
	}              tmp;
	_mm_store_si128 (&tmp.m, val);

	if ((len & 1) != 0)
	{
		*(static_cast <uint8_t  *> (ptr) + len - 1) = tmp.v08 [len - 1];
	}
	len >>= 1;
	if ((len & 1) != 0)
	{
		*(static_cast <uint16_t *> (ptr) + len - 1) = tmp.v16 [len - 1];
	}
	len >>= 1;
	if ((len & 1) != 0)
	{
		*(static_cast <uint32_t *> (ptr) + len - 1) = tmp.v32 [len - 1];
	}
	len >>= 1;
	if (len != 0)
	{
		* static_cast <uint64_t *> (ptr)            = tmp.v64 [0      ];
	}
}



void	ToolsSse2::store_epi64_partial (void *ptr, __m128i val, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

#if (fstb_WORD_SIZE == 64)

	uint64_t       tmp = _mm_cvtsi128_si64 (val);
	if ((len & 4) != 0)
	{
		*static_cast <uint32_t *> (ptr) = uint32_t (tmp);
		ptr = static_cast <uint32_t *> (ptr) + 1;
		tmp >>= 32;
	}
	if ((len & 2) != 0)
	{
		*static_cast <uint16_t *> (ptr) = uint16_t (tmp);
		ptr = static_cast <uint16_t *> (ptr) + 1;
		tmp >>= 16;
	}
	if ((len & 1) != 0)
	{
		*static_cast <uint8_t *> (ptr) = uint8_t (tmp);
	}

#else

	union
	{
		VectI08        v08;
		VectI16        v16;
		VectI32        v32;
		__m128i        m;
	}              tmp;
	_mm_store_si128 (&tmp.m, val);

	if ((len & 1) != 0)
	{
		*(static_cast <uint8_t  *> (ptr) + len - 1) = tmp.v08 [len - 1];
	}
	len >>= 1;
	if ((len & 1) != 0)
	{
		*(static_cast <uint16_t *> (ptr) + len - 1) = tmp.v16 [len - 1];
	}
	len >>= 1;
	if (len != 0)
	{
		*(static_cast <uint32_t *> (ptr) + len - 1) = tmp.v32 [len - 1];
	}

#endif
}



// Set msw to 0 for unsigned input, or to 0/FFFF for signed input (depending on the src sign)
void	ToolsSse2::add_x16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i msw)
{
	const __m128i  res0 = _mm_unpacklo_epi16 (src, msw);
	const __m128i  res1 = _mm_unpackhi_epi16 (src, msw);

	dst0 = _mm_add_epi32 (dst0, res0);
	dst1 = _mm_add_epi32 (dst1, res1);
}



// Multiplies 8 16-bit signed words and puts the result into 8 32-bit signed words.
void	ToolsSse2::mul_s16_s16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i coef)
{
	const __m128i  hi = _mm_mulhi_epi16 (src, coef);
	const __m128i  lo = _mm_mullo_epi16 (src, coef);

	dst0 = _mm_unpacklo_epi16 (lo, hi);
	dst1 = _mm_unpackhi_epi16 (lo, hi);
}



// Multiplies 32-bit signed words with 15-bits signed words.
// Result is expected to fit into 32-bit signed words.
// Coef content is 4 pairs of integers.
// Both members of each pair must be equal
// Only 4 different coeffients are possible
// They must be in range -2^15 to 2^15-1 (where 1.0 = 2^16)
__m128i	ToolsSse2::mul_s32_s15_s16 (__m128i src0, __m128i src1, __m128i coef)
{
	const __m128i  hi0 = _mm_mulhi_epu16 (src0, coef);
	const __m128i  lo0 = _mm_mullo_epi16 (src0, coef);
	const __m128i  hi1 = _mm_mulhi_epu16 (src1, coef);
	const __m128i  lo1 = _mm_mullo_epi16 (src1, coef);

	const __m128i	hi0s = _mm_slli_epi32 (hi0, 16);
	const __m128i	hi1s = _mm_slli_epi32 (hi1, 16);

	__m128i	      sum0 = _mm_add_epi32 (hi0s, lo0);
	__m128i	      sum1 = _mm_add_epi32 (hi1s, lo1);
	sum0 = _mm_srai_epi32 (sum0, 16);
	sum1 = _mm_srai_epi32 (sum1, 16);

	const __m128i	res  = _mm_packs_epi32 (sum0, sum1);

	return res;
}



// Multiplies 8 16-bit signed words and accumulate to 8 32-bit signed words.
void	ToolsSse2::mac_s16_s16_s32 (__m128i &dst0, __m128i &dst1, __m128i src, __m128i coef)
{
	const __m128i  hi = _mm_mulhi_epi16 (src, coef);
	const __m128i  lo = _mm_mullo_epi16 (src, coef);

	const __m128i  res0 = _mm_unpacklo_epi16 (lo, hi);
	const __m128i  res1 = _mm_unpackhi_epi16 (lo, hi);

	dst0 = _mm_add_epi32 (dst0, res0);
	dst1 = _mm_add_epi32 (dst1, res1);
}



// Code of this function shamelessly borrowed from tp7
// https://github.com/tp7/masktools/blob/16bit/masktools/common/simd.h
__m128i	ToolsSse2::mullo_epi32 (const __m128i &a, const __m128i &b)
{
	const __m128i  a13    = _mm_shuffle_epi32 (a, 0xF5);         // (-,a3,-,a1)
	const __m128i  b13    = _mm_shuffle_epi32 (b, 0xF5);         // (-,b3,-,b1)
	const __m128i  prod02 = _mm_mul_epu32 (a, b);                // (-,a2*b2,-,a0*b0)
	const __m128i  prod13 = _mm_mul_epu32 (a13, b13);            // (-,a3*b3,-,a1*b1)
	const __m128i  prod01 = _mm_unpacklo_epi32 (prod02, prod13); // (-,-,a1*b1,a0*b0)
	const __m128i  prod23 = _mm_unpackhi_epi32 (prod02, prod13); // (-,-,a3*b3,a2*b2)
	const __m128i  res    = _mm_unpacklo_epi64 (prod01 ,prod23); // (ab3,ab2,ab1,ab0)

	return res;
}



// Same here
__m128i	ToolsSse2::pack_epi16 (__m128i a, __m128i b)
{
#if 0	// To do: test which one is faster

	a = _mm_shufflelo_epi16 (a, _MM_SHUFFLE (3, 3, 2, 0));
	a = _mm_shufflehi_epi16 (a, _MM_SHUFFLE (3, 3, 2, 0));
	a = _mm_shuffle_epi32 (a, _MM_SHUFFLE (3, 3, 2, 0));

	b = _mm_shufflelo_epi16 (b, _MM_SHUFFLE (3, 3, 2, 0));
	b = _mm_shufflehi_epi16 (b, _MM_SHUFFLE (3, 3, 2, 0));
	b = _mm_shuffle_epi32 (b, _MM_SHUFFLE (2, 0, 3, 3));

	const __m128i  ma = _mm_set_epi32 (-1, -1, 0, 0);
	const __m128i  pa = _mm_and_si128 (ma, a);
	const __m128i  pb = _mm_andnot_si128 (ma, b);
	const __m128i  p  = _mm_or_si128 (pa, pb);

#else

	const __m128i  mask_lo  = _mm_load_si128 (
		reinterpret_cast <const __m128i *> (ToolsSse2::_c32_0000ffff)
	);
	const __m128i  offset_s = _mm_load_si128 (
		reinterpret_cast <const __m128i *> (ToolsSse2::_c32_00008000)
	);
	const __m128i  mask_s   = _mm_load_si128 (
		reinterpret_cast <const __m128i *> (ToolsSse2::_c16_8000)
	);

	a = _mm_and_si128 (mask_lo, a);
	b = _mm_and_si128 (mask_lo, b);
	a = _mm_sub_epi32 (a, offset_s);
	b = _mm_sub_epi32 (b, offset_s);
	__m128i        p = _mm_packs_epi32 (a, b);
	p = _mm_xor_si128 (p, mask_s);

#endif

	return p;
}



__m128i	ToolsSse2::select (const __m128i &cond, const __m128i &v_t, const __m128i &v_f)
{
	const __m128i  cond_1   = _mm_and_si128 (cond, v_t);
	const __m128i  cond_0   = _mm_andnot_si128 (cond, v_f);
	const __m128i  res      = _mm_or_si128 (cond_0, cond_1);

	return res;
}



__m128	ToolsSse2::select (const __m128 &cond, const __m128 &v_t, const __m128 &v_f)
{
	const __m128   cond_1   = _mm_and_ps (cond, v_t);
	const __m128   cond_0   = _mm_andnot_ps (cond, v_f);
	const __m128   res      = _mm_or_ps (cond_0, cond_1);

	return res;
}



__m128i	ToolsSse2::select_16_equ (const __m128i &lhs, const __m128i &rhs, const __m128i &v_t, const __m128i &v_f)
{
	const __m128i  cond = _mm_cmpeq_epi16 (lhs, rhs);

	return ToolsSse2::select (cond, v_t, v_f);
}



__m128i	ToolsSse2::limit_epi16 (const __m128i &x, const __m128i &mi, const __m128i &ma)
{
	return _mm_max_epi16 (_mm_min_epi16 (x, ma), mi);
}



__m128i	ToolsSse2::abs_dif_epu16 (const __m128i &a, const __m128i &b)
{
	const __m128i  p  = _mm_subs_epu16 (a, b);
	const __m128i  n  = _mm_subs_epu16 (b, a);

	return _mm_or_si128 (p, n);
}



// Returns an unsigned value.
__m128i	ToolsSse2::abs_dif_epi16 (const __m128i &a, const __m128i &b)
{
	const __m128i  mask_s  = _mm_load_si128 (
		reinterpret_cast <const __m128i *> (ToolsSse2::_c16_8000)
	);
	const __m128i  au = _mm_xor_si128 (a, mask_s);
	const __m128i  bu = _mm_xor_si128 (b, mask_s);

	return abs_dif_epu16 (au, bu);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



#endif	// fstb_ToolsSse2_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
