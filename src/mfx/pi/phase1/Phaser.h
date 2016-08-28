/*****************************************************************************

        Phaser.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase1_Phaser_HEADER_INCLUDED)
#define mfx_pi_phase1_Phaser_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/pi/phase1/PhasedVoice.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace phase1
{



class Phaser
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Buf
	{
		Buf_SRC = 0,
		Buf_PH_L,
		Buf_PH_R,

		Buf_NBR_ELT
	};

	               Phaser ();
	virtual        ~Phaser () = default;



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

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           update_param (bool force_flag = false);

	State          _state;

	PhaserDesc     _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_osc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_fdbk;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_bpf;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_mix;

	fstb::SingleObj <PhasedVoice>
	               _phased_voice;

	BufAlign       _tmp_buf;
	BufAlign       _tmp_buf_pv;
	int            _mbl_align;
	float          _phase_mix_cur;
	float          _phase_mix_old;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Phaser (const Phaser &other)            = delete;
	Phaser &       operator = (const Phaser &other)        = delete;
	bool           operator == (const Phaser &other) const = delete;
	bool           operator != (const Phaser &other) const = delete;

}; // class Phaser



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase1/Phaser.hpp"



#endif   // mfx_pi_phase1_Phaser_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
