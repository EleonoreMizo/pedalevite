/*****************************************************************************

        EnvFollow.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_envf_EnvFollow_HEADER_INCLUDED)
#define mfx_pi_envf_EnvFollow_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/dyn/EnvFollowerAHR4SimdHelper.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/pi/envf/EnvFollowDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace envf
{



class EnvFollow final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       EnvFollow (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::dyn::EnvFollowerAHR4SimdHelper <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		1
	> EnvFolHelper;
	typedef fstb::SingleObj <EnvFolHelper> EnvFolAlign;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	enum Mode
	{
		Mode_LINEAR = 0,
		Mode_LOG,

		Mode_NBR_ELT
	};

	class Channel
	{
	public:
		dsp::iir::OnePole
		               _hpf;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	float          conv_time_to_coef (float t);
	void           square_block (const piapi::ProcInfo &proc);
	void           clip_block (int nbr_spl);

	piapi::HostInterface &
	               _host;
	State          _state;

	EnvFollowDesc  _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_time;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;

	ChannelArray   _chn_arr;

	EnvFolAlign    _envf;
	BufAlign       _buf_src;
	BufAlign       _buf_env;

	dsp::iir::OnePole
	               _hpf;
	float          _hpf_freq;
	Mode           _mode;
	float          _gain;
	float          _thresh;
	float          _log_mul;
	float          _log_add;
	float          _clip_src_lvl;
	bool           _clip_env_flag;
	bool           _hpf_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EnvFollow ()                               = delete;
	               EnvFollow (const EnvFollow &other)         = delete;
	               EnvFollow (EnvFollow &&other)              = delete;
	EnvFollow &    operator = (const EnvFollow &other)        = delete;
	EnvFollow &    operator = (EnvFollow &&other)             = delete;
	bool           operator == (const EnvFollow &other) const = delete;
	bool           operator != (const EnvFollow &other) const = delete;

}; // class EnvFollow



}  // namespace envf
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/envf/EnvFollow.hpp"



#endif   // mfx_pi_envf_EnvFollow_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
