/*****************************************************************************

        TransSZBilin.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_TransSZBilin_HEADER_INCLUDED)
#define mfx_dsp_iir_TransSZBilin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <complex>



namespace mfx
{
namespace dsp
{
namespace iir
{



class TransSZBilin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static inline double
	               prewarp_freq (double f0, double fs);
	static double  prewarp_freq_rel_1 (double f0, double fref, double fs);
	static double  prewarp_freq_rel (double f0, double fref, double fs);
	static double  prewarp_freq_rel_mul (double f0, double fref, double fs);
	static std::complex <double>
	               prewarp_root_rel (const std::complex <double> &root, double fref, double fs);
	static void    prewarp_biquad (double b [3], double a [3], double fref, double fs);
	static inline double
	               unwarp_freq (double f_w, double fs);

	template <class TS, class TZ>
	static void    map_s_to_z (TZ z_eq_b [3], TZ z_eq_a [3], const TS s_eq_b [3], const TS s_eq_a [3], double f0, double fs);
	template <class TS, class TZ>
	static void    map_s_to_z_one_pole (TZ z_eq_b [2], TZ z_eq_a [2], const TS s_eq_b [2], const TS s_eq_a [2], double f0, double fs);
	template <class TS, class TZ>
	static void    map_s_to_z_ap1 (TZ z_eq_b [2], double f0, double fs);
	template <class TS, class TZ>
	static void    map_s_to_z_ap2 (TZ z_eq_b [3], TS s_eq_b1, double f0, double fs);

	template <class TS, class TZ>
	static void    map_z_to_s (TS s_eq_b [3], TS s_eq_a [3], const TZ z_eq_b [3], const TZ z_eq_a [3], double f0, double fs);
	template <class TS, class TZ>
	static void    map_z_to_s_one_pole (TS s_eq_b [2], TS s_eq_a [2], const TZ z_eq_b [2], const TZ z_eq_a [2], double f0, double fs);

	static inline float
	               compute_k_approx (float f);
	static inline fstb::ToolsSimd::VectF32
	               compute_k_approx (fstb::ToolsSimd::VectF32 f);
	static void    map_s_to_z_approx (float z_eq_b [3], float z_eq_a [3], const float s_eq_b [3], const float s_eq_a [3], float k);
	static void    map_s_to_z_approx (fstb::ToolsSimd::VectF32 z_eq_b [3], fstb::ToolsSimd::VectF32 z_eq_a [3], const fstb::ToolsSimd::VectF32 s_eq_b [3], const fstb::ToolsSimd::VectF32 s_eq_a [3], fstb::ToolsSimd::VectF32 k);
	static void    map_s_to_z_one_pole_approx (float z_eq_b [2], float z_eq_a [2], const float s_eq_b [2], const float s_eq_a [2], float k);
	static void    map_s_to_z_one_pole_approx (fstb::ToolsSimd::VectF32 z_eq_b [2], fstb::ToolsSimd::VectF32 z_eq_a [2], const fstb::ToolsSimd::VectF32 s_eq_b [2], const fstb::ToolsSimd::VectF32 s_eq_a [2], fstb::ToolsSimd::VectF32 k);
	static void    map_s_to_z_ap1_approx (float z_eq_b [2], float k);
	static void    map_s_to_z_ap1_approx (fstb::ToolsSimd::VectF32 z_eq_b [2], fstb::ToolsSimd::VectF32 k);
	static float   map_s_to_z_ap1_approx_b0 (float k);
	static fstb::ToolsSimd::VectF32
	               map_s_to_z_ap1_approx_b0 (fstb::ToolsSimd::VectF32 k);
	static void    map_s_to_z_ap2_approx (float z_eq_b [3], float s_eq_b1, float k);
	static void    map_s_to_z_ap2_approx (fstb::ToolsSimd::VectF32 z_eq_b [3], fstb::ToolsSimd::VectF32 s_eq_b1, fstb::ToolsSimd::VectF32 k);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TransSZBilin ()                               = delete;
	               TransSZBilin (const TransSZBilin &other)      = delete;
	virtual        ~TransSZBilin ()                              = delete;
	TransSZBilin & operator = (const TransSZBilin &other)        = delete;
	bool           operator == (const TransSZBilin &other) const = delete;
	bool           operator != (const TransSZBilin &other) const = delete;

}; // class TransSZBilin



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/TransSZBilin.hpp"



#endif   // mfx_dsp_iir_TransSZBilin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
