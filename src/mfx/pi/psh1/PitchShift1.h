/*****************************************************************************

        PitchShift1.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_psh1_PitchShift1_HEADER_INCLUDED)
#define mfx_pi_psh1_PitchShift1_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dly/DelayLineReaderPitch.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/wnd/XFadeShape.h"
#include "mfx/pi/psh1/PitchShift1Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace psh1
{



class PitchShift1 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PitchShift1 ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		dsp::dly::DelayLine
		               _delay;
		dsp::dly::DelayLineReaderPitch <float>
		               _reader;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	State          _state;

	PitchShift1Desc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq
	float          _min_dly_time;       // s, > 0. 0 = not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;
	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;
	dsp::wnd::XFadeShape
	               _xfade_shape;
	BufAlign       _tmp_buf;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PitchShift1 (const PitchShift1 &other)        = delete;
	PitchShift1 &  operator = (const PitchShift1 &other)        = delete;
	bool           operator == (const PitchShift1 &other) const = delete;
	bool           operator != (const PitchShift1 &other) const = delete;

}; // class PitchShift1



}  // namespace psh1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/psh1/PitchShift1.hpp"



#endif   // mfx_pi_psh1_PitchShift1_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
