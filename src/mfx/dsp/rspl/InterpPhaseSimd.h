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

	static constexpr int PHASE_LEN      = PL;                   // Samples
	static constexpr int PHASE_LEN_V128 = (PHASE_LEN + 3) >> 2; // Vectors
	static constexpr int ARRAY_LEN      = PHASE_LEN_V128 * 4;

	typedef std::array <float, ARRAY_LEN> Buffer;

	inline void		set_data (int pos, float imp, float dif) noexcept;
	fstb_FORCEINLINE void
						precompute_impulse (Buffer &imp, float q) const noexcept;
	fstb_FORCEINLINE float
						convolve (const float data_ptr [], const Buffer &imp) const noexcept;
	fstb_FORCEINLINE float
						convolve (const float data_ptr [], float q) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _chk_impulse_not_set = 12345;

	alignas (16) Buffer  // Impulse
	               _imp = Buffer {{ float (_chk_impulse_not_set) }};
	alignas (16) Buffer  // Difference between next impulse and current impulse
	               _dif;



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
