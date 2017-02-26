/*****************************************************************************

        AdsrSeg.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_env_AdsrSeg_HEADER_INCLUDED)
#define mfx_dsp_ctrl_env_AdsrSeg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



enum AdsrSeg
{

	AdsrSeg_NONE = 0,
	AdsrSeg_ATK,
	AdsrSeg_DCY,
	AdsrSeg_SUS,
	AdsrSeg_RLS,

	AdsrSeg_NBR_ELT

}; // enum AdsrSeg



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/env/AdsrSeg.hpp"



#endif   // mfx_dsp_ctrl_env_AdsrSeg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
