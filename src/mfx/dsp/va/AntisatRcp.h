/*****************************************************************************

        AntisatRcp.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_AntisatRcp_HEADER_INCLUDED)
#define mfx_dsp_va_AntisatRcp_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class AntisatRcp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static fstb_FORCEINLINE void
	               eval (float &y, float &dy, float x);
	static fstb_FORCEINLINE float
	               eval_inv (float y);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               AntisatRcp ()                               = delete;
	               AntisatRcp (const AntisatRcp &other)        = delete;
	               AntisatRcp (AntisatRcp &&other)             = delete;
	               ~AntisatRcp ()                              = delete;
	AntisatRcp &   operator = (const AntisatRcp &other)        = delete;
	AntisatRcp &   operator = (AntisatRcp &&other)             = delete;
	bool           operator == (const AntisatRcp &other) const = delete;
	bool           operator != (const AntisatRcp &other) const = delete;

}; // class AntisatRcp



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/AntisatRcp.hpp"



#endif   // mfx_dsp_va_AntisatRcp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
