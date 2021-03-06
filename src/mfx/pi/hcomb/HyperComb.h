/*****************************************************************************

        HyperComb.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_hcomb_HyperComb_HEADER_INCLUDED)
#define mfx_pi_hcomb_HyperComb_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/pi/hcomb/Cst.h"
#include "mfx/pi/hcomb/HyperCombDesc.h"
#include "mfx/pi/hcomb/Voice.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace hcomb
{



class HyperComb final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       HyperComb (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class VoicePack
	{
	public:
		Voice          _voice;
		float          _vol           = 1;
		dsp::ctrl::Ramp
		               _vol_final     = dsp::ctrl::Ramp (1);
		float          _note          = 60;
		float          _finetune      = 0;
		float          _decay_mult    = 1;
		bool           _active_flag   = false;
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag;
		fstb::util::NotificationFlagCascadeSingle
							_param_change_flag_comb;
		fstb::util::NotificationFlagCascadeSingle
							_param_change_flag_misc;
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           clear_buffers ();
	void           update_param (bool force_flag);

	piapi::HostInterface &
	               _host;
	State          _state;

	HyperCombDesc  _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_comb;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_filt;
	fstb::util::NotificationFlagCascadeSingle
						_param_change_flag_misc;

	float          _tune;              // semi-tones
	float          _decay;             // s, > 0
	float          _volume;            // >= 0

	std::array <VoicePack, Cst::_nbr_voices>
	               _voice_arr;
	BufAlign       _buf_tmp;
	std::array <BufAlign, _max_nbr_chn>
	               _buf_dst_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               HyperComb ()                               = delete;
	               HyperComb (const HyperComb &other)         = delete;
	               HyperComb (HyperComb &&other)              = delete;
	HyperComb &    operator = (const HyperComb &other)        = delete;
	HyperComb &    operator = (HyperComb &&other)             = delete;
	bool           operator == (const HyperComb &other) const = delete;
	bool           operator != (const HyperComb &other) const = delete;

}; // class HyperComb



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/hcomb/HyperComb.hpp"



#endif   // mfx_pi_hcomb_HyperComb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
