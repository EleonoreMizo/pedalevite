/*****************************************************************************

        InterpPhaseFpu.h
        Author: Laurent de Soras, 2019

This class can be inherited but is not polymorph.

Template parameters:

- PL: phase length of the FIR in samples. > 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_rspl_InterpPhaseFpu_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpPhaseFpu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <int PL>
class InterpPhaseFpu
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  PHASE_LEN = PL;   // Samples

	typedef std::array <float, PHASE_LEN> Buffer;

	inline void    set_data (int pos, float imp, float dif);
	fstb_FORCEINLINE void
	               precompute_impulse (Buffer &imp, float q) const;
	fstb_FORCEINLINE float
	               convolve (const float data_ptr [], const Buffer &imp) const;
	fstb_FORCEINLINE float
	               convolve (const float data_ptr [], float q) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  CHK_IMPULSE_NOT_SET = 12345;

	Buffer         _imp = Buffer {{ float (CHK_IMPULSE_NOT_SET) }}; // Impulse
	Buffer         _dif; // Difference between next impulse and current impulse



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               InterpPhaseFpu (const InterpPhaseFpu &other)    = delete;
	InterpPhaseFpu &
	               operator = (const InterpPhaseFpu &other)        = delete;
	bool           operator == (const InterpPhaseFpu &other) const = delete;
	bool           operator != (const InterpPhaseFpu &other) const = delete;

}; // class InterpPhaseFpu



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpPhaseFpu.hpp"



#endif   // mfx_dsp_rspl_InterpPhaseFpu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
