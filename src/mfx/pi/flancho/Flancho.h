/*****************************************************************************

        Flancho.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_flancho_Flancho_HEADER_INCLUDED)
#define mfx_pi_flancho_Flancho_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/pi/flancho/FlanchoChn.h"
#include "mfx/pi/flancho/FlanchoDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{
namespace pi
{
namespace flancho
{



class Flancho
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Flancho ();
	virtual        ~Flancho () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _update_resol = 64;  // Must be a multiple of 4

	typedef std::shared_ptr <FlanchoChn>       ChnSPtr;
	typedef std::array <ChnSPtr, _max_nbr_chn> ChnArray;

	typedef	std::vector <float>	Buffer;

	void				update_param (bool force_flag = false);
	void				update_lfo_period ();

	State          _state;

	FlanchoDesc    _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_depth_fdbk;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_wf;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_speed;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_delay;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_voices;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_phase_set;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_dry;

	ChnArray			_chn_arr;
	dsp::rspl::InterpolatorHermite43
						_interp;
	Buffer			_buf_tmp;
	Buffer			_buf_render;

	// Cached
	int				_nbr_chn_in;			// > 0. 0 = not set
	int				_nbr_chn_out;			// > 0. 0 = not set
	bool           _dry_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Flancho (const Flancho &other)           = delete;
	Flancho &      operator = (const Flancho &other)        = delete;
	bool           operator == (const Flancho &other) const = delete;
	bool           operator != (const Flancho &other) const = delete;

}; // class Flancho



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/flancho/Flancho.hpp"



#endif   // mfx_pi_flancho_Flancho_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
