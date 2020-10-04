/*****************************************************************************

        Synth0.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_syn0_Synth0_HEADER_INCLUDED)
#define mfx_pi_syn0_Synth0_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/env/AdsrRc.h"
#include "mfx/pi/syn0/Synth0Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace syn0
{



class Synth0 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Synth0 ();



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

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	template <typename OP>
	inline void    fix_edge_polyblep (float &x, float &pos, OP op);

	State          _state;

	Synth0Desc     _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	float          _pitch_oct;
	float          _velo;
	dsp::ctrl::env::AdsrRc
	               _env_amp;

	float          _osc_stp;            // cycle/sample
	float          _osc_pos;            // [0 ; 1[
	BufAlign       _buf_syn;
	BufAlign       _buf_env_amp;

	float          _osc_stp_inv;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Synth0 (const Synth0 &other)            = delete;
	Synth0 &       operator = (const Synth0 &other)        = delete;
	bool           operator == (const Synth0 &other) const = delete;
	bool           operator != (const Synth0 &other) const = delete;

}; // class Synth0



}  // namespace syn0
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/syn0/Synth0.hpp"



#endif   // mfx_pi_syn0_Synth0_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
