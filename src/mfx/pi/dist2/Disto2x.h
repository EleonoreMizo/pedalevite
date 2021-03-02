/*****************************************************************************

        Disto2x.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist2_Disto2x_HEADER_INCLUDED)
#define mfx_pi_dist2_Disto2x_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/def.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/cdsp/FreqSplitter5.h"
#include "mfx/pi/dist2/Disto2xDesc.h"
#include "mfx/pi/dist2/DistoStage.h"
#include "mfx/pi/dist2/TransientAnalyser.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dist2
{


class Disto2x final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_stages = 2;

	               Disto2x ();



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

#if defined (_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4324)
#endif
	class Proc
	{
	public:
		class Channel
		{
		public:
			dsp::iir::Biquad
			               _lpf_pre;
			std::array <BufAlign, 2>      // 0 = Low, 1 = High
			               _buf_xover_arr;
			std::array <BufAlign, _nbr_stages>
			               _buf_stage_arr;
			dsp::iir::OnePole
			               _dc_killer;
		};
		typedef std::array <Channel, _max_nbr_chn> ChannelArray;

		fstb_TYPEDEF_ALIGN (16, TransientAnalyser, TransientAnalyserAlign);

		ChannelArray   _chn_arr;
		TransientAnalyserAlign
		               _trans_ana;
		cdsp::FreqSplitter5
		               _freq_split;
		std::array <DistoStage, Disto2x::_nbr_stages>
		               _stage_arr;

	};
#if defined (_MSC_VER)
#pragma warning (pop)
#endif
	typedef fstb::SingleObj <Proc, fstb::AllocAlign <Proc, 16> > ProcAlign;

	void           update_param (bool force_flag = false);
	void           clear_buffers ();
	void           set_next_buffer ();
	void           update_lpf_pre ();
	void           square_block (float dst_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);

	State          _state;

	Disto2xDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;

	fstb::util::NotificationFlag
	               _param_change_flag;
	std::array <fstb::util::NotificationFlagCascadeSingle, _nbr_stages>
	               _param_change_flag_stage_arr;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_other;

	float          _sample_freq;
	float          _inv_fs;
	ProcAlign      _proc;

	float          _gmod_atk;           // Multiplier. 1 = neutral
	float          _gmod_sus;           // Multiplier. 1 = neutral
	float          _gmod_atk_l2;
	float          _gmod_sus_l2;
	float          _gmod_atk_max_l2;
	float          _gmod_sus_max_l2;
	float          _mix_s12_cur;
	float          _mix_s12_old;
	float          _mix_lb_cur;
	float          _mix_lb_old;
	float          _freq_lpf_pre;
	float          _density;
	float          _thresh;

	BufAlign       _buf_trans_atk;
	BufAlign       _buf_trans_sus;
	BufAlign       _buf_rms_pre;
	BufAlign       _buf_rms_post;

	dsp::dyn::EnvFollowerRms
	               _env_pre;
	dsp::dyn::EnvFollowerRms
	               _env_post;
	float          _fixgain_cur;
	float          _fixgain_old;

	static const std::array <int, _nbr_stages>
	               _param_stage_base_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Disto2x (const Disto2x &other)           = delete;
	Disto2x &      operator = (const Disto2x &other)        = delete;
	bool           operator == (const Disto2x &other) const = delete;
	bool           operator != (const Disto2x &other) const = delete;

}; // class Disto2x



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/Disto2x.hpp"



#endif   // mfx_pi_dist2_Disto2x_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
