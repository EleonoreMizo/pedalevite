/*****************************************************************************

        Phaser2.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase2_Phaser2_HEADER_INCLUDED)
#define mfx_pi_phase2_Phaser2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/lfo/LfoModule.h"
#include "mfx/dsp/iir/AllPass1pChain.h"
#include "mfx/pi/phase2/Phaser2Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>
#include <array>



namespace mfx
{
namespace pi
{
namespace phase2
{



class Phaser2 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Phaser2 ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _int_block_size = 64;  // Maximum internal block size, samples
	static_assert ((_int_block_size & 3) == 0, "Block size must be multiple of 4");

	class Channel
	{
	public:
		dsp::iir::AllPass1pChain
		               _apf;
		float          _fdbk = 0;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           clear_buffers ();
	void           update_param (bool force_flag);
	float          compute_coef (float f0);
	void           update_filter (float b0);
	void           set_fdbk_pos (int pos);

	static inline constexpr float
	               saturate (float x);

	State          _state;

	Phaser2Desc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_freq;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_base;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phase;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_phset;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_shape;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lfo_wf;

	dsp::ctrl::lfo::LfoModule
	               _lfo;
	int            _nbr_stages;
	int            _fdbk_pos;           // in [1 ; _nbr_stages]
	float          _freq_min_l2;        // log2 (Hz)
	float          _freq_max_l2;        // log2 (Hz)
	float          _feedback;
	float          _mix;
	ChannelArray   _chn_arr;
	BufAlign       _buf;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Phaser2 (const Phaser2 &other)           = delete;
	Phaser2 &      operator = (const Phaser2 &other)        = delete;
	bool           operator == (const Phaser2 &other) const = delete;
	bool           operator != (const Phaser2 &other) const = delete;

}; // class Phaser2



}  // namespace phase2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase2/Phaser2.hpp"



#endif   // mfx_pi_phase2_Phaser2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
