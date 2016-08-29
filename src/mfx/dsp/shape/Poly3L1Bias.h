/*****************************************************************************

        Poly3L1Bias.h
        Author: Laurent de Soras, 2016

Output is soft-clipped between -1.0 and 1.0.
For bias = 0, output > input for positive values and output < input for
negative values. Thus, applying the formula recursively on any input excepted
0 leads to -1 or +1.
Function is continuous up to its first derivate.

Template parameters:

- BIAS: std::ratio for the bias, in [-1 ; 1]

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_Poly3L1Bias_HEADER_INCLUDED)
#define mfx_dsp_shape_Poly3L1Bias_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <ratio>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <class BIAS>
class Poly3L1Bias
{

	static_assert (BIAS::den != 0, "");
	static_assert (std::ratio_greater <BIAS, std::ratio <-1, 1> >::value, "");
	static_assert (std::ratio_less <   BIAS, std::ratio < 1, 1> >::value, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static fstb_FORCEINLINE float
	               process_sample (float sample);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Poly3L1Bias &other) const = delete;
	bool           operator != (const Poly3L1Bias &other) const = delete;

}; // class Poly3L1Bias



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/Poly3L1Bias.hpp"



#endif   // mfx_dsp_shape_Poly3L1Bias_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
