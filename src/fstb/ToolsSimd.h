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
#include "fstb/Vf32.h"
#include "fstb/Vs32.h"
#include "fstb/Vu32.h"

#include <tuple>

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

	union Combo
	{
		Vf32           _vf32;
		Vs32        _vs32;
		Vu32        _vu32;
		float          _f32 [4];
		int32_t        _s32 [4];
		uint32_t       _u32 [4];
		int16_t        _s16 [8];
		uint16_t       _u16 [8];
	};

	static void    disable_denorm () noexcept;

	static inline Vf32
	               swap_2f32 (Vf32 v) noexcept;

	static inline Vs32
	               reverse_s32 (Vs32 x) noexcept;

	static inline Vf32
	               fmadd (Vf32 x, Vf32 a, Vf32 b) noexcept;
	static inline Vf32
	               fmsub (Vf32 x, Vf32 a, Vf32 b) noexcept;
	static inline Vf32
	               div_approx (Vf32 n, Vf32 d) noexcept;
	static inline Vf32
	               sqrt (Vf32 v) noexcept;
	static inline Vf32
	               sqrt_approx (Vf32 v) noexcept;
	static inline Vf32
	               rsqrt_approx (Vf32 v) noexcept;
	static inline Vf32
	               rsqrt_approx2 (Vf32 v) noexcept;
	template <typename P>
	static fstb_FORCEINLINE Vf32
	               log2_base (Vf32 x, P poly) noexcept;
	static inline Vf32
	               log2_approx2 (Vf32 v) noexcept;
	template <typename P>
	static fstb_FORCEINLINE Vf32
	               exp2_base (Vf32 x, P poly) noexcept;
	static inline Vf32
	               exp2_approx2 (Vf32 v) noexcept;

	static inline Vf32
	               cast_f32 (Vs32 x) noexcept;
	static inline Vs32
	               cast_s32 (Vf32 x) noexcept;

	static inline Vf32
	               butterfly_f32_w64 (Vf32 x) noexcept;
	static inline Vf32
	               butterfly_f32_w32 (Vf32 x) noexcept;

	static inline Vf32
	               interleave_2f32_lo (Vf32 p0, Vf32 p1) noexcept;
	static inline Vf32
	               interleave_2f32_hi (Vf32 p0, Vf32 p1) noexcept;
	static inline void
	               interleave_f32 (Vf32 &i0, Vf32 &i1, Vf32 p0, Vf32 p1) noexcept;
	static inline void
	               deinterleave_f32 (Vf32 &p0, Vf32 &p1, Vf32 i0, Vf32 i1) noexcept;
	static inline Vf32
	               deinterleave_f32_lo (Vf32 i0, Vf32 i1) noexcept;
	static inline Vf32
	               deinterleave_f32_hi (Vf32 i0, Vf32 i1) noexcept;
	static inline void
	               transpose_f32 (Vf32 &a0, Vf32 &a1, Vf32 &a2, Vf32 &a3) noexcept;

	static inline Vf32
	               monofy_2f32_lo (Vf32 v) noexcept;
	static inline Vf32
	               monofy_2f32_hi (Vf32 v) noexcept;

	static inline Vs32
	               conv_f32_to_s32 (Vf32 x) noexcept;
	static inline Vs32
	               round_f32_to_s32 (Vf32 x) noexcept;
	static inline Vs32
	               floor_f32_to_s32 (Vf32 x) noexcept;
	static inline Vf32
	               conv_s32_to_f32 (Vs32 x) noexcept;

	static inline void
	               start_lerp (Vf32 &val_cur, Vf32 &step, float val_beg, float val_end, int size) noexcept;

	template <int N>
	static inline Vs32
	               srai_s32 (Vs32 lhs) noexcept;
	template <int N>
	static inline Vs32
	               srli_s32 (Vs32 lhs) noexcept;
	template <int N>
	static inline Vs32
	               slli_s32 (Vs32 lhs) noexcept;

	template <int SHIFT>
	class Shift
	{
	public:
		static inline Vf32
		               rotate (Vf32 a) noexcept;
		static inline float
		               extract (Vf32 a) noexcept;
		static inline Vf32
		               insert (Vf32 a, float val) noexcept;
		static inline Vf32
		               spread (Vf32 a) noexcept;
		static inline Vf32
		               compose (Vf32 a, Vf32 b) noexcept;

		static inline Vs32
		               rotate (Vs32 a) noexcept;
		static inline int32_t
		               extract (Vs32 a) noexcept;
		static inline Vs32
		               insert (Vs32 a, int32_t val) noexcept;
		static inline Vs32
		               spread (Vs32 a) noexcept;
		static inline Vs32
		               compose (Vs32 a, Vs32 b) noexcept;
	};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int
	               _inv_table_4_len = 1024+1;
	static const float
	               _inv_table_4 [_inv_table_4_len];

	static constexpr int8_t  _sign8  = INT8_MIN;
	static constexpr int16_t _sign16 = INT16_MIN;
	static constexpr int32_t _sign32 = INT32_MIN;
	static constexpr int64_t _sign64 = INT64_MIN;



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



#include "fstb/ToolsSimd.hpp"



#endif   // fstb_ToolsSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
