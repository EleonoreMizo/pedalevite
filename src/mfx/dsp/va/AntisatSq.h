/*****************************************************************************

        AntisatSq.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_AntisatSq_HEADER_INCLUDED)
#define mfx_dsp_va_AntisatSq_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class AntisatSq
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static fstb_FORCEINLINE void
	               eval (float &y, float &dy, float x) noexcept;
	static fstb_FORCEINLINE float
	               eval_inv (float y) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               AntisatSq ()                               = delete;
	               AntisatSq (const AntisatSq &other)         = delete;
	               AntisatSq (AntisatSq &&other)              = delete;
	               ~AntisatSq ()                              = delete;
	AntisatSq &    operator = (const AntisatSq &other)        = delete;
	AntisatSq &    operator = (AntisatSq &&other)             = delete;
	bool           operator == (const AntisatSq &other) const = delete;
	bool           operator != (const AntisatSq &other) const = delete;

}; // class AntisatSq



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/AntisatSq.hpp"



#endif   // mfx_dsp_va_AntisatSq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

