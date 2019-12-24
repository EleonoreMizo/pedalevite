/*****************************************************************************

        Tremolo.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_trem1_Tremolo_HEADER_INCLUDED)
#define mfx_pi_trem1_Tremolo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/trem1/Waveform.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace trem1
{



class Tremolo
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Tremolo ();
	virtual        ~Tremolo () = default;



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
	float          get_lfo_val (float pos) const;

	State          _state;

	TremoloDesc    _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_shape_flag;
	fstb::util::NotificationFlag
	               _param_change_amp_flag;

	double         _lfo_pos;            // Position within the LFO. [-0.5; 0.5[
	double         _lfo_step;           // Step per sample, >= 0
	Waveform       _lfo_wf;
	float          _amt;
	float          _bias;
	float          _sat;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tremolo (const Tremolo &other)           = delete;
	Tremolo &      operator = (const Tremolo &other)        = delete;
	bool           operator == (const Tremolo &other) const = delete;
	bool           operator != (const Tremolo &other) const = delete;

}; // class Tremolo



}  // namespace trem1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/trem1/Tremolo.hpp"



#endif   // mfx_pi_trem1_Tremolo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
