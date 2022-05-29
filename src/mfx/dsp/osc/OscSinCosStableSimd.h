/*****************************************************************************

        OscSinCosStableSimd.h
        Author: Laurent de Soras, 2016

This object must be aligned on 16-byte boundaries!

Algorithm from:
Martin Vicanek, A New Recursive Quadrature Oscillator, 2015-10-21
https://vicanek.de/articles/QuadOsc.pdf

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSinCosStableSimd_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStableSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "fstb/Vf32.h"



namespace mfx
{
namespace dsp
{
namespace osc
{



class OscSinCosStableSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units = 4;

	               OscSinCosStableSimd () noexcept;
	               OscSinCosStableSimd (const OscSinCosStableSimd &other) = default;
	               OscSinCosStableSimd (OscSinCosStableSimd &&other)      = default;
	OscSinCosStableSimd &
	               operator = (const OscSinCosStableSimd &other) = default;
	OscSinCosStableSimd &
	               operator = (OscSinCosStableSimd &&other)      = default;

	void           set_step (float angle_rad) noexcept;

	void           step () noexcept;

	template <class VD = fstb::DataAlign <false> >
	void           process_block (float cos_ptr [], float sin_ptr [], int nbr_spl) noexcept;

	inline fstb::Vf32
	               get_cos () const noexcept;
	inline fstb::Vf32
	               get_sin () const noexcept;

	void           clear_buffers () noexcept;
	void           correct_fast () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               update_future (float alpha, float beta);

	template <typename T>
	fstb_FORCEINLINE void
	               step (T &pos_cos, T &pos_sin, T alpha, T beta) noexcept;

	template <typename T>
	static inline void
	               compute_step (T &alpha, T &beta, float angle_rad) noexcept;

	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _pos_cos;
	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _pos_sin;
	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _v_alpha;
	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _v_beta;
	float          _s_alpha = 0;
	float          _s_beta  = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSinCosStableSimd &other) const    = delete;
	bool           operator != (const OscSinCosStableSimd &other) const    = delete;

}; // class OscSinCosStableSimd



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSinCosStableSimd.hpp"



#endif   // mfx_dsp_osc_OscSinCosStableSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
