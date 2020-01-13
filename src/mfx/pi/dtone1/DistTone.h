/*****************************************************************************

        DistTone.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dtone1_DistTone_HEADER_INCLUDED)
#define mfx_pi_dtone1_DistTone_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/dtone1/DistToneDesc.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace dtone1
{



class DistTone final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistTone ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <dsp::iir::Biquad, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_filter_shape ();
	void           update_filter_coef ();

	State          _state;

	DistToneDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_shape_flag;
	fstb::util::NotificationFlag
	               _param_change_freq_flag;

	float          _inv_fs;
	ChannelArray   _chn_arr;
	float          _tone;               // [ 0 ; 1]
	float          _mid;                // [-1 ; 1]
	float          _freq;
	std::array <float, 3>
	               _b_s;
	std::array <float, 3>
	               _a_s;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistTone (const DistTone &other)          = delete;
	DistTone &     operator = (const DistTone &other)        = delete;
	bool           operator == (const DistTone &other) const = delete;
	bool           operator != (const DistTone &other) const = delete;

}; // class DistTone



}  // namespace dtone1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dtone1/DistTone.hpp"



#endif   // mfx_pi_dtone1_DistTone_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
