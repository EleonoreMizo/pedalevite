/*****************************************************************************

        Param.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_hcomb_Param_HEADER_INCLUDED)
#define mfx_pi_hcomb_Param_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/hcomb/Cst.h"



namespace mfx
{
namespace pi
{
namespace hcomb
{



enum ParamVoice
{

	ParamVoice_POLARITY = 0,
	ParamVoice_NOTE,
	ParamVoice_FINETUNE,
	ParamVoice_TIME_MULT,
	ParamVoice_GAIN,

	ParamVoice_NBR_ELT
};

enum Param
{

	Param_TUNE = 0,
	Param_TIME,
	Param_VOLUME,
	Param_FLT_FREQ,
	Param_FLT_RESO,
	Param_FLT_DAMP,
	Param_BASE_VOICE,

	Param_NBR_ELT = Param_BASE_VOICE + ParamVoice_NBR_ELT * Cst::_nbr_voices

}; // enum Param



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/hcomb/Param.hpp"



#endif   // mfx_pi_hcomb_Param_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
