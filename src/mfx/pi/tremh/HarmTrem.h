/*****************************************************************************

        HarmTrem.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_tremh_HarmTrem_HEADER_INCLUDED)
#define mfx_pi_tremh_HarmTrem_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/Vf32.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/pi/tremh/HarmTremDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>
#include <array>



namespace mfx
{
namespace pi
{
namespace tremh
{



class HarmTrem final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       HarmTrem (piapi::HostInterface &host);



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
		dsp::iir::OnePole
		               _lpf;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::array <BufAlign, 2> BufArray;

	void           clear_buffers ();
	void           update_param (bool force_flag);
	void           update_filter_freq ();
	void           mix_buf (float dst_ptr [], int buf, int nbr_spl, fstb::Vf32 v_gain, bool r_flag, bool copy_flag) const;

	piapi::HostInterface &
	               _host;
	State          _state;

	HarmTremDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_vol;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_tone;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_base;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phase;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phset;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_shape;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_wf;

	dsp::ctrl::lfo::LfoModule
	               _lfo;
	float          _amt;
	float          _bias;
	float          _sat;
	float          _tone;
	float          _stereo;
	float          _lo;       // [-1 ; 1]. -1 = LFO -, 0 = cst, 1 = LFO +, interpolation inbetween.
	float          _hi;       // Same as _lo
	float          _freq;     // Hz
	ChannelArray   _chn_arr;
	BufArray       _buf_arr;  // 0 = bass, 1 = treble



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               HarmTrem ()                               = delete;
	               HarmTrem (const HarmTrem &other)          = delete;
	               HarmTrem (HarmTrem &&other)               = delete;
	HarmTrem &     operator = (const HarmTrem &other)        = delete;
	HarmTrem &     operator = (HarmTrem &&other)             = delete;
	bool           operator == (const HarmTrem &other) const = delete;
	bool           operator != (const HarmTrem &other) const = delete;

}; // class HarmTrem



}  // namespace tremh
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/tremh/HarmTrem.hpp"



#endif   // mfx_pi_tremh_HarmTrem_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
