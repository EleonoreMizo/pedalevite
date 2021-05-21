/*****************************************************************************

        Platitude.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_verb2_Platitude_HEADER_INCLUDED)
#define mfx_pi_verb2_Platitude_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/VarBlock.h"
#include "mfx/dsp/iir/DcKiller2p.h"
#include "mfx/dsp/spat/EarlyRef.h"
#include "mfx/dsp/spat/ReverbDattorro.h"
#include "mfx/pi/verb2/PlatitudeDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace verb2
{



class Platitude final
:	public piapi::PluginInterface
{
	static_assert (_max_nbr_chn == 2, "This reverb works only in stereo");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Platitude (piapi::HostInterface &host);
	               ~Platitude () = default;



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
		dsp::spat::EarlyRef <float>
		               _early_reflections;
		BufAlign       _buf_tmp;
		BufAlign       _buf_erf;
		BufAlign       _buf_dif;
		dsp::iir::DcKiller2p
		               _dc_kill_out;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state;

	PlatitudeDesc  _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lvl;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_early;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_dif_inp;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_dif_tnk;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_tank;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_line;

	ChannelArray   _chn_arr;

	dsp::spat::ReverbDattorro
	               _reverb;

	dsp::ctrl::VarBlock
	               _lvl_dry;
	dsp::ctrl::VarBlock
	               _lvl_early;
	dsp::ctrl::VarBlock
	               _lvl_diffuse;

	// Keeps track of the flush parameter so we send the command only
	// on rising edges (false -> true).
	bool           _flush_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Platitude ()                               = delete;
	               Platitude (const Platitude &other)         = delete;
	               Platitude (Platitude &&other)              = delete;
	Platitude &    operator = (const Platitude &other)        = delete;
	Platitude &    operator = (Platitude &&other)             = delete;
	bool           operator == (const Platitude &other) const = delete;
	bool           operator != (const Platitude &other) const = delete;

}; // class Platitude



}  // namespace verb2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/verb2/Platitude.hpp"



#endif   // mfx_pi_verb2_Platitude_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
