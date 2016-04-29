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

#if (fstb_ARCHI == fstb_ARCHI_X86)
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

#if fstb_IS (ARCHI, X86)
	typedef __m128i     VectU32;
	typedef __m128i     VectS32;
	typedef __m128      VectF32;
#elif fstb_IS (ARCHI, ARM)
	typedef uint32x4_t  VectU32;
	typedef int32x4_t   VectS32;
	typedef float32x4_t VectF32;
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

	static inline VectF32
	               load_f32 (const void *ptr);
	static inline void
	               store_f32 (void *ptr, VectF32 v);

	static inline VectF32
	               set_f32_zero ();
	static inline VectF32
	               set1_f32 (float a);
	static inline VectF32
	               set_f32 (float a0, float a1, float a2, float a3);
	static inline void
	               mac (VectF32 &s, VectF32 a, VectF32 b);
	static inline VectF32
	               min_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectF32
	               max_f32 (VectF32 lhs, VectF32 rhs);
	static inline float
	               sum_h_flt (VectF32 v);

	static inline VectF32
	               select (VectF32 cond, VectF32 v_t, VectF32 v_f);
	static inline VectF32
	               cmp_gt_f32 (VectF32 lhs, VectF32 rhs);
	static inline VectF32
	               cmp_lt_f32 (VectF32 lhs, VectF32 rhs);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	virtual        ~ToolsSimd ()                              = delete;
	               ToolsSimd ()                               = delete;
	               ToolsSimd (const ToolsSimd &other)         = delete;
	ToolsSimd &    operator = (const ToolsSimd &other)        = delete;
	bool           operator == (const ToolsSimd &other) const = delete;
	bool           operator != (const ToolsSimd &other) const = delete;

}; // class ToolsSimd



}  // namespace fstb



#if fstb_IS (COMPILER, MSVC)

inline fstb::ToolsSimd::VectF32 & operator += (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs);
inline fstb::ToolsSimd::VectF32 & operator -= (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs);
inline fstb::ToolsSimd::VectF32 & operator *= (fstb::ToolsSimd::VectF32 &lhs, fstb::ToolsSimd::VectF32 rhs);

inline fstb::ToolsSimd::VectF32 operator + (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs);
inline fstb::ToolsSimd::VectF32 operator - (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs);
inline fstb::ToolsSimd::VectF32 operator * (fstb::ToolsSimd::VectF32 lhs, fstb::ToolsSimd::VectF32 rhs);

#endif // MSVC



#include "fstb/ToolsSimd.hpp"



#endif   // fstb_ToolsSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
