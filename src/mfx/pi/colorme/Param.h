/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_colorme_Param_HEADER_INCLUDED)
#define mfx_pi_colorme_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/colorme/Cst.h"



namespace mfx
{
namespace pi
{
namespace colorme
{



enum ParamVowel
{
	ParamVowel_TYPE = 0,
	ParamVowel_SET,

	ParamVowel_NBR_ELT
};

enum Param
{
	Param_VOW_MORPH = 0,
	Param_VOW_RESO,
	Param_VOW_Q,
	Param_VOW_TRANSP,
	Param_VOW_NBR_FORM,
	Param_VOW_NBR_VOW,
	Param_VOW_LIST,

	Param_NBR_ELT = Param_VOW_LIST + ParamVowel_NBR_ELT * Cst::_nbr_vow_morph

}; // enum Param



}  // namespace colorme
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/colorme/Param.hpp"



#endif   // mfx_pi_colorme_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
