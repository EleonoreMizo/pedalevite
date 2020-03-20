/*****************************************************************************

        IvPoly.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_IvPoly_HEADER_INCLUDED)
#define mfx_dsp_va_IvPoly_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



template <int OP = 17, int ON = 5>
class IvPoly
{
	static_assert (OP > 0, "Positive order must be greater than 0.");
	static_assert (ON > 0, "Negative order must be greater than 0.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    set_atten_p (float a);
	inline void    set_atten_n (float a);

	fstb_FORCEINLINE void
	               eval (float &y, float &dy, float x) const;
	fstb_FORCEINLINE float
	               get_max_step (float x) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _attn_p = 1e-3f;
	float          _attn_n = 2e-3f;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const IvPoly <OP, ON> &other) const = delete;
	bool           operator != (const IvPoly <OP, ON> &other) const = delete;

}; // class IvPoly



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/IvPoly.hpp"



#endif   // mfx_dsp_va_IvPoly_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
