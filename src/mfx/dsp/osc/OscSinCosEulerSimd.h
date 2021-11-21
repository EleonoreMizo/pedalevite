/*****************************************************************************

        OscSinCosEulerSimd.h
        Author: Laurent de Soras, 2016

This object must be aligned on 16-byte boundaries!

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSinCosEulerSimd_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosEulerSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/Vf32.h"



namespace mfx
{
namespace dsp
{
namespace osc
{



class OscSinCosEulerSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units_l2 = 2;
	static const int  _nbr_units    = 1 << _nbr_units_l2;

	               OscSinCosEulerSimd () noexcept;
	               OscSinCosEulerSimd (const OscSinCosEulerSimd &other) = default;
	               OscSinCosEulerSimd (OscSinCosEulerSimd &&other) = default;

	               ~OscSinCosEulerSimd ()                          = default;

	OscSinCosEulerSimd &
	               operator = (const OscSinCosEulerSimd &other)    = default;
	OscSinCosEulerSimd &
	               operator = (OscSinCosEulerSimd &&other)         = default;

	void           set_phase (float phase) noexcept;
	void           set_step (float step) noexcept;

	void           step () noexcept;
	fstb::Vf32     get_cos () const noexcept;
	fstb::Vf32     get_sin () const noexcept;
	void           process_block (float cos_ptr [], float sin_ptr [], int nbr_vec) noexcept;
	void           correct () noexcept;
	void           correct_fast () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           resync (float c0, float s0) noexcept;

	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _pos_cos;
	alignas (fstb_SIMD128_ALIGN) fstb::Vf32
	               _pos_sin;
	float          _step_cosn;
	float          _step_sinn;
	float          _step_cos1;
	float          _step_sin1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSinCosEulerSimd &other) const = delete;
	bool           operator != (const OscSinCosEulerSimd &other) const = delete;

}; // class OscSinCosEulerSimd



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/osc/OscSinCosEulerSimd.hpp"



#endif   // mfx_dsp_osc_OscSinCosEulerSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
