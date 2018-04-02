/*****************************************************************************

        OnsetDetect.h
        Author: Laurent de Soras, 2018

Inspired by OnsetsDS by Dan Stowell
http://onsetsds.sourceforge.net/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED)
#define mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/dyn/EnvFollowerPeak.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/rspl/InterpolatorOrder0.h"
#include "mfx/pi/osdet/OnsetDetectDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>
#include <array>



namespace mfx
{
namespace pi
{
namespace osdet
{



class OnsetDetect
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OnsetDetect ();
	virtual        ~OnsetDetect () = default;



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

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	State          _state;

	OnsetDetectDesc
	               _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	BufAlign       _buf_env_vol;
	BufAlign       _buf_env_os;
	BufAlign       _buf_old_vol;
	BufAlign       _buf_old_os;
	BufAlign       _buf_tmp;
	dsp::iir::Biquad
	               _prefilter;
	dsp::dyn::EnvFollowerRms
	               _env_vol;
	dsp::dyn::EnvFollowerPeak
	               _env_os;
	dsp::dly::DelayLine
	               _dly_vol;
	dsp::dly::DelayLine
	               _dly_os;
	dsp::rspl::InterpolatorOrder0
	               _interp;

	int            _last_count;
	int            _last_delay;
	bool           _note_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               OnsetDetect (const OnsetDetect &other)       = delete;
	OnsetDetect &  operator = (const OnsetDetect &other)        = delete;
	bool           operator == (const OnsetDetect &other) const = delete;
	bool           operator != (const OnsetDetect &other) const = delete;

}; // class OnsetDetect



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/osdet/OnsetDetect.hpp"



#endif   // mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
