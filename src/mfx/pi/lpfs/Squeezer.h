/*****************************************************************************

        Squeezer.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lpfs_Squeezer_HEADER_INCLUDED)
#define mfx_pi_lpfs_Squeezer_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/SqueezerOpBypass.h"
#include "mfx/dsp/iir/SqueezerOpDefect.h"
#include "mfx/dsp/iir/SqueezerSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/pi/lpfs/SqueezerDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace lpfs
{



class Squeezer final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Squeezer (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _update_resol = 64;  // Must be a multiple of 4

	static const int  _ovrspl      = 4;
	static const int  _nbr_coef_42 = 3;
	static const int  _nbr_coef_21 = 8;

	class Channel
	{
	public:
		typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
		typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;
		typedef dsp::iir::SqueezerSimd <true, dsp::iir::SqueezerOpDefect <5> > Lpf1;
		typedef dsp::iir::SqueezerSimd <true, dsp::iir::SqueezerOpDefect <2> > Lpf2;
		typedef dsp::iir::SqueezerSimd <true, dsp::iir::SqueezerOpBypass     > Lpf3;
		UpSpl          _us;
		DwSpl          _ds;
		Lpf1           _lpf1;
		Lpf2           _lpf2;
		Lpf3           _lpf3;
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state;

	SqueezerDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_freq_reso;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_color;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_drive;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_type;

	float          _drive_gain;
	float          _drive_inv;
	float          _drive_gain_old;
	float          _drive_inv_old;
	int            _type;
	BufAlign       _buf;
	BufAlign       _buf_ovrspl;
	ChannelArray   _chn_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Squeezer ()                               = delete;
	               Squeezer (const Squeezer &other)          = delete;
	               Squeezer (Squeezer &other)                = delete;
	Squeezer &     operator = (const Squeezer &other)        = delete;
	Squeezer &     operator = (Squeezer &other)              = delete;
	bool           operator == (const Squeezer &other) const = delete;
	bool           operator != (const Squeezer &other) const = delete;

}; // class Squeezer



}  // namespace lpfs
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lpfs/Squeezer.hpp"



#endif   // mfx_pi_lpfs_Squeezer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
