/*****************************************************************************

        CoefGenBHMinLobe.h
        Author: Laurent de Soras, 2018

Blackmann-Harris window "minimum 4-term".
1st side lobe at -93 dB, but slow rolloff.
Also called Nuttall window.

From:
Richard Lyons, "Windowing Functions Improve FFT Results - Part I", Test &
Measurement World, June 1998

Albert H. Nuttall, "Some Windows with Very Good Sidelobe Behavior",
IEEE Transactions on Acoustics, Speech, and Signal Processing,
Vol. ASSP-29, No. 1, IEEE, Piscataway, NJ, February 1981.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_CoefGenBHMinLobe_HEADER_INCLUDED)
#define mfx_dsp_wnd_CoefGenBHMinLobe_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace wnd
{



class CoefGenBHMinLobe
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline double  operator () (double pos) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CoefGenBHMinLobe &other) const = delete;
	bool           operator != (const CoefGenBHMinLobe &other) const = delete;

}; // class CoefGenBHMinLobe



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/CoefGenBHMinLobe.hpp"



#endif   // mfx_dsp_wnd_CoefGenBHMinLobe_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

