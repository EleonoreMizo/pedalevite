/*****************************************************************************

        EnvAdsr.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_adsr_EnvAdsr_HEADER_INCLUDED)
#define mfx_pi_adsr_EnvAdsr_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/ctrl/env/AdsrRc.h"
#include "mfx/pi/adsr/EnvAdsrDesc.h"
#include "mfx/pi/adsr/Param.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace adsr
{



class EnvAdsr
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               EnvAdsr ();
	virtual        ~EnvAdsr () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (piapi::ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int
	               _sus_time_inf = 255; // Infinite sustain value, seconds

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	State          _state;

	EnvAdsrDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_trig;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_adsr;

	dsp::ctrl::env::AdsrRc
	               _env;
	float          _velo;               // ]0 ; 1]
	float          _velo_sens;          // [0 ; 1]. 0: _amp is always 1, 1: _amp = _velo



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               EnvAdsr (const EnvAdsr &other)           = delete;
	EnvAdsr &      operator = (const EnvAdsr &other)        = delete;
	bool           operator == (const EnvAdsr &other) const = delete;
	bool           operator != (const EnvAdsr &other) const = delete;

}; // class EnvAdsr



}  // namespace adsr
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/adsr/EnvAdsr.hpp"



#endif   // mfx_pi_adsr_EnvAdsr_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
