/*****************************************************************************

        DistoPwm2.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distpwm2_DistoPwm2_HEADER_INCLUDED)
#define mfx_pi_distpwm2_DistoPwm2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/dyn/EnvFollowerPeak.h"
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/InertiaLin.h"
#include "mfx/pi/distpwm2/DistoPwm2Desc.h"
#include "mfx/pi/distpwm2/PreFilterType.h"
#include "mfx/pi/distpwm2/Voice.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace distpwm2
{



class DistoPwm2
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistoPwm2 ();
	virtual        ~DistoPwm2 () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::iir::Biquad4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Lpf;
	typedef fstb::SingleObj <Lpf, fstb::AllocAlign <Lpf> > LpfAlign;
	typedef std::array <BufAlign, _max_nbr_chn> BufMixArray;

	class VoiceInfo
	{
	public:
		BufAlign       _buf_gen;
		dsp::InertiaLin
		               _vol         = dsp::InertiaLin (0);
		bool           _active_flag = false;
		float          _vol_beg     = 0;
		float          _vol_end     = 0;
	};
	typedef std::array <VoiceInfo, OscType_NBR_ELT> VoiceArray;

	class PeakAnalyser
	{
	public:
		class PeakUnipolar
		{
		public:
			dsp::dyn::EnvFollowerPeak
			               _env;
			std::array <float, 2>   // Previous output samples from the envelope follower
			               _mem;
		};
		std::array <PeakUnipolar, 2>  // Positive, Negative
		               _env_bip;
	};

	class Channel
	{
	public:
		static const int  _nbr_periods = 8; // A power of 2
		static const int  _period_mask = _nbr_periods - 1;
		typedef std::array <Voice, OscType_NBR_ELT> VoiceProcArray;

		dsp::iir::OnePole
		               _hpf_out;
		VoiceProcArray _voice_arr;
		PeakAnalyser   _peak_analyser;
		int            _zx_idx     = 0;  // Increased at each zero-crossing
		float          _spl_prev   = 0;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_prefilter ();

	State          _state;

	DistoPwm2Desc  _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	std::array <fstb::util::NotificationFlagCascadeSingle, OscType_NBR_ELT>
	               _param_change_flag_osc_arr;

	LpfAlign       _filter_in;
	ChannelArray   _chn_arr;
	VoiceArray     _voice_arr;
	PreFilterType  _prefilter;
	float          _threshold;
	BufAlign       _buf_tmp;
	BufMixArray    _buf_mix_arr;
	bool           _peak_det_flag;      // Detection method. True = peak, false = Zero-crossing



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistoPwm2 (const DistoPwm2 &other)         = delete;
	DistoPwm2 &    operator = (const DistoPwm2 &other)        = delete;
	bool           operator == (const DistoPwm2 &other) const = delete;
	bool           operator != (const DistoPwm2 &other) const = delete;

}; // class DistoPwm2



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distpwm2/DistoPwm2.hpp"



#endif   // mfx_pi_distpwm2_DistoPwm2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
