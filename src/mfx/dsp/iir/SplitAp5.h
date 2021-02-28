/*****************************************************************************

        SplitAp5.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_SplitAp5_HEADER_INCLUDED)
#define mfx_dsp_iir_SplitAp5_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/AllPass1p.h"
#include "mfx/dsp/iir/AllPass2p.h"
#include "mfx/dsp/BandSplitAllPassPair.h"
#include "mfx/dsp/FilterCascadeIdOdd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



class SplitAp5
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_split_freq (float f);
	void           set_param (float ka, float kb);
	inline bool    is_dirty () const;
	void           update_coef ();
	void           copy_param_from (const SplitAp5 &other);

	void           clear_buffers ();
	inline std::array <float, 2>
	               process_sample_split (float x);
	inline float   process_sample_compensate (float x);
	void           process_block_split (float lo_ptr [], float hi_ptr [], const float src_ptr [], int nbr_spl);
	void           process_block_compensate (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::iir::AllPass2p	Filter0;
	typedef dsp::FilterCascadeIdOdd <
		dsp::iir::AllPass1p,
		dsp::iir::AllPass2p,
		1
	> Filter1;
	typedef dsp::BandSplitAllPassPair <Filter0, Filter1, true> BandSplitApp;

	void           update_filters ();

	float          _sample_freq = 0;    // Hz, > 0. 0 = not set
	float          _inv_fs      = 0;    // s, > 0. 0 = not set

	float          _split_freq  = 0;    // Hz, ]0, fs/2[. 0 = not set
	float          _ka          = 0.5f;
	float          _kb          = 1.f;

	BandSplitApp   _band_split;
	bool           _dirty_flag  = true; // Filters require an update



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SplitAp5 &other) const = delete;
	bool           operator != (const SplitAp5 &other) const = delete;

}; // class SplitAp5



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/SplitAp5.hpp"



#endif   // mfx_dsp_iir_SplitAp5_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
