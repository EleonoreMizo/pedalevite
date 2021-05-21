/*****************************************************************************

        StereoPan.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_pan_StereoPan_HEADER_INCLUDED)
#define mfx_pi_pan_StereoPan_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/dsp/ctrl/VarBlock.h"
#include "mfx/pi/pan/StereoPanDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace pan
{



class StereoPan final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       StereoPan (piapi::HostInterface &host);
	               ~StereoPan () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	static void    compute_matrix (dsp::StereoLevel &mat, float pos, float pos_l, float pos_r, float law, bool mono_flag);
	static float   compute_pan_l (float pos, float law);

	piapi::HostInterface &
	               _host;
	State          _state;

	StereoPanDesc  _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	dsp::ctrl::VarBlock
	               _pos;
	dsp::ctrl::VarBlock
	               _pos_l;
	dsp::ctrl::VarBlock
	               _pos_r;
	dsp::ctrl::VarBlock
	               _law;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               StereoPan ()                               = delete;
	               StereoPan (const StereoPan &other)         = delete;
	               StereoPan (StereoPan &&other)              = delete;
	StereoPan &    operator = (const StereoPan &other)        = delete;
	StereoPan &    operator = (StereoPan &&other)             = delete;
	bool           operator == (const StereoPan &other) const = delete;
	bool           operator != (const StereoPan &other) const = delete;

}; // class StereoPan



}  // namespace pan
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/pan/StereoPan.hpp"



#endif   // mfx_pi_pan_StereoPan_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
