/*****************************************************************************

        InterpPhaseSimd.h
        Author: Laurent de Soras, 2019

Objects of this class should be aligned on 16-bytes boudaries.
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
#if ! defined (mfx_dsp_rspl_InterpPhaseSimd_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpPhaseSimd_HEADER_INCLUDED

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
class InterpPhaseSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  PHASE_LEN      = PL;                   // Samples
	static const int  PHASE_LEN_V128 = (PHASE_LEN + 3) >> 2; // Vectors
	static const int  ARRAY_LEN      = PHASE_LEN_V128 * 4;

	typedef std::array <float, ARRAY_LEN> BufferUnaligned;
	fstb_TYPEDEF_ALIGN (16, BufferUnaligned, Buffer);

	inline void		set_data (int pos, float imp, float dif);
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

	bool           operator == (const InterpPhaseSimd &other) const = delete;
	bool           operator != (const InterpPhaseSimd &other) const = delete;

}; // class InterpPhaseSimd



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpPhaseSimd.hpp"



#endif   // mfx_dsp_rspl_InterpPhaseSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
