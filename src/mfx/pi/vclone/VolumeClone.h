/*****************************************************************************

        VolumeClone.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_vclone_VolumeClone_HEADER_INCLUDED)
#define mfx_pi_vclone_VolumeClone_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/ctrl/VarBlock.h"
#include "mfx/dsp/dyn/EnvFollowerAHR1LrSimdHelper.h"
#include "mfx/pi/vclone/ChnMode.h"
#include "mfx/pi/vclone/VolumeCloneDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace vclone
{



class VolumeClone final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       VolumeClone (piapi::HostInterface &host);
	               ~VolumeClone () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Filter order for the envelope followers
	static constexpr int    _order_env   = 3;

	// Reference volume for the multiplicative mode. When the signal reaches
	// this volume, the gain is 1.
	static constexpr float  _vol_ref     = 0.125f;

	// Below this threshold, the gain is reduced down to 0, to avoid
	// amplifying the background noise.
	static constexpr float  _thr_silence = 1e-4f; // -80 dB

	// Constant added to denominators that may be null to avoid divisions by 0.
	static constexpr float  _eps         = 1e-15f;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::dyn::EnvFollowerAHR1LrSimdHelper <
		fstb::DataAlign <true>,
		_order_env
	> Follower;

	class Channel
	{
	public:
		Follower    _ef_car;    // Carrier
		Follower    _ef_mod;    // Modulator
		BufAlign    _buf_car;
		BufAlign    _buf_mod;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state;

	VolumeCloneDesc 
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_mode;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_gain;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_env;

	ChannelArray   _chn_arr;

	ChnMode        _chn_mode;
	dsp::ctrl::VarBlock
	               _mul_copy;
	dsp::ctrl::VarBlock
	               _strength;
	dsp::ctrl::VarBlock
	               _gain_min;
	dsp::ctrl::VarBlock
	               _gain_max;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               VolumeClone ()                               = delete;
	               VolumeClone (const VolumeClone &other)       = delete;
	               VolumeClone (VolumeClone &&other)            = delete;
	VolumeClone &  operator = (const VolumeClone &other)        = delete;
	VolumeClone &  operator = (VolumeClone &&other)             = delete;
	bool           operator == (const VolumeClone &other) const = delete;
	bool           operator != (const VolumeClone &other) const = delete;

}; // class VolumeClone



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vclone/VolumeClone.hpp"



#endif   // mfx_pi_vclone_VolumeClone_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
