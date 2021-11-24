/*****************************************************************************

        Vf32.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_VectF32_CODEHEADER_INCLUDED)
#define fstb_VectF32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>
#include <cmath>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vf32::Vf32 (Scalar a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { a, a, a, a }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (a) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (a) }
#endif // fstb_ARCHI
{
	// Nothing
}



Vf32::Vf32 (double a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { Scalar (a), Scalar (a), Scalar (a), Scalar (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (Scalar (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (Scalar (a)) }
#endif // fstb_ARCHI
{
	// Nothing
}



Vf32::Vf32 (int a) noexcept
#if ! defined (fstb_HAS_SIMD)
:	_x { Scalar (a), Scalar (a), Scalar (a), Scalar (a) }
#elif fstb_ARCHI == fstb_ARCHI_X86
:	_x { _mm_set1_ps (Scalar (a)) }
#elif fstb_ARCHI == fstb_ARCHI_ARM
:	_x { vdupq_n_f32 (Scalar (a)) }
#endif // fstb_ARCHI
{
	// Nothing
}



// Works only with well-formed condition results (tested bits depend on the
// implementation).
// For each scalar, true = all bits set, false = all bits cleared
Vf32::operator bool () const noexcept
{
	return and_h ();
}



Vf32 &	Vf32::operator += (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] += other [0];
	_x [1] += other [1];
	_x [2] += other [2];
	_x [3] += other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_add_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vaddq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator -= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] -= other [0];
	_x [1] -= other [1];
	_x [2] -= other [2];
	_x [3] -= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_sub_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vsubq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator *= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] *= other [0];
	_x [1] *= other [1];
	_x [2] *= other [2];
	_x [3] *= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_mul_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vmulq_f32 (_x, other);
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator /= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	_x [0] /= other [0];
	_x [1] /= other [1];
	_x [2] /= other [2];
	_x [3] /= other [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_div_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = _x * (Vf32 { other }.rcp_approx2 ())._x;
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator &= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = _x;
	ar._vf32   = other;
	al._s32 [0] &= ar._s32 [0];
	al._s32 [1] &= ar._s32 [1];
	al._s32 [2] &= ar._s32 [2];
	al._s32 [3] &= ar._s32 [3];
	_x = al._s32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_and_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator |= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = _x;
	ar._vf32   = other;
	al._s32 [0] |= ar._s32 [0];
	al._s32 [1] |= ar._s32 [1];
	al._s32 [2] |= ar._s32 [2];
	al._s32 [3] |= ar._s32 [3];
	_x = al._s32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_or_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (vorrq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



Vf32 &	Vf32::operator ^= (const Vf32Native &other) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          al;
	Combo          ar;
	Combo          r;
	al._vf32   = _x;
	ar._vf32   = other;
	al._s32 [0] ^= ar._s32 [0];
	al._s32 [1] ^= ar._s32 [1];
	al._s32 [2] ^= ar._s32 [2];
	al._s32 [3] ^= ar._s32 [3];
	_x = al._s32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	_x = _mm_xor_ps (_x, other);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	_x = vreinterpretq_f32_u32 (veorq_u32 (
		vreinterpretq_u32_f32 (_x),
		vreinterpretq_u32_f32 (other)
	));
#endif // fstb_ARCHI
	return *this;
}



// Assumes "to nearest" rounding mode on x86
Vf32	Vf32::round () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		roundf (_x [0]),
		roundf (_x [1]),
		roundf (_x [2]),
		roundf (_x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cvtepi32_ps (_mm_cvtps_epi32 (_x));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const auto     zero = vdupq_n_f32 ( 0.0f);
	const auto     m    = vdupq_n_f32 (-0.5f);
	const auto     p    = vdupq_n_f32 (+0.5f);
	const auto     gt0  = vcgtq_f32 (_x, zero);
	const auto     u    = vbslq_f32 (gt0, p, m);
	return vaddq_f32 (_x, u);
#endif // fstb_ARCHI
}



Vf32	Vf32::rcp_approx () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return { {
		1.f / v._x [0],
		1.f / v._x [1],
		1.f / v._x [2],
		1.f / v._x [3]
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_rcp_ps (_x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           r = vrecpeq_f32 (_x);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	return r;
#endif // fstb_ARCHI
}



// With more accuracy
Vf32	Vf32::rcp_approx2 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return rcp_approx ();
#elif fstb_ARCHI == fstb_ARCHI_X86
	auto           r = _mm_rcp_ps (_x);
	r = r * (_mm_set1_ps (2.f) - r * _x);
	return r;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	auto           r = vrecpeq_f32 (_x);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	r = vmulq_f32 (vrecpsq_f32 (_x, r), r);
	return r;
#endif // fstb_ARCHI
}



Vf32	Vf32::signbit () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return { {
		copysignf (0.f, _x [0]),
		copysignf (0.f, _x [1]),
		copysignf (0.f, _x [2]),
		copysignf (0.f, ._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_and_ps (signbit_mask (), _x);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vandq_u32 (
		vreinterpretq_u32_f32 (_x),
		vdupq_n_u32 (0x80000000U)
	));
#endif // fstb_ARCHI
}



Vf32   Vf32::is_lt_0 () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (_x [0] < 0) ? -1 : 0;
	r._s32 [1] = (_x [1] < 0) ? -1 : 0;
	r._s32 [2] = (_x [2] < 0) ? -1 : 0;
	r._s32 [3] = (_x [3] < 0) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_castsi128_ps (_mm_srai_epi32 (_mm_castps_si128 (_x), 31));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_s32 (vshrq_n_s32 (vreinterpretq_s32_f32 (_x), 31));
#endif // fstb_ARCHI
}



void	Vf32::explode (float &a0, float &a1, float &a2, float &a3) const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	a0 = _x [0];
	a1 = _x [1];
	a2 = _x [2];
	a3 = _x [3];
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     tmp = _mm_movehl_ps (_x, _x);
	a0 = _mm_cvtss_f32 (_x);
	a2 = _mm_cvtss_f32 (tmp);
	a1 = _mm_cvtss_f32 (_mm_shuffle_ps (_x, _x, (1<<0)));
	a3 = _mm_cvtss_f32 (_mm_shuffle_ps (tmp, tmp, (1<<0)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	a0 = vgetq_lane_f32 (_x, 0);
	a1 = vgetq_lane_f32 (_x, 1);
	a2 = vgetq_lane_f32 (_x, 2);
	a3 = vgetq_lane_f32 (_x, 3);
#endif // fstb_ARCHI
}



float	Vf32::sum_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return (_x [0] + _x [2]) + (_x [1] + _x [3]);
#elif fstb_ARCHI == fstb_ARCHI_X86
	// s = v3,v2,v1,v0
	const auto s = _mm_shuffle_ps (_x, _x, (3 << 0) | (2 << 2) | (1 << 4) | (0 << 6));
	const auto v = _mm_add_ps (_x, s); // v0+v3,v1+v2,v2+v1,v3+v0
	return _mm_cvtss_f32 (_mm_add_ss (v, _mm_movehl_ps (s, v)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	#if fstb_WORD_SIZE == 64
		return vaddvq_f32 (_x);
	#else
		float32x2_t    v2 = vadd_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
		return vget_lane_f32 (vpadd_f32 (v2, v2), 0);
	#endif
#endif // fstb_ARCHI
}



float	Vf32::min_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::min (std::min (_x [0], _x [2]), std::min (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v = _mm_min_ps (_x, _mm_shuffle_ps (_x, _x, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_min_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmin_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
	return vget_lane_f32 (vpmin_f32 (v2, v2), 0);
#endif // fstb_ARCHI
}



float	Vf32::max_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return std::max (std::max (_x [0], _x [2]), std::max (_x [1], _x [3]));
#elif fstb_ARCHI == fstb_ARCHI_X86
	const auto     v = _mm_max_ps (_x, _mm_shuffle_ps (_x, _x, (3 << 2) | 2));
	return _mm_cvtss_f32 (_mm_max_ss (v, _mm_shuffle_ps (v, v, 1)));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	float32x2_t    v2 = vmax_f32 (vget_high_f32 (_x), vget_low_f32 (_x));
	return vget_lane_f32 (vpmax_f32 (v2, v2), 0);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool   Vf32::and_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	const Combo    c { _x };
	const int32_t  t = (c._s32 [0] & c._s32 [1]) & (c._s32 [2] & c._s32 [3]);
	return (t == -1);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (_x) == 15);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) == 0xFFFFFFFFU
	        && vget_lane_u32 (tmp, 1) == 0xFFFFFFFFU);
#endif // fstb_ARCHI
}



// Works only with well-formed condition results (tested bits depends on the implementation).
// For each scalar, true = all bits set, false = all bits cleared
bool   Vf32::or_h () const noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._vf32 = _x;
	const int32_t  t = (c._s32 [0] | c._s32 [1]) | (c._s32 [2] | c._s32 [3]);
	return (t != 0);
#elif fstb_ARCHI == fstb_ARCHI_X86
	return (_mm_movemask_ps (_x) != 0);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	const uint32x2_t  tmp = vreinterpret_u32_u16 (
		vqmovn_u32 (vreinterpretq_u32_f32 (_x))
	);
	return (   vget_lane_u32 (tmp, 0) != 0
	        || vget_lane_u32 (tmp, 1) != 0);
#endif // fstb_ARCHI
}



Vf32Native   Vf32::signbit_mask () noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          c;
	c._u32 [0] = 0x80000000U;
	c._u32 [1] = 0x80000000U;
	c._u32 [2] = 0x80000000U;
	c._u32 [3] = 0x80000000U;
	return c._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
//	return _mm_set1_ps (-0.f);
	return _mm_castsi128_ps (_mm_set1_epi32 (0x80000000));
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vdupq_n_u32 (0x80000000U)); 
#endif // fstb_ARCHI
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Vf32 operator + (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs += rhs;
	return lhs;
}

Vf32 operator - (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs -= rhs;
	return lhs;
}

Vf32 operator * (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs *= rhs;
	return lhs;
}

Vf32 operator / (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs /= rhs;
	return lhs;
}

Vf32 operator & (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs &= rhs;
	return lhs;
}

Vf32 operator | (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs |= rhs;
	return lhs;
}

Vf32 operator ^ (Vf32 lhs, const Vf32 &rhs) noexcept
{
	lhs ^= rhs;
	return lhs;
}



Vf32	operator == (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] == rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] == rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] == rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] == rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpeq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vceqq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator != (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] != rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] != rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] != rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] != rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpneq_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vmvnq_u32 (vceqq_f32 (lhs, rhs)));
#endif // fstb_ARCHI
}



Vf32	operator <  (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] < rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] < rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] < rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] < rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmplt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcltq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator <= (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] <= rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] <= rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] <= rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] <= rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmple_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcleq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator >  (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] > rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] > rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] > rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] > rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpgt_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcgtq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32	operator >= (const Vf32 &lhs, const Vf32 &rhs) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	Combo          r;
	r._s32 [0] = (lhs._x [0] >= rhs._x [0]) ? -1 : 0;
	r._s32 [1] = (lhs._x [1] >= rhs._x [1]) ? -1 : 0;
	r._s32 [2] = (lhs._x [2] >= rhs._x [2]) ? -1 : 0;
	r._s32 [3] = (lhs._x [3] >= rhs._x [3]) ? -1 : 0;
	return r._vf32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_cmpge_ps (lhs, rhs);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vreinterpretq_f32_u32 (vcgeq_f32 (lhs, rhs));
#endif // fstb_ARCHI
}



Vf32 abs (const Vf32 &v) noexcept
{
#if ! defined (fstb_HAS_SIMD)
	return Vf32 { {
		fabsf (v._x [0]),
		fabsf (v._x [1]),
		fabsf (v._x [2]),
		fabsf (v._x [3])
	} };
#elif fstb_ARCHI == fstb_ARCHI_X86
	return _mm_andnot_ps (Vf32::signbit_mask (), v);
#elif fstb_ARCHI == fstb_ARCHI_ARM
	return vabsq_f32 (v);
#endif // fstb_ARCHI
}



Vf32 round (const Vf32 &v) noexcept
{
	return v.round ();
}



}  // namespace fstb



#endif   // fstb_VectF32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
