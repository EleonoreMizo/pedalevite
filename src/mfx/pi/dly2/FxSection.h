/*****************************************************************************

        FxSection.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_FxSection_HEADER_INCLUDED)
#define mfx_pi_dly2_FxSection_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/pi/dly2/FilterType.h"
#include "mfx/pi/dly2/FxDisto.h"
#include "mfx/pi/freqsh/FreqShiftCore.h"



namespace mfx
{
namespace pi
{

class ParamStateSet;

namespace dly2
{



class FxSection
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FxSection ()  = default;
	virtual        ~FxSection () = default;

	void           init (int line_index, const ParamDescSet &desc_set, ParamStateSet &state_set);
	void           reset (double sample_freq, int max_buf_len);
	void           clear_buffers ();

	void           process_block (float data_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_param (bool force_flag);
	void           update_filter ();
	void           update_shelf ();

	int            _line_index    = -1;
	ParamStateSet* _state_set_ptr = 0;  // 0 = not initialised
	const ParamDescSet *                // 0 = not initialised
	               _desc_set_ptr  = 0;

	float          _sample_freq   = 0;  // Hz, > 0. 0 = not initialised
	float          _inv_fs        = 0;  // 1 / _sample_freq, > 0. 0 = not initialised

	dsp::iir::Biquad
	               _filter;
	FxDisto        _disto;
	dsp::iir::OnePole
	               _shelf_hi;
	freqsh::FreqShiftCore
	               _freq_shift;

	fstb::util::NotificationFlag
		            _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
		            _param_change_flag_filter;
	fstb::util::NotificationFlagCascadeSingle
		            _param_change_flag_dist;
	fstb::util::NotificationFlagCascadeSingle
		            _param_change_flag_shelf;
	fstb::util::NotificationFlagCascadeSingle
		            _param_change_flag_freqsh;

	FilterType     _filter_type   = FilterType_RESO;
	float          _filter_freq   = 1000;
	float          _filter_reso   = 1;
	float          _filter_q      = 1;
	float          _shelf_freq    = 1000;
	float          _shelf_lvl     = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxSection (const FxSection &other)         = delete;
	FxSection &    operator = (const FxSection &other)        = delete;
	bool           operator == (const FxSection &other) const = delete;
	bool           operator != (const FxSection &other) const = delete;

}; // class FxSection



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/FxSection.hpp"



#endif   // mfx_pi_dly2_FxSection_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
