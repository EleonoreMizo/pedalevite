/*****************************************************************************

        MidSide.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ms_MidSide_HEADER_INCLUDED)
#define mfx_pi_ms_MidSide_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/pi/ms/MidSideDesc.h"
#include "mfx/pi/ms/Op.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{

namespace dsp
{
	class StereoLevel;
}

namespace pi
{
namespace ms
{



class MidSide final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       MidSide (piapi::HostInterface &host);
	               ~MidSide () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Conf
	{
	public:
		Op             _op        = Op_NONE;
		bool           _swap_flag = false;
		bool           _invl_flag = false;
		bool           _invr_flag = false;
	};

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	static void    compute_matrix (dsp::StereoLevel &mat, const Conf &conf, bool mono_flag);

	piapi::HostInterface &
	               _host;
	State          _state;

	MidSideDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;

	fstb::util::NotificationFlag
	               _param_change_flag;

	Conf           _conf_cur;
	Conf           _conf_old;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MidSide ()                               = delete;
	               MidSide (const MidSide &other)           = delete;
	               MidSide (MidSide &&other)                = delete;
	MidSide &      operator = (const MidSide &other)        = delete;
	MidSide &      operator = (MidSide &&other)             = delete;
	bool           operator == (const MidSide &other) const = delete;
	bool           operator != (const MidSide &other) const = delete;

}; // class MidSide



}  // namespace ms
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ms/MidSide.hpp"



#endif   // mfx_pi_ms_MidSide_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
