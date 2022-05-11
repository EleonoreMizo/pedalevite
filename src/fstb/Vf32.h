/*****************************************************************************

        Vf32.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Vf32_HEADER_INCLUDED)
#define fstb_Vf32_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if ! defined (fstb_HAS_SIMD)
	#include <array>
#elif (fstb_ARCHI == fstb_ARCHI_X86)
	#include <emmintrin.h>
#elif (fstb_ARCHI == fstb_ARCHI_ARM)
	#include <arm_neon.h>
#else
	#error
#endif

#include <tuple>

#include <cstdint>



namespace fstb
{



#if ! defined (fstb_HAS_SIMD)

typedef std::array <float, 4> Vf32Native;

#elif fstb_ARCHI == fstb_ARCHI_X86

typedef __m128      Vf32Native;

#elif fstb_ARCHI == fstb_ARCHI_ARM

typedef float32x4_t Vf32Native;

#else // fstb_ARCHI
#error
#endif // fstb_ARCHI



class Vf32
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _len_l2 = 2;
	static constexpr int _length = 1 << _len_l2;
	typedef float Scalar;

	               Vf32 ()                        = default;
	fstb_FORCEINLINE
	               Vf32 (Vf32Native a) noexcept : _x { a } {}
	explicit fstb_FORCEINLINE
	               Vf32 (Scalar a) noexcept;
	explicit fstb_FORCEINLINE
	               Vf32 (double a) noexcept;
	explicit fstb_FORCEINLINE
	               Vf32 (int a) noexcept;
	explicit fstb_FORCEINLINE
	               Vf32 (Scalar a0, Scalar a1, Scalar a2, Scalar a3) noexcept;
	explicit fstb_FORCEINLINE
	               Vf32 (const std::tuple <Scalar, Scalar, Scalar, Scalar> &a) noexcept;
	               Vf32 (const Vf32 &other)       = default;
	               Vf32 (Vf32 &&other)            = default;
	               ~Vf32 ()                       = default;
	Vf32 &         operator = (const Vf32 &other) = default;
	Vf32 &         operator = (Vf32 &&other)      = default;

	template <typename MEM>
	fstb_FORCEINLINE void
	               store (MEM *ptr) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               store_part (MEM *ptr, int n) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu (MEM *ptr) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu_part (MEM *ptr, int n) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu_pair (MEM *ptr) const noexcept;
	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu_scalar (MEM *ptr) const noexcept;

	fstb_FORCEINLINE
	               operator Vf32Native () const noexcept { return _x; }
	fstb_FORCEINLINE explicit
	               operator bool () const noexcept;

	fstb_FORCEINLINE Vf32 &
	               operator += (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator -= (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator *= (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator /= (const Vf32Native &other) noexcept;

	fstb_FORCEINLINE Vf32 &
	               operator &= (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator |= (const Vf32Native &other) noexcept;
	fstb_FORCEINLINE Vf32 &
	               operator ^= (const Vf32Native &other) noexcept;

	fstb_FORCEINLINE Vf32 &
	               mac (Vf32 a, Vf32 b) noexcept;
	fstb_FORCEINLINE Vf32 &
	               msu (Vf32 a, Vf32 b) noexcept;

	fstb_FORCEINLINE Vf32
	               operator - () const noexcept;
	fstb_FORCEINLINE Vf32
	               reverse () const noexcept;
	fstb_FORCEINLINE Vf32
	               swap_pairs () const noexcept;
	fstb_FORCEINLINE Vf32
	               monofy_pairs_lo () const noexcept;
	fstb_FORCEINLINE Vf32
	               monofy_pairs_hi () const noexcept;

	fstb_FORCEINLINE Vf32
	               butterfly_w64 () const noexcept;
	fstb_FORCEINLINE Vf32
	               butterfly_w32 () const noexcept;

	template <int SHIFT>
	fstb_FORCEINLINE Vf32
	               rotate () const noexcept;
	template <int POS>
	fstb_FORCEINLINE float
	               extract () const noexcept;
	template <int POS>
	fstb_FORCEINLINE Vf32
	               insert (float val) const noexcept;
	template <int POS>
	fstb_FORCEINLINE Vf32
	               spread () const noexcept;

	fstb_FORCEINLINE Vf32
	               round () const noexcept;
	fstb_FORCEINLINE Vf32
	               rcp_approx () const noexcept;
	fstb_FORCEINLINE Vf32
	               rcp_approx2 () const noexcept;
	fstb_FORCEINLINE Vf32
	               div_approx (const Vf32 &d) const noexcept;
	fstb_FORCEINLINE Vf32
	               sqrt_approx () const noexcept;
	fstb_FORCEINLINE Vf32
	               rsqrt () const noexcept;
	fstb_FORCEINLINE Vf32
	               rsqrt_approx () const noexcept;
	template <typename P>
	fstb_FORCEINLINE Vf32
	               log2_base (P poly) const noexcept;
	template <typename P>
	fstb_FORCEINLINE Vf32
	               exp2_base (P poly) const noexcept;
	fstb_FORCEINLINE Vf32
	               signbit () const noexcept;
	fstb_FORCEINLINE Vf32
	               is_lt_0 () const noexcept;

	fstb_FORCEINLINE std::tuple <float, float, float, float>
	               explode () const noexcept;
	fstb_FORCEINLINE std::tuple <float, float>
	               extract_pair () const noexcept;
	fstb_FORCEINLINE std::tuple <Vf32, Vf32>
	               spread_pairs () const noexcept;

	fstb_FORCEINLINE float
	               sum_h () const noexcept;
	fstb_FORCEINLINE float
	               min_h () const noexcept;
	fstb_FORCEINLINE float
	               max_h () const noexcept;

	fstb_FORCEINLINE bool
	               and_h () const noexcept;
	fstb_FORCEINLINE bool
	               or_h () const noexcept;
	fstb_FORCEINLINE unsigned int
	               movemask () const noexcept;

	static fstb_FORCEINLINE Vf32
	               zero () noexcept;
	static fstb_FORCEINLINE Vf32
	               set_pair (float a0, float a1) noexcept;
	static fstb_FORCEINLINE Vf32
	               set_pair_fill (float a02, float a13) noexcept;
	static fstb_FORCEINLINE Vf32
	               set_pair_dbl (float a01, float a23) noexcept;
	static fstb_FORCEINLINE Vf32
	               set_mask (bool m0, bool m1, bool m2, bool m3) noexcept;
	static fstb_FORCEINLINE Vf32Native
	               signbit_mask () noexcept;
	static fstb_FORCEINLINE Vf32
	               interleave_pair_lo (Vf32 p0, Vf32 p1) noexcept;
	static fstb_FORCEINLINE Vf32
	               interleave_pair_hi (Vf32 p0, Vf32 p1) noexcept;
	static fstb_FORCEINLINE std::tuple <Vf32, Vf32>
	               interleave (Vf32 p0, Vf32 p1) noexcept;
	static fstb_FORCEINLINE std::tuple <Vf32, Vf32>
	               deinterleave (Vf32 i0, Vf32 i1) noexcept;
	static fstb_FORCEINLINE Vf32
	               deinterleave_lo (Vf32 i0, Vf32 i1) noexcept;
	static fstb_FORCEINLINE Vf32
	               deinterleave_hi (Vf32 i0, Vf32 i1) noexcept;
	template <int POS>
	static fstb_FORCEINLINE Vf32
	               compose (Vf32 a, Vf32 b) noexcept;

	template <typename MEM>
	static fstb_FORCEINLINE Vf32
	               load (const MEM *ptr) noexcept;
	template <typename MEM>
	static fstb_FORCEINLINE Vf32
	               loadu (const MEM *ptr) noexcept;
	template <typename MEM>
	static fstb_FORCEINLINE Vf32
	               loadu_part (const MEM *ptr, int n) noexcept;
	template <typename MEM>
	static fstb_FORCEINLINE Vf32
	               loadu_pair (const MEM *ptr) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int32_t _sign32 = INT32_MIN;

	template <typename MEM>
	fstb_FORCEINLINE void
	               storeu_part_n13 (MEM *ptr, int n) const noexcept;

#if ! defined (fstb_HAS_SIMD)
public:
	union Combo
	{
		Vf32Native     _vf32;
		int32_t        _s32 [_length];
		uint32_t       _u32 [_length];
	};
	static_assert (
		sizeof (Combo) == sizeof (Vf32Native),
		"Wrong size for the wrapping combo structure"
	);
#endif
	Vf32Native  _x;
private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class Vf32

static_assert (
	sizeof (Vf32) == sizeof (Vf32Native),
	"Wrong size for the wrapping structure"
);



/*\\\ GLOBAL OPERATORS AND FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_FORCEINLINE Vf32 operator + (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator - (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator * (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator / (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator & (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator | (Vf32 lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator ^ (Vf32 lhs, const Vf32 &rhs) noexcept;

fstb_FORCEINLINE Vf32 operator == (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator != (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator <  (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator <= (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator >  (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 operator >= (const Vf32 &lhs, const Vf32 &rhs) noexcept;

fstb_FORCEINLINE Vf32 abs (const Vf32 &v) noexcept;
fstb_FORCEINLINE Vf32 fma (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept;
fstb_FORCEINLINE Vf32 fms (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept;
fstb_FORCEINLINE Vf32 fnma (const Vf32 &x, const Vf32 &a, const Vf32 &b) noexcept;
fstb_FORCEINLINE Vf32 round (const Vf32 &v) noexcept;
fstb_FORCEINLINE Vf32 min (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 max (const Vf32 &lhs, const Vf32 &rhs) noexcept;
fstb_FORCEINLINE Vf32 limit (const Vf32 &v, const Vf32 &mi, const Vf32 &ma) noexcept;
fstb_FORCEINLINE Vf32 select (Vf32 cond, Vf32 v_t, Vf32 v_f) noexcept;
fstb_FORCEINLINE std::tuple <Vf32, Vf32> swap_if (Vf32 cond, Vf32 lhs, Vf32 rhs) noexcept;
fstb_FORCEINLINE Vf32 sqrt (Vf32 v) noexcept;
fstb_FORCEINLINE Vf32 log2 (Vf32 v) noexcept;
fstb_FORCEINLINE Vf32 exp2 (Vf32 v) noexcept;



}  // namespace fstb



#include "fstb/Vf32.hpp"



#endif   // fstb_Vf32_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
