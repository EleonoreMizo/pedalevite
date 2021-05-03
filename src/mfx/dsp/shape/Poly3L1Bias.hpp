/*****************************************************************************

        Poly3L1Bias.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_Poly3L1Bias_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_Poly3L1Bias_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class BIAS>
float	Poly3L1Bias <BIAS>::process_sample (float sample) noexcept
{
	// Bias
	const float    bias = float (BIAS::num) / float (BIAS::den);
	sample += bias;

	// Waveshapping
	sample = fstb::limit (sample, -1.0f, 1.0f);
	sample += sample * (fabs (sample) - sample * sample);

	// Unbias
	const float    bias_abs = fabs (float (BIAS::num) / float (BIAS::den));
	const float		unbias   = bias + bias * (bias_abs - bias * bias);
	sample -= unbias;

	return sample;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_Poly3L1Bias_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
