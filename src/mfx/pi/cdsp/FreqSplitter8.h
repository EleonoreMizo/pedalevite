/*****************************************************************************

        FreqSplitter8.h
        Author: Laurent de Soras, 2016

The group delay depends on the splitting frequency. Approximate figures:
GD(<fsplit) is close to fs / (fsplit * 2) samples
GD(~fsplit) doubles relatively to GD(~DC)
GD(>fsplit) gets down quickly to almost 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_FreqSplitter8_HEADER_INCLUDED)
#define mfx_pi_cdsp_FreqSplitter8_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/SplitThiele8.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace cdsp
{



class FreqSplitter8
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           clear_buffers ();
	void           set_sample_freq (double sample_freq);
	void           set_split_freq (float freq);
	void           set_thiele_coef (float k);
	void           copy_z_eq (const FreqSplitter8 &other);
	void           process_block (int chn, float dst_l_ptr [], float dst_h_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <
		dsp::iir::SplitThiele8, piapi::PluginInterface::_max_nbr_chn
	> SplitArray;

	SplitArray     _band_split_arr;     // One per channel



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FreqSplitter8 &other) const = delete;
	bool           operator != (const FreqSplitter8 &other) const = delete;

}; // class FreqSplitter8



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cdsp/FreqSplitter8.hpp"



#endif   // mfx_pi_cdsp_FreqSplitter8_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
