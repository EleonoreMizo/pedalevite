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

#include "fstb/Vf32.h"

#include <array>

#include <cstdint>



namespace fstb
{



class Approx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <typename T>
	static inline constexpr T
	               sin_rbj (T x) noexcept;
	template <typename T>
	static inline T
	               cos_rbj (T x) noexcept;
	static inline void
	               cos_sin_rbj (Vf32 &c, Vf32 &s, Vf32 x) noexcept;
	template <typename T>
	static inline constexpr T
	               sin_rbj_halfpi (T x) noexcept;
	template <typename T>
	static inline constexpr T
	               sin_rbj_pi (T x) noexcept;
	static inline void
	               sin_rbj_halfpi_pi (float &sx, float &s2x, float x) noexcept;

	template <typename T>
	static inline T
	               sin_nick (T x) noexcept;
	template <typename T>
	static inline T
	               sin_nick_2pi (T x) noexcept;

	template <typename T>
	static inline std::array <T, 2>
	               cos_sin_nick_2pi (T x) noexcept;
	static inline std::array <Vf32, 2>
	               cos_sin_nick_2pi (Vf32 x) noexcept;

	static inline float
	               log2 (float val) noexcept;
	static inline Vf32
	               log2 (Vf32 val) noexcept;
	static inline float
	               log2_5th (float val) noexcept;
	static inline Vf32
	               log2_5th (Vf32 val) noexcept;
	static inline float
	               log2_7th (float val) noexcept;
	static inline Vf32
	               log2_7th (Vf32 val) noexcept;
	static inline float
	               log2_crude (float val) noexcept;
	static inline Vf32
	               log2_crude (Vf32 val) noexcept;

	static inline float
	               exp2 (float val) noexcept;
	static inline Vf32
	               exp2 (Vf32 val) noexcept;
	static inline float
	               exp2_5th (float val) noexcept;
	static inline Vf32
	               exp2_5th (Vf32 val) noexcept;
	static inline float
	               exp2_7th (float val) noexcept;
	static inline Vf32
	               exp2_7th (Vf32 val) noexcept;
	static inline float
	               exp2_crude (float val) noexcept;
	static inline Vf32
	               exp2_crude (Vf32 val) noexcept;
	template <int A, typename T>
	static inline constexpr T
	               exp_m (T val) noexcept;

	static inline double
	               pow_crude (double a, double b) noexcept;

	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32 (int val) noexcept;
	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32_4th (int val) noexcept;

	template <typename T>
	static inline constexpr T
	               tan_taylor5 (T x) noexcept;

	template <typename T>
	static inline T
	               tan_mystran (T x) noexcept;
	static inline Vf32
	               tan_mystran (Vf32 x) noexcept;

	template <typename T>
	static inline constexpr T
	               tan_pade33 (T x) noexcept;

	template <typename T>
	static inline constexpr T
	               tan_pade55 (T x) noexcept;

	template <typename T>
	static inline constexpr T
	               atan2_3th (T y, T x) noexcept;
	static inline Vf32
	               atan2_3th (Vf32 y, Vf32 x) noexcept;

	template <typename T>
	static inline constexpr T
	               atan2_7th (T y, T x) noexcept;
	static inline Vf32
	               atan2_7th (Vf32 y, Vf32 x) noexcept;

	template <typename T>
	static inline T
	               tanh_mystran (T x) noexcept;
	template <typename T>
	static inline T
	               tanh_urs (T x) noexcept;
	template <typename T>
	static inline constexpr T
	               tanh_2dat (T x) noexcept;
	static inline Vf32
	               tanh_2dat (Vf32 x) noexcept;
	template <typename T>
	static inline constexpr T
	               tanh_andy (T x) noexcept;

	template <int P = 1>
	static inline float
	               rsqrt (float x) noexcept;
	template <int P = 1>
	static inline double
	               rsqrt (double x) noexcept;
	template <int P = 1>
	static inline Vf32
	               rsqrt (Vf32 x) noexcept;

	template <typename T>
	static inline T
	               wright_omega_3 (T x) noexcept;
	static inline Vf32
	               wright_omega_3 (Vf32 x) noexcept;

	template <typename T>
	static inline T
	               wright_omega_4 (T x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	union Combo32
	{
		float          _f;
		int32_t        _i;
	};

	static inline Vf32
	               restrict_angle_to_mpi_pi (Vf32 x, const Vf32 &pm, const Vf32 &p, const Vf32 &tp) noexcept;
	static inline Vf32
	               restrict_sin_angle_to_mhpi_hpi (Vf32 x, const Vf32 &hpm, const Vf32 &hp, const Vf32 &pm, const Vf32 &p) noexcept;

	template <typename P>
	static fstb_FORCEINLINE float
	               log2_base (float val, P poly) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               log2_poly2 (T x) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               log2_poly5 (T x) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               log2_poly7 (T x) noexcept;

	template <typename P>
	static fstb_FORCEINLINE float
	               exp2_base (float val, P poly) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               exp2_poly2 (T x) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               exp2_poly5 (T x) noexcept;
	template <typename T>
	static fstb_FORCEINLINE constexpr T
	               exp2_poly7 (T x) noexcept;

	template <typename T>
	static fstb_FORCEINLINE T
	               tanh_from_sinh (T x) noexcept;
	static fstb_FORCEINLINE Vf32
	               tanh_from_sinh (Vf32 x) noexcept;

	template <typename T>
	static fstb_FORCEINLINE constexpr std::array <T, 2>
	               atan2_beg (T y, T x) noexcept;
	static fstb_FORCEINLINE std::array <Vf32, 2>
	               atan2_beg (Vf32 y, Vf32 x) noexcept;



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
