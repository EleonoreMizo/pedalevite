/*****************************************************************************

        OnsetDetect2.h
        Author: Laurent de Soras, 2018

On-set detection algorithm is the time-based method from:

Luca Turchet,
Hard Real-Time Onset Detection of Percussive Sounds,
Proceedings of the 21st International Conference on Digital Audio Effects
(DAFx-18), 2018

Paul Brossier, Juan Pablo Bello, Mark D. Plumbley
Real-Time Temporal Segmentation of Note Objects In Music Signals,
Proceedings of the International Computer Music Conference, 2004

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_osdet2_OnsetDetect2_HEADER_INCLUDED)
#define mfx_pi_osdet2_OnsetDetect2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/dyn/EnvFollowerPeak.h"
#include "mfx/dsp/fir/MovingSum.h"
#include "mfx/dsp/fir/RankSelL.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/pi/osdet2/OnsetDetect2Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>
#include <array>



namespace mfx
{
namespace pi
{
namespace osdet2
{



class OnsetDetect2 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       OnsetDetect2 (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _fs_ref = 44100;  // Reference sampling rate for the on-set detection algorithm. Hz.

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_os_stuff ();


	piapi::HostInterface &
	               _host;
	State          _state;

	OnsetDetect2Desc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	BufAlign       _buf_env_vol;
	BufAlign       _buf_old_vol;
	BufAlign       _buf_tmp;
	BufAlign       _buf_os_dly;
	BufAlign       _buf_sum;
	dsp::iir::Biquad
	               _prefilter;
	dsp::dyn::EnvFollowerRms
	               _env_vol;
	dsp::dly::DelaySimple <float>
	               _dly_vol;

	dsp::dyn::EnvFollowerRms
	               _env_pk;
	dsp::iir::Biquad                    // Derivative + LPF
	               _deriv_smth;
	dsp::fir::MovingSum <float, float>
	               _flt_sum;
	dsp::fir::RankSelL
	               _flt_med;
	dsp::dly::DelaySimple <float>
	               _dly_pk;

	bool           _velo_clip_flag;
	float          _hpf_freq;           // Cutoff freq of the high-pass pre-filter. Hz, > 0
	float          _lpf1_freq;          // Cutoff freq of the envelope detector. Hz, > 0
	float          _lpf2_freq;          // Cutoff freq of the smoother for the envelope derivative. Hz, > 0
	int            _a;                  // Number of past samples for the median/mean window at fs = 44.1 kHz
	int            _b;                  // Number of lookahead samples for the median/mean window at fs = 44.1 kHz
	float          _beta;               // Detection threshold
	float          _alpha;              // Mean % in the dynamic threshold
	float          _lambda;             // Median % in the dynamic threshold
	float          _sum_mul;            // 1 / (a + b), with a and b in real samples

	float          _rls_thr;
	float          _rls_ratio;

	int            _last_count;
	int            _last_delay;         // Minimum delay between two onsets. Samples, > 0
	bool           _note_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               OnsetDetect2 ();
	               OnsetDetect2 (const OnsetDetect2 &other)      = delete;
	               OnsetDetect2 (OnsetDetect2 &&other)           = delete;
	OnsetDetect2 & operator = (const OnsetDetect2 &other)        = delete;
	OnsetDetect2 & operator = (OnsetDetect2 &&other)             = delete;
	bool           operator == (const OnsetDetect2 &other) const = delete;
	bool           operator != (const OnsetDetect2 &other) const = delete;

}; // class OnsetDetect2



}  // namespace osdet2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/osdet2/OnsetDetect2.hpp"



#endif   // mfx_pi_osdet2_OnsetDetect2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
