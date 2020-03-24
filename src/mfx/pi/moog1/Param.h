/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_moog1_Param_HEADER_INCLUDED)
#define mfx_pi_moog1_Param_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace moog1
{



enum Param
{

	Param_MODE = 0,
	Param_CUTOFF,
	Param_RESO,
	Param_GCOMP,
	Param_SIDEC_ENA,
	Param_SIDEC_AMP,
	Param_SELF_ENA,
	Param_SELF_AMP,
	Param_SELF_LPF,

	Param_NBR_ELT

}; // enum Param



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/moog1/Param.hpp"



#endif   // mfx_pi_moog1_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
