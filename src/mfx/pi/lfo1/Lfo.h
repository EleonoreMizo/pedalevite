/*****************************************************************************

        Lfo.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lfo1_Lfo_HEADER_INCLUDED)
#define mfx_pi_lfo1_Lfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/pi/lfo1/LfoDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace lfo1
{



template <bool SLOW>
class Lfo
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Lfo ();
	virtual        ~Lfo () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (piapi::ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           clear_buffers ();
	void           update_param (bool force_flag);

	State          _state;

	LfoDesc <SLOW> _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_base;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_phase;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_phset;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_shape;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_wf;

	dsp::ctrl::lfo::LfoModule
	               _lfo;
	float          _amp;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Lfo <SLOW> (const Lfo <SLOW> &other)        = delete;
	Lfo <SLOW> &   operator = (const Lfo <SLOW> &other)        = delete;
	bool           operator == (const Lfo <SLOW> &other) const = delete;
	bool           operator != (const Lfo <SLOW> &other) const = delete;

}; // class Lfo



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/lfo1/Lfo.hpp"



#endif   // mfx_pi_lfo1_Lfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
