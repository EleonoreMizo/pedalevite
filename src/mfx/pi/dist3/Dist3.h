/*****************************************************************************

        Dist3.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Dist3_HEADER_INCLUDED)
#define mfx_pi_dist3_Dist3_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/pi/dist3/Dist3Desc.h"
#include "mfx/pi/dist3/Dist3Proc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace dist3
{



class Dist3 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Dist3 (piapi::HostInterface &host);
	               ~Dist3 () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef fstb::SingleObj <Dist3Proc, fstb::AllocAlign <Dist3Proc, 16> > ProcAlign;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state;

	Dist3Desc      _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_in;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_out;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_bias;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_psu;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;

	ProcAlign      _proc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Dist3 ()                               = delete;
	               Dist3 (const Dist3 &other)             = delete;
	               Dist3 (Dist3 &&other)                  = delete;
	Dist3 &        operator = (const Dist3 &other)        = delete;
	Dist3 &        operator = (Dist3 &&other)             = delete;
	bool           operator == (const Dist3 &other) const = delete;
	bool           operator != (const Dist3 &other) const = delete;

}; // class Dist3



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Dist3.hpp"



#endif   // mfx_pi_dist3_Dist3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
