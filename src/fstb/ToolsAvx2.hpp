/*****************************************************************************

        ToolsAvx2.hpp
        Author: Laurent de Soras, 2015

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_ToolsAvx2_CODEHEADER_INCLUDED)
#define	fstb_ToolsAvx2_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSse2.h"

#include <cassert>
#include <cstddef>
#include <cstdint>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
inline bool	ToolsAvx2::check_ptr_align (T *ptr)
{
	return (ptr != nullptr && (reinterpret_cast <intptr_t> (ptr) & 31) == 0);
}



__m256i	ToolsAvx2::load_16_16ml (const void *msb_ptr, const void *lsb_ptr)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);

	const __m256i  val_msb = _mm256_cvtepu8_epi16 (_mm_loadu_si128 (
		static_cast <const __m128i *> (msb_ptr)
	));
	const __m256i  val_lsb = _mm256_cvtepu8_epi16 (_mm_loadu_si128 (
		static_cast <const __m128i *> (lsb_ptr)
	));
	const __m256i  val = _mm256_or_si256 (
		_mm256_slli_epi16 (val_msb, 8),
		val_lsb
	);

	return val;
}



__m256i	ToolsAvx2::load_16_16m (const void *msb_ptr)
{
	assert (msb_ptr != nullptr);

	const __m256i  val_msb = _mm256_cvtepu8_epi16 (_mm_loadu_si128 (
		static_cast <const __m128i *> (msb_ptr)
	));
	const __m256i  val = _mm256_slli_epi16 (val_msb, 8);

	return val;
}



__m256i	ToolsAvx2::load_16_16l (const void *lsb_ptr)
{
	assert (lsb_ptr != nullptr);

	const __m256i  val_lsb = _mm256_cvtepu8_epi16 (_mm_loadu_si128 (
		static_cast <const __m128i *> (lsb_ptr)
	));

	return val_lsb;
}



__m256i	ToolsAvx2::load_16_16ml_partial (const void *msb_ptr, const void *lsb_ptr, int len)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	const __m256i  val_msb =
		_mm256_cvtepu8_epi16 (ToolsSse2::load_si128_partial (msb_ptr, len));
	const __m256i  val_lsb =
		_mm256_cvtepu8_epi16 (ToolsSse2::load_si128_partial (lsb_ptr, len));
	const __m256i  val = _mm256_or_si256 (
		_mm256_slli_epi16 (val_msb, 8),
		val_lsb
	);

	return val;
}



__m256i	ToolsAvx2::load_16_16m_partial (const void *msb_ptr, int len)
{
	assert (msb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	const __m256i  val_msb =
		_mm256_cvtepu8_epi16 (ToolsSse2::load_si128_partial (msb_ptr, len));
	const __m256i  val = _mm256_slli_epi16 (val_msb, 8);

	return val;
}



__m256i	ToolsAvx2::load_16_16l_partial (const void *lsb_ptr, int len)
{
	assert (lsb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	const __m256i  val =
		_mm256_cvtepu8_epi16 (ToolsSse2::load_si128_partial (lsb_ptr, len));

	return val;
}



// mask_lsb = 0x00FF00FF00FF00...
void	ToolsAvx2::store_16_16ml (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);
	
	const __m256i	lsb = _mm256_and_si256 (mask_lsb, val);
	__m256i			msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);

	__m256i			lsbmsb = _mm256_packus_epi16 (lsb, msb);
	lsbmsb = _mm256_permute4x64_epi64 (lsbmsb, (0<<0) + (2<<2) + (1<<4) + (3<<6));

	_mm_storeu_si128 (
		static_cast <__m128i *> (lsb_ptr), _mm256_castsi256_si128 (lsbmsb)
	);
	_mm_storeu_si128 (
		static_cast <__m128i *> (msb_ptr), _mm256_extractf128_si256 (lsbmsb, 0x1)
	);
}



// mask_lsb = 0x00FF00FF00FF00...
void	ToolsAvx2::store_16_16ml_perm (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, __m256i permute)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);
	
	const __m256i	lsb = _mm256_and_si256 (mask_lsb, val);
	__m256i			msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);

	__m256i			lsbmsb = _mm256_packus_epi16 (lsb, msb);
	lsbmsb = _mm256_permutevar8x32_epi32 (lsbmsb, permute);

	_mm_storeu_si128 (
		static_cast <__m128i *> (lsb_ptr), _mm256_castsi256_si128 (lsbmsb)
	);
	_mm_storeu_si128 (
		static_cast <__m128i *> (msb_ptr), _mm256_extractf128_si256 (lsbmsb, 0x1)
	);
}



// mask_lsb = 0x00FF00FF00FF00FF00FF00FF00FF00FF
void	ToolsAvx2::store_16_16m (void *msb_ptr, __m256i val, __m256i mask_lsb)
{
	assert (msb_ptr != nullptr);

	__m256i        msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);
	msb = _mm256_packus_epi16 (msb, msb);
	msb = _mm256_permute4x64_epi64 (msb, (0<<0) + (2<<2));
	_mm_storeu_si128 (
		static_cast <__m128i *> (msb_ptr), _mm256_castsi256_si128 (msb)
	);
}



// mask_lsb = 0x00FF00FF00FF00FF00FF00FF00FF00FF
void	ToolsAvx2::store_16_16l (void *lsb_ptr, __m256i val, __m256i mask_lsb)
{
	assert (lsb_ptr != nullptr);

	__m256i        lsb = _mm256_and_si256 (mask_lsb, val);
	lsb = _mm256_packus_epi16 (lsb, lsb);
	lsb = _mm256_permute4x64_epi64 (lsb, (0<<0) + (2<<2));
	_mm_storeu_si128 (
		static_cast <__m128i *> (lsb_ptr), _mm256_castsi256_si128 (lsb)
	);
}



void	ToolsAvx2::store_16_16ml_partial (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, int len)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);
	assert (len >= 0);
	assert (len < 16);
	
	const __m256i	lsb = _mm256_and_si256 (mask_lsb, val);
	__m256i			msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);

	__m256i			lsbmsb = _mm256_packus_epi16 (lsb, msb);  // lsblo msblo lsbhi msbhi
	lsbmsb = _mm256_permute4x64_epi64 (lsbmsb, (0<<0) + (2<<2) + (1<<4) + (3<<6));

	const __m128i  lsb128 = _mm256_castsi256_si128 (lsbmsb);
	const __m128i  msb128 = _mm256_extractf128_si256 (lsbmsb, 1);

	ToolsSse2::store_si128_partial (msb_ptr, msb128, len);
	ToolsSse2::store_si128_partial (lsb_ptr, lsb128, len);
}



void	ToolsAvx2::store_16_16ml_perm_partial (void *msb_ptr, void *lsb_ptr, __m256i val, __m256i mask_lsb, __m256i permute, int len)
{
	assert (msb_ptr != nullptr);
	assert (lsb_ptr != nullptr);
	assert (lsb_ptr != msb_ptr);
	assert (len >= 0);
	assert (len < 16);

	const __m256i	lsb = _mm256_and_si256 (mask_lsb, val);
	__m256i			msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);

	__m256i			lsbmsb = _mm256_packus_epi16 (lsb, msb);  // lsblo msblo lsbhi msbhi
	lsbmsb = _mm256_permutevar8x32_epi32 (lsbmsb, permute);

	const __m128i  lsb128 = _mm256_castsi256_si128 (lsbmsb);
	const __m128i  msb128 = _mm256_extractf128_si256 (lsbmsb, 1);

	ToolsSse2::store_si128_partial (msb_ptr, msb128, len);
	ToolsSse2::store_si128_partial (lsb_ptr, lsb128, len);
}



void	ToolsAvx2::store_16_16m_partial (void *msb_ptr, __m256i val, __m256i mask_lsb, int len)
{
	assert (msb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	__m256i        msb = _mm256_andnot_si256 (mask_lsb, val);
	msb = _mm256_srli_si256 (msb, 1);
	msb = _mm256_packus_epi16 (msb, msb);
	msb = _mm256_permute4x64_epi64 (msb, (0<<0) + (2<<2));
	const __m128i  msb128 = _mm256_castsi256_si128 (msb);
	ToolsSse2::store_si128_partial (msb_ptr, msb128, len);
}



void	ToolsAvx2::store_16_16l_partial (void *lsb_ptr, __m256i val, __m256i mask_lsb, int len)
{
	assert (lsb_ptr != nullptr);
	assert (len >= 0);
	assert (len < 16);

	__m256i        lsb = _mm256_and_si256 (mask_lsb, val);
	lsb = _mm256_packus_epi16 (lsb, lsb);
	lsb = _mm256_permute4x64_epi64 (lsb, (0<<0) + (2<<2));
	const __m128i  lsb128 = _mm256_castsi256_si128 (lsb);
	ToolsSse2::store_si128_partial (lsb_ptr, lsb128, len);
}



__m256	ToolsAvx2::load_ps_partial (const void *ptr, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	__m256         val;
	if (len >= 4)
	{
		const __m128   src_0 =
			_mm_loadu_ps (static_cast <const float *> (ptr));
		const __m128   src_1 = ToolsSse2::load_ps_partial (
			static_cast <const char *> (ptr) + sizeof (src_0),
			len - 4
		);
		val = _mm256_insertf128_ps (
			_mm256_castps128_ps256 (src_0), src_1, 1
		);
	}
	else
	{
		const __m128   src_0 = ToolsSse2::load_ps_partial (ptr, len);
		val = _mm256_insertf128_ps (
			_mm256_castps128_ps256 (src_0), _mm_setzero_ps (), 1
		);
	}

	return (val);
}



__m256i	ToolsAvx2::load_si256_partial (const void *ptr, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 32);

	__m256i        val;
	if (len >= 16)
	{
		const __m128i  src_0 =
			_mm_loadu_si128 (static_cast <const __m128i *> (ptr));
		const __m128i  src_1 = ToolsSse2::load_si128_partial (
			static_cast <const char *> (ptr) + sizeof (src_0),
			len - 16
		);
		val = _mm256_insertf128_si256 (
			_mm256_castsi128_si256 (src_0), src_1, 1
		);
	}
	else
	{
		const __m128i  src_0 = ToolsSse2::load_si128_partial (ptr, len);
		val = _mm256_insertf128_si256 (
			_mm256_castsi128_si256 (src_0), _mm_setzero_si128 (), 1
		);
	}

	return (val);
}



void	ToolsAvx2::store_ps_partial (void *ptr, __m256 val, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 8);

	const __m128   val_0 = _mm256_castps256_ps128 (val);
	if (len >= 4)
	{
		_mm_storeu_ps (static_cast <float *> (ptr), val_0);
		const __m128   val_1 = _mm256_extractf128_ps (val, 1);
		ToolsSse2::store_ps_partial (
			static_cast <char *> (ptr) + sizeof (val_0), val_1, len - 4
		);
	}
	else
	{
		ToolsSse2::store_ps_partial (static_cast <char *> (ptr), val_0, len);
	}
}



void	ToolsAvx2::store_si256_partial (void *ptr, __m256i val, int len)
{
	assert (ptr != nullptr);
	assert (len >= 0);
	assert (len < 32);

	const __m128i  val_0 = _mm256_castsi256_si128 (val);
	if (len >= 16)
	{
		_mm_storeu_si128 (static_cast <__m128i *> (ptr), val_0);
		const __m128i  val_1 = _mm256_extractf128_si256 (val, 1);
		ToolsSse2::store_si128_partial (
			static_cast <char *> (ptr) + sizeof (val_0), val_1, len - 16
		);
	}
	else
	{
		ToolsSse2::store_si128_partial (static_cast <char *> (ptr), val_0, len);
	}
}



// Multiplies 8 16-bit signed words and accumulate to 8 32-bit signed words.
// Important: the 128-bit lane order from the unpack functions is kept.
void	ToolsAvx2::mac_s16_s16_s32 (__m256i &dst0, __m256i &dst1, __m256i src, __m256i coef)
{
	const __m256i  hi = _mm256_mulhi_epi16 (src, coef);
	const __m256i  lo = _mm256_mullo_epi16 (src, coef);

	const __m256i  res0 = _mm256_unpacklo_epi16 (lo, hi);
	const __m256i  res1 = _mm256_unpackhi_epi16 (lo, hi);

	dst0 = _mm256_add_epi32 (dst0, res0);
	dst1 = _mm256_add_epi32 (dst1, res1);
}



__m256i	ToolsAvx2::mullo_epi32 (const __m256i &a, const __m256i &b)
{	                                                                // For each 128-bit lane:
	const __m256i  a13    = _mm256_shuffle_epi32 (a, 0xF5);         // (-,a3,-,a1)
	const __m256i  b13    = _mm256_shuffle_epi32 (b, 0xF5);         // (-,b3,-,b1)
	const __m256i  prod02 = _mm256_mul_epu32 (a, b);                // (-,a2*b2,-,a0*b0)
	const __m256i  prod13 = _mm256_mul_epu32 (a13, b13);            // (-,a3*b3,-,a1*b1)
	const __m256i  prod01 = _mm256_unpacklo_epi32 (prod02, prod13); // (-,-,a1*b1,a0*b0)
	const __m256i  prod23 = _mm256_unpackhi_epi32 (prod02, prod13); // (-,-,a3*b3,a2*b2)
	const __m256i  res    = _mm256_unpacklo_epi64 (prod01 ,prod23); // (ab3,ab2,ab1,ab0)

	return (res);
}



__m256i	ToolsAvx2::pack_epi16 (__m256i a, __m256i b)
{
	const __m256i  mask_lo  = _mm256_load_si256 (
		reinterpret_cast <const __m256i *> (ToolsAvx2::_c32_0000ffff)
	);
	const __m256i  offset_s = _mm256_load_si256 (
		reinterpret_cast <const __m256i *> (ToolsAvx2::_c32_00008000)
	);
	const __m256i  mask_s   = _mm256_load_si256 (
		reinterpret_cast <const __m256i *> (ToolsAvx2::_c16_8000)
	);

	a = _mm256_and_si256 (mask_lo, a);
	b = _mm256_and_si256 (mask_lo, b);
	a = _mm256_sub_epi32 (a, offset_s);
	b = _mm256_sub_epi32 (b, offset_s);
	__m256i        p = _mm256_packs_epi32 (a, b);
	p = _mm256_permute4x64_epi64 (p, (0<<0) + (2<<2) + (1<<4) + (3<<6));
	p = _mm256_xor_si256 (p, mask_s);

	return (p);
}



__m256i	ToolsAvx2::select (const __m256i &cond, const __m256i &v_t, const __m256i &v_f)
{
	const __m256i  cond_1   = _mm256_and_si256 (cond, v_t);
	const __m256i  cond_0   = _mm256_andnot_si256 (cond, v_f);
	const __m256i  res      = _mm256_or_si256 (cond_0, cond_1);

	return (res);
}



__m256	ToolsAvx2::select (const __m256 &cond, const __m256 &v_t, const __m256 &v_f)
{
	const __m256   cond_1   = _mm256_and_ps (cond, v_t);
	const __m256   cond_0   = _mm256_andnot_ps (cond, v_f);
	const __m256   res      = _mm256_or_ps (cond_0, cond_1);

	return (res);
}



__m256i	ToolsAvx2::select_16_equ (const __m256i &lhs, const __m256i &rhs, const __m256i &v_t, const __m256i &v_f)
{
	const __m256i  cond = _mm256_cmpeq_epi16 (lhs, rhs);

	return (ToolsAvx2::select (cond, v_t, v_f));
}



__m256i	ToolsAvx2::limit_epi16 (const __m256i &x, const __m256i &mi, const __m256i &ma)
{
	return (_mm256_max_epi16 (_mm256_min_epi16 (x, ma), mi));
}



__m256i	ToolsAvx2::abs_dif_epu16 (const __m256i &a, const __m256i &b)
{
	const __m256i  p  = _mm256_subs_epu16 (a, b);
	const __m256i  n  = _mm256_subs_epu16 (b, a);

	return (_mm256_or_si256 (p, n));
}



// Returns an unsigned value.
__m256i	ToolsAvx2::abs_dif_epi16 (const __m256i &a, const __m256i &b)
{
	const __m256i  mask_s  = _mm256_load_si256 (
		reinterpret_cast <const __m256i *> (ToolsAvx2::_c16_8000)
	);
	const __m256i  au = _mm256_xor_si256 (a, mask_s);
	const __m256i  bu = _mm256_xor_si256 (b, mask_s);

	return (abs_dif_epu16 (au, bu));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



#endif	// fstb_ToolsAvx2_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
