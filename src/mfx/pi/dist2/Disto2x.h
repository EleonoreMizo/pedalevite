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
#include "fstb/SingleObj.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/dist2/Disto2xDesc.h"
#include "mfx/pi/dist2/DistoStage.h"
#include "mfx/pi/dist2/FreqSplitter.h"
#include "mfx/pi/dist2/TranscientAnalyser.h"
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


class Disto2x
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_stages = 2;

	               Disto2x ();
	virtual        ~Disto2x () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

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
		};
		typedef std::array <Channel, _max_nbr_chn> ChannelArray;

		ChannelArray   _chn_arr;
		TranscientAnalyser
		               _trans_ana;
		FreqSplitter   _freq_split;
		std::array <DistoStage, Disto2x::_nbr_stages>
		               _stage_arr;

	};
	typedef fstb::SingleObj <Proc, fstb::AllocAlign <Proc, 16> > ProcAlign;

	void           update_param (bool force_flag = false);
	void           clear_buffers ();
	void           set_next_buffer ();
	void           update_lpf_pre ();

	State          _state;

	Disto2xDesc    _desc;
	ParamStateSet  _state_set;

	fstb::util::NotificationFlag
	               _param_change_flag;
	std::array <fstb::util::NotificationFlagCascadeSingle, _nbr_stages>
	               _param_change_flag_stage_arr;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_other;

	float          _sample_freq;
	float          _inv_fs;
	ProcAlign      _proc;

	float          _mix_s12_cur;
	float          _mix_s12_old;
	float          _mix_lb_cur;
	float          _mix_lb_old;
	float          _freq_lpf_pre;

	BufAlign       _buf_trans_ana;

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
