/*****************************************************************************

        FreqSplitter.h
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
#if ! defined (mfx_pi_dist2_FreqSplitter_HEADER_INCLUDED)
#define mfx_pi_dist2_FreqSplitter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/AllPass1p.h"
#include "mfx/dsp/iir/AllPass2p.h"
#include "mfx/dsp/BandSplitAllPassPair.h"
#include "mfx/dsp/FilterCascadeIdOdd.h"
#include "mfx/piapi/PluginInterface.h"

#include <memory>
#include <array>



namespace mfx
{
namespace pi
{
namespace dist2
{



class FreqSplitter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FreqSplitter ()  = default;
	virtual        ~FreqSplitter () = default;

	void           clear_buffers ();
	void           set_sample_freq (double sample_freq);
	void           set_split_freq (float freq);
	void           process_block (int chn, float dst_l_ptr [], float dst_h_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::FilterCascadeIdOdd <
		dsp::iir::AllPass1p,
		dsp::iir::AllPass2p,
		1
	> Filter0;
	typedef dsp::iir::AllPass2p	Filter1;
	typedef dsp::BandSplitAllPassPair <Filter0, Filter1, true> BandSplitApp;
	typedef std::array <BandSplitApp, piapi::PluginInterface::_max_nbr_chn> SplitArray;

	void           update_filter ();

	SplitArray     _band_split_arr;     // One per channel
	float          _sample_freq    = 0; // Hz, > 0. 0 = not set
	float          _inv_fs         = 0;
	float          _split_freq     = 5; // Hz, > 0. 0 = not set



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FreqSplitter (const FreqSplitter &other)      = delete;
	FreqSplitter & operator = (const FreqSplitter &other)        = delete;
	bool           operator == (const FreqSplitter &other) const = delete;
	bool           operator != (const FreqSplitter &other) const = delete;

}; // class FreqSplitter



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/FreqSplitter.hpp"



#endif   // mfx_pi_dist2_FreqSplitter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
