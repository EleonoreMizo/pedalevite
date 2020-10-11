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
	               sin_rbj (ToolsSimd::VectF32 x);
	static inline ToolsSimd::VectF32
	               cos_rbj (ToolsSimd::VectF32 x);
	static inline void
	               cos_sin_rbj (ToolsSimd::VectF32 &c, ToolsSimd::VectF32 &s, ToolsSimd::VectF32 x);
	static inline constexpr float
	               sin_rbj_halfpi (float x);
	static inline ToolsSimd::VectF32
	               sin_rbj_halfpi (ToolsSimd::VectF32 x);
	static inline ToolsSimd::VectF32
	               sin_rbj_pi (ToolsSimd::VectF32 x);
	static inline void
	               sin_rbj_halfpi_pi (float &sx, float &s2x, float x);

	static inline float
	               sin_nick (float x);
	static inline ToolsSimd::VectF32
	               sin_nick (ToolsSimd::VectF32 x);
	static inline float
	               sin_nick_2pi (float x);
	static inline ToolsSimd::VectF32
	               sin_nick_2pi (ToolsSimd::VectF32 x);

	static inline float
	               log2 (float val);
	static inline float
	               log2_crude (float val);
	static inline float
	               exp2 (float val);
	static inline float
	               exp2_crude (float val);
	static inline float
	               exp2_5th (float val);
	template <int A, typename T>
	static inline T
	               exp_m (T val);

	static inline double
	               pow (double a, double b);

	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32 (int val);
	static inline uint32_t
	               fast_partial_exp2_int_16_to_int_32_4th (int val);

	static inline constexpr float
	               tan_taylor5 (float x);
	static inline ToolsSimd::VectF32
	               tan_taylor5 (ToolsSimd::VectF32 x);
	static inline float
	               tan_mystran (float x);
	static inline ToolsSimd::VectF32
	               tan_mystran (ToolsSimd::VectF32 x);

	template <typename T>
	static inline constexpr T
	               tan_pade55 (T x);
	static inline ToolsSimd::VectF32
	               tan_pade55 (ToolsSimd::VectF32 x);

	static inline float
	               tanh_mystran (float x);
	static inline ToolsSimd::VectF32
	               tanh_mystran (ToolsSimd::VectF32 x);
	static inline float
	               tanh_2dat (float x);
	static inline ToolsSimd::VectF32
	               tanh_2dat (ToolsSimd::VectF32 x);
	template <typename T>
	static inline T
	               tanh_andy (T x);

	template <typename T>
	static inline T
	               wright_omega_3 (T x);
	static inline ToolsSimd::VectF32
	               wright_omega_3 (ToolsSimd::VectF32 x);

	template <typename T>
	static inline T
	               wright_omega_4 (T x);
	static inline ToolsSimd::VectF32
	               wright_omega_4 (ToolsSimd::VectF32 x);



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
	               restrict_angle_to_mpi_pi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p, const ToolsSimd::VectF32 &tp);
	static inline ToolsSimd::VectF32
	               restrict_sin_angle_to_mhpi_hpi (ToolsSimd::VectF32 x, const ToolsSimd::VectF32 &hpm, const ToolsSimd::VectF32 &hp, const ToolsSimd::VectF32 &pm, const ToolsSimd::VectF32 &p);



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
