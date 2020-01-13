/*****************************************************************************

        Freeverb.h
        Author: Laurent de Soras, 2016

Based on Freeverb by Jezar at Dreampoint (June 2000)

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_fv_Freeverb_HEADER_INCLUDED)
#define mfx_pi_fv_Freeverb_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/fv/FreeverbCore.h"
#include "mfx/pi/fv/FreeverbDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace fv
{



class Freeverb final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Freeverb ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Channel
	{
	public:
		std::array <std::vector <float, fstb::AllocAlign <float, 16> >, 2>
		               _buf_arr;         // 0 = FV input, 1 = FV output
		dsp::iir::Biquad                 // On the reverb output
		               _filter;
	};

	typedef std::array <Channel, _max_nbr_chn> ChnArray;


	void           clear_buffers ();
	void           update_param ();

	State          _state;

	FreeverbDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lvl;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_flt;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_other;

	FreeverbCore   _fv;
	ChnArray       _chn_arr;
	float          _src_lvl;
	float          _dry_lvl;
	float          _wet_lvl_direct;
	float          _wet_lvl_cross;
	bool           _flt_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Freeverb (const Freeverb &other)          = delete;
	Freeverb &     operator = (const Freeverb &other)        = delete;
	bool           operator == (const Freeverb &other) const = delete;
	bool           operator != (const Freeverb &other) const = delete;

}; // class Freeverb



}  // namespace fv
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/fv/Freeverb.hpp"



#endif   // mfx_pi_fv_Freeverb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
