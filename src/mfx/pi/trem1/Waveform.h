/*****************************************************************************

        Waveform.h
        Copyright (c) 2016 Ohm Force

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_trem1_Waveform_HEADER_INCLUDED)
#define mfx_pi_trem1_Waveform_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace trem1
{



enum Waveform
{

		Waveform_SIN = 0,
		Waveform_SQUARE,
		Waveform_TRI,
		Waveform_RAMP_U,
		Waveform_RAMP_D,

		Waveform_NBR_ELT

}; // enum Waveform



}  // namespace trem1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/trem1/Waveform.hpp"



#endif   // mfx_pi_trem1_Waveform_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
