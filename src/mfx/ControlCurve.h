/*****************************************************************************

        ControlCurve.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ControlCurve_HEADER_INCLUDED)
#define mfx_ControlCurve_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



enum ControlCurve
{

	ControlCurve_INVALID = -1,

	ControlCurve_LINEAR = 0,
	ControlCurve_SQ,
	ControlCurve_CB,
	ControlCurve_SQINV,
	ControlCurve_CBINV,
	ControlCurve_S1,
	ControlCurve_S2,
	ControlCurve_FLAT1,
	ControlCurve_FLAT2,
	ControlCurve_DES1,
	ControlCurve_SAT1,
	ControlCurve_DES2,
	ControlCurve_SAT2,
	ControlCurve_DES3,
	ControlCurve_SAT3,
	ControlCurve_DES4,
	ControlCurve_SAT4,
	ControlCurve_STEP1,
	ControlCurve_STEP2,

	ControlCurve_NBR_ELT

}; // enum ControlCurve


const char *  ControlCurve_get_name (ControlCurve c);
float	ControlCurve_apply_curve (float val, ControlCurve curve, bool invert_flag);



}  // namespace mfx



//#include "mfx/ControlCurve.hpp"



#endif   // mfx_ControlCurve_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
