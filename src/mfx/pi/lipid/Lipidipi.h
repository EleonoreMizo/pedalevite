/*****************************************************************************

        Lipidipi.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lipid_Lipidipi_HEADER_INCLUDED)
#define mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/dly/DelayLineReaderPitch.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/wnd/XFadeEqPowC2.h"
#include "mfx/dsp/wnd/XFadeShape.h"
#include "mfx/pi/lipid/Cst.h"
#include "mfx/pi/lipid/LipidipiDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace lipid
{



class Lipidipi final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Lipidipi (piapi::HostInterface &host);
	               ~Lipidipi () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr double _win_dur = 0.100; // s
	static constexpr int _max_nbr_voices = Cst::_max_voice_pairs * 2;

	typedef std::vector <
		float, fstb::AllocAlign <float, fstb_SIMD128_ALIGN>
	> BufAlign;

	class Channel
	{
	public:
		dsp::dly::DelayLine
		               _delay;
		std::array <dsp::dly::DelayLineReaderPitch <float>, _max_nbr_voices>
		               _reader_arr;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	LipidipiDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq  = 0;   // Hz, > 0. <= 0: not initialized
	float          _inv_fs       = 0;   // 1 / _sample_freq
	float          _min_dly_time = 0;   // s, > 0. 0 = not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;
	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;
	dsp::wnd::XFadeShape <dsp::wnd::XFadeEqPowC2>
	               _xfade_shape;
	BufAlign       _buf_dly;
	BufAlign       _buf_mix;

	int            _nbr_vc_pairs = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Lipidipi ()                               = delete;
	               Lipidipi (const Lipidipi &other)          = delete;
	               Lipidipi (Lipidipi &&other)               = delete;
	Lipidipi &     operator = (const Lipidipi &other)        = delete;
	Lipidipi &     operator = (Lipidipi &&other)             = delete;
	bool           operator == (const Lipidipi &other) const = delete;
	bool           operator != (const Lipidipi &other) const = delete;

}; // class Lipidipi



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lipid/Lipidipi.hpp"



#endif   // mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
