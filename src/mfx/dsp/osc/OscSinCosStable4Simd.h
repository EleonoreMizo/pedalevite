/*****************************************************************************

        OscSinCosStable4Simd.h
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
#if ! defined (mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"



namespace mfx
{
namespace dsp
{
namespace osc
{



class OscSinCosStable4Simd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_units = 4;

	               OscSinCosStable4Simd ();
	virtual        ~OscSinCosStable4Simd () = default;

	void           set_step (float angle_rad);

	void           step ();
	void           process_block (float cos_ptr [], float sin_ptr [], int nbr_vec);

	fstb::ToolsSimd::VectF32
	               get_cos () const;
	fstb::ToolsSimd::VectF32
	               get_sin () const;

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_TYPEDEF_ALIGN (16, fstb::ToolsSimd::VectF32, VectF32Align);

	inline void    step (fstb::ToolsSimd::VectF32 &pos_cos, fstb::ToolsSimd::VectF32 &pos_sin, fstb::ToolsSimd::VectF32 alpha, fstb::ToolsSimd::VectF32 beta);

	static inline void
	               compute_step (fstb::ToolsSimd::VectF32 &alpha, fstb::ToolsSimd::VectF32 &beta, float angle_rad);

	VectF32Align   _pos_cos;
	VectF32Align   _pos_sin;
	VectF32Align   _alpha;
	VectF32Align   _beta;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               OscSinCosStable4Simd (const OscSinCosStable4Simd &other) = delete;
	OscSinCosStable4Simd &
	               operator = (const OscSinCosStable4Simd &other)           = delete;
	bool           operator == (const OscSinCosStable4Simd &other) const    = delete;
	bool           operator != (const OscSinCosStable4Simd &other) const    = delete;

}; // class OscSinCosStable4Simd



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/osc/OscSinCosStable4Simd.hpp"



#endif   // mfx_dsp_osc_OscSinCosStable4Simd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
