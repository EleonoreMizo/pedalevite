/*****************************************************************************

        Approx.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Approx_HEADER_INCLUDED)
#define fstb_Approx_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <cstdint>



namespace fstb
{



class Approx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static inline ToolsSimd::VectF32
	               sin_rbj (ToolsSimd::VectF32 x) noexcept;
	static inline ToolsSimd::VectF32
	               cos_rbj (ToolsSimd::VectF32 x) noexcept;
	static inline void
	               cos_sin_rbj (ToolsSimd::VectF32 &c, ToolsSimd::VectF32 &s, ToolsSimd::VectF32 x) noexcept;
	template <typename T>
	static inline constexpr T
	               sin_rbj_halfpi (T x) noexcept;
	static inline ToolsSimd::VectF32
	               sin_rbj_halfpi (ToolsSimd::VectF32 x) noexcept;
	static inline ToolsSimd::VectF32
	               sin_rbj_pi (ToolsSimd::VectF32 x) noexcept;
	static inline void
	               sin_rbj_halfpi_pi (float &sx, float &s2x, float x) noexcept;

	template <typename T>
	static inline T
	               sin_nick (T x) noexcept;
	static inline ToolsSimd::VectF32
	               sin_nick (ToolsSimd::VectF32 x) noexcept;
	template <typename T>
	static inline T
	               sin_nick_2pi (T x) noexcept;
	static inline ToolsSimd::VectF32
	               sin_nick_2pi (ToolsSimd::VectF32 x) noexcept;

	static inline float
	               log2 (float val) noexcept;
	static inline float
	               log2_crude (float val) noexcept;
	static inline float
	               exp2 (float val) noexcept;
	static inline float
	               exp2_crude (float val) noexcept;
	static inline float
	               exp2_5th (float val) noexcept;
	template <int A, typename T>
	static inline T
	               exp_m (T val) noexcept;

	static inline double
	               pow (double a, double b) noexcept;

	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32 (int val) noexcept;
	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32_4th (int val) noexcept;

	template <typename T>
	static inline constexpr T
	               tan_taylor5 (T x) noexcept;
	static inline ToolsSimd::VectF32
	               tan_taylor5 (ToolsSimd::VectF32 x) noexcept;
	template <typename T>
	static inline T
	               tan_mystran (T x) noexcept;
	static inline ToolsSimd::VectF32
	               tan_mystran (ToolsSimd::VectF32 x) noexcept;

	template <typename T>
	static inline constexpr T
	               tan_pade55 (T x) noexcept;
	static inline ToolsSimd::VectF32
	               tan_pade55 (ToolsSimd::VectF32 x) noexcept;

	template <typename T>
	static inline constexpr T
	               atan2_3th (T y, T x) noexcept;
	static inline ToolsSimd::VectF32
	               atan2_3th (ToolsSimd::VectF32 y, ToolsSimd::VectF32 x) noexcept;

	template <typename T>
	static inline T
	               tanh_mystran (T x) noexcept;
	static inline ToolsSimd::VectF32
	               tanh_mystran (ToolsSimd::VectF32 x) noexcept;
	template <typename T>
	static inline T
	               tanh_2dat (T x) noexcept;
	static inline ToolsSimd::VectF32
	               tanh_2dat (ToolsSimd::VectF32 x) noexcept;
	template <typename T>
	static inline T
	               tanh_andy (T x) noexcept;

	template <int P = 1>
	static inline float
	               rsqrt (float x) noexcept;
	template <int P = 1>
	static inline double
	               rsqrt (double x) noexcept;

	template <typename T>
	static inline T
	               wright_omega_3 (T x) noexcept;
	static inline ToolsSimd::VectF32
	               wright_omega_3 (ToolsSimd::VectF32 x) noexcept;

	template <typename T>
	static inline T
	               wright_omega_4 (T x) noexcept;
	static inline ToolsSimd::VectF32
	               wright_omega_4 (ToolsSimd::VectF32 x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	union Combo32
	{
		float          _f;
		int32_t        _i;
	};

	static inline ToolsSimd::VectF32
	               restrict_angle_to_mpi_pi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p, const ToolsSimd::VectF32 &tp) noexcept;
	static inline ToolsSimd::VectF32
	               restrict_sin_angle_to_mhpi_hpi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &hpm, const ToolsSimd::VectF32 &hp, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p) noexcept;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Approx ()                               = delete;
	               Approx (const Approx &other)            = delete;
	virtual        ~Approx ()                              = delete;
	Approx &       operator = (const Approx &other)        = delete;
	bool           operator == (const Approx &other) const = delete;
	bool           operator != (const Approx &other) const = delete;

}; // class Approx



}  // namespace fstb



#include "fstb/Approx.hpp"



#endif   // fstb_Approx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
