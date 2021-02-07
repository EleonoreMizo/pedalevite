/*****************************************************************************

        ToolsSimd.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_ToolsSimd_HEADER_INCLUDED)
#define fstb_ToolsSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if ! defined (fstb_HAS_SIMD)
	// Nothing here
#elif (fstb_ARCHI == fstb_ARCHI_X86)
	#include <emmintrin.h>
#elif (fstb_ARCHI == fstb_ARCHI_ARM)
	#include <arm_neon.h>
#else
	#error
#endif

#include <cstddef>
#include <cstdint>



namespace fstb
{



class ToolsSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class VectU32Scalar
	{
	public:
		uint32_t       _ [4];
	};
	class VectS32Scalar
	{
	public:
		int32_t        _ [4];
	};
	class VectF32Scalar
	{
	public:
		float          _ [4];
	};

#if ! defined (fstb_HAS_SIMD)
	typedef VectU32Scalar  VectU32;
	typedef VectS32Scalar  VectS32;
	typedef VectF32Scalar  VectF32;
#elif fstb_ARCHI == fstb_ARCHI_X86
	typedef __m128i        VectU32;
	typedef __m128i        VectS32;
	typedef __m128         VectF32;
#elif fstb_ARCHI == fstb_ARCHI_ARM
	typedef uint32x4_t     VectU32;
	typedef int32x4_t      VectS32;
	typedef float32x4_t    VectF32;
#else
	#error
#endif

	union Combo
	{
		VectF32        _vf32;
		VectS32        _vs32;
		VectU32        _vu32;
		float          _f32 [4];
		int32_t        _s32 [4];
		uint32_t       _u32 [4];
		int16_t        _s16 [8];
		uint16_t       _u16 [8];
	};

	template <typename MEM>
	static inline VectF32
	               load_f32 (const MEM *ptr);
	template <typename MEM>
	static inline VectS32
	               load_s32 (const MEM *ptr);
	template <typename MEM>
	static inline void
	               store_f32 (MEM *ptr, VectF32 v);
	template <typename MEM>
	static inline void
	               store_s32 (MEM *ptr, VectS32 v);
	template <typename MEM>
	static inline void
	               store_f32_part (MEM *ptr, VectF32 v, int n);
	template <typename MEM>
	static inline VectF32
	               loadu_f32 (const MEM *ptr);
	template <typename MEM>
	static inline VectS32
	               loadu_s32 (const MEM *ptr);
	template <typename MEM>
	static inline VectF32
	               loadu_f32_part (const MEM *ptr, int n);
	template <typename MEM>
	static inline void
	               storeu_f32 (MEM *ptr, VectF32 v);
	template <typename MEM>
	static inline void
	               storeu_s32 (MEM *ptr, VectS32 v);
	template <typename MEM>
	static inline void
	               storeu_f32_part (MEM *ptr, VectF32 v, int n);
	template <typename MEM>
	static inline void
	               storeu_s32_part (MEM *ptr, VectS32 v, int n);

	template <typename MEM>
	static inline VectF32
	               loadu_2f32 (const MEM *ptr);
	template <typename MEM>
	static inline void
	               storeu_2f32 (MEM *ptr, VectF32 v);

	template <typename MEM>
	static inline void
	               storeu_1f32 (MEM *ptr, VectF32 v);

	static inline VectF32
	               set_f32_zero ();
	static inline VectS32
	               set_s32_zero ();
	static inline VectF32
	               set1_f32 (float a);
	static inline VectS32
	               set1_s32 (int32_t a);
	static inline VectF32
	               set_f32 (float a0, float a1, float a2, float a3);
	static inline VectS32
	               set_s32 (int32_t a0, int32_t a1, int32_t a2, int32_t a3);
	static inline VectF32
	               set_2f32 (float a0, float a1);
	static inline VectF32
	               set_2f32_fill (float a02, float a13);
	static inline VectF32
	               set_2f32_dbl (float a01, float a23);
	static inline VectF32
	               swap_2f32 (VectF32 v);
	static inline void
	               extract_2f32 (float &a0, float &a1, VectF32 v);
	static inline void
	               spread_2f32 (VectF32 &ra, VectF32 &rb, VectF32 v);


	static inline void
	               mac (VectF32 &s, VectF32 a, VectF32 b);
	static inline void
	               msu (VectF32 &s, VectF32 a, VectF32 b);
	static inline VectF32
	               fmadd (VectF32 x, VectF32 a, VectF32 b);
	static inline VectF32
	               fmsub (VectF32 x, VectF32 a, VectF32 b);
	static inline VectF32
	               min_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectF32
	               max_f32 (VectF32 lhs, VectF32 rhs);
	static inline float
	               sum_h_flt (VectF32 v);
	static inline float
	               min_h_flt (VectF32 v);
	static inline float
	               max_h_flt (VectF32 v);
	static inline VectF32
	               round (VectF32 v);
	static inline VectF32
	               abs (VectF32 v);
	static inline VectF32
	               signbit (VectF32 v);
	static inline VectF32
	               signbit_mask_f32 ();
	static inline VectF32
	               rcp_approx (VectF32 v);
	static inline VectF32
	               rcp_approx2 (VectF32 v);
	static inline VectF32
	               div_approx (VectF32 n, VectF32 d);
	static inline VectF32
	               div_approx2 (VectF32 n, VectF32 d);
	static inline VectF32
	               sqrt (VectF32 v);
	static inline VectF32
	               sqrt_approx (VectF32 v);
	static inline VectF32
	               rsqrt_approx (VectF32 v);
	static inline VectF32
	               rsqrt_approx2 (VectF32 v);
	static inline VectF32
	               log2_approx (VectF32 v);
	static inline VectF32
	               log2_approx2 (VectF32 v);
	static inline VectF32
	               exp2_approx (VectF32 v);
	static inline VectF32
	               exp2_approx2 (VectF32 v);

	static inline VectF32
	               select (VectF32 cond, VectF32 v_t, VectF32 v_f);
	static inline VectF32
	               cmp_gt_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               cmp_gt_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               cmp_lt_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               cmp_lt_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               cmp_eq_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               cmp_eq_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               cmp_ne_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               cmp_ne_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               and_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               and_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               or_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               or_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectF32
	               xor_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectS32
	               xor_s32 (VectS32 lhs, VectS32 rhs);
	static inline bool
	               and_h (VectF32 cond);
	static inline bool
	               and_h (VectS32 cond);
	static inline bool
	               or_h (VectF32 cond);
	static inline bool
	               or_h (VectS32 cond);
	static inline VectF32
	               set_mask_f32 (bool m0, bool m1, bool m2, bool m3);
	static inline int
	               count_bits (VectS32 x);
	static inline unsigned int
	               movemask_f32 (VectF32 x);

	static inline VectF32
	               interleave_2f32_lo (VectF32 p0, VectF32 p1);
	static inline VectF32
	               interleave_2f32_hi (VectF32 p0, VectF32 p1);
	static inline void
	               interleave_f32 (VectF32 &i0, VectF32 &i1, VectF32 p0, VectF32 p1);
	static inline void
	               deinterleave_f32 (VectF32 &p0, VectF32 &p1, VectF32 i0, VectF32 i1);
	static inline VectF32
	               deinterleave_f32_lo (VectF32 i0, VectF32 i1);
	static inline VectF32
	               deinterleave_f32_hi (VectF32 i0, VectF32 i1);
	static inline void
	               transpose_f32 (VectF32 &a0, VectF32 &a1, VectF32 &a2, VectF32 &a3);

	static inline VectF32
	               monofy_2f32_lo (VectF32 v);
	static inline VectF32
	               monofy_2f32_hi (VectF32 v);

	static inline VectS32
	               conv_f32_to_s32 (VectF32 x);
	static inline VectS32
	               round_f32_to_s32 (VectF32 x);
	static inline VectS32
	               floor_f32_to_s32 (VectF32 x);
	static inline VectF32
	               conv_s32_to_f32 (VectS32 x);

	static inline void
	               start_lerp (VectF32 &val_cur, VectF32 &step, float val_beg, float val_end, int size);

	static inline VectS32
	               select (VectS32 cond, VectS32 v_t, VectS32 v_f);
	static inline VectS32
	               min_s32 (VectS32 lhs, VectS32 rhs);
	static inline VectS32
	               max_s32 (VectS32 lhs, VectS32 rhs);

	template <int SHIFT>
	class Shift
	{
	public:
		static inline VectF32
		               rotate (VectF32 a);
		static inline float
		               extract (VectF32 a);
		static inline VectF32
		               insert (VectF32 a, float val);
		static inline VectF32
		               spread (VectF32 a);
		static inline VectF32
		               compose (VectF32 a, VectF32 b);

		static inline VectS32
		               rotate (VectS32 a);
		static inline int32_t
		               extract (VectS32 a);
		static inline VectS32
		               insert (VectS32 a, int32_t val);
		static inline VectS32
		               spread (VectS32 a);
		static inline VectS32
		               compose (VectS32 a, VectS32 b);
	};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int
	               _inv_table_4_len = 1024+1;
	static const float
	               _inv_table_4 [_inv_table_4_len];

	template <typename MEM>
	static inline void
	               store_f32_part_n13 (MEM *ptr, VectF32 v, int n);
	template <typename MEM>
	static inline void
	               store_s32_part_n13 (MEM *ptr, VectS32 v, int n);
	template <typename MEM>
	static inline VectF32
	               load_f32_part_n13 (const MEM *ptr, int n);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ~ToolsSimd ()                              = delete;
	               ToolsSimd ()                               = delete;
	               ToolsSimd (const ToolsSimd &other)         = delete;
	               ToolsSimd (const ToolsSimd &&other)        = delete;
	ToolsSimd &    operator = (const ToolsSimd &other)        = delete;
	ToolsSimd &    operator = (const ToolsSimd &&other)       = delete;
	bool           operator == (const ToolsSimd &other) const = delete;
	bool           operator != (const ToolsSimd &other) const = delete;

}; // class ToolsSimd



}  // namespace fstb



/*
Namespace and type issue:
- Vect* are typedef'd to __m128*, which are in the global namespace
- Vect*Scalar are defined in the fstb namespace.
The namespace where the operators are defined should be the same as the type
of the operands, otherwise the name lookup may fail.
So we have to use the following trick to define everything in the right
namespace.
See also:
https://stackoverflow.com/questions/5195512/namespaces-and-operator-resolution
*/

#if ! defined (fstb_HAS_SIMD)
#define fstb_ToolsSimd_OPNS_BEG  namespace fstb {
#define fstb_ToolsSimd_OPNS( x)  ToolsSimd::x
#define fstb_ToolsSimd_OPNS_END  }
#elif fstb_COMPILER == fstb_COMPILER_MSVC
#define fstb_ToolsSimd_OPNS_BEG
#define fstb_ToolsSimd_OPNS(x)   fstb::ToolsSimd::x
#define fstb_ToolsSimd_OPNS_END
#endif

#if ! defined (fstb_HAS_SIMD) || (fstb_COMPILER == fstb_COMPILER_MSVC)

fstb_ToolsSimd_OPNS_BEG

inline fstb_ToolsSimd_OPNS (VectF32) & operator += (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);
inline fstb_ToolsSimd_OPNS (VectF32) & operator -= (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);
inline fstb_ToolsSimd_OPNS (VectF32) & operator *= (fstb_ToolsSimd_OPNS (VectF32) &lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);

inline fstb_ToolsSimd_OPNS (VectF32) operator + (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);
inline fstb_ToolsSimd_OPNS (VectF32) operator - (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);
inline fstb_ToolsSimd_OPNS (VectF32) operator * (fstb_ToolsSimd_OPNS (VectF32) lhs, const fstb_ToolsSimd_OPNS (VectF32) &rhs);

inline fstb_ToolsSimd_OPNS (VectS32) & operator += (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) & operator -= (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) & operator *= (fstb_ToolsSimd_OPNS (VectS32) &lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) & operator >>= (fstb_ToolsSimd_OPNS (VectS32) &x, int scalar);
inline fstb_ToolsSimd_OPNS (VectS32) & operator <<= (fstb_ToolsSimd_OPNS (VectS32) &x, int scalar);

inline fstb_ToolsSimd_OPNS (VectS32) operator + (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) operator - (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) operator * (fstb_ToolsSimd_OPNS (VectS32) lhs, const fstb_ToolsSimd_OPNS (VectS32) &rhs);
inline fstb_ToolsSimd_OPNS (VectS32) operator >> (fstb_ToolsSimd_OPNS (VectS32) x, int scalar);
inline fstb_ToolsSimd_OPNS (VectS32) operator << (fstb_ToolsSimd_OPNS (VectS32) x, int scalar);

fstb_ToolsSimd_OPNS_END

#endif // Operators



#include "fstb/ToolsSimd.hpp"



#endif   // fstb_ToolsSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
