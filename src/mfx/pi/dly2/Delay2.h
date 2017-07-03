/*****************************************************************************

        Delay2.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_Delay2_HEADER_INCLUDED)
#define mfx_pi_dly2_Delay2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/DelayLineBbd.h"
#include "mfx/pi/dly2/StageTaps.h"
#include "mfx/pi/fv/FreeverbCore.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dly2
{



class Delay2
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Delay2 ();
	virtual        ~Delay2 () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class InfoTap
	{
	public:
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag;
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag_input;
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag_delay;
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag_eq;
		fstb::util::NotificationFlagCascadeSingle
		               _param_change_flag_mix;
	};
	typedef std::array <InfoTap, Cst::_nbr_taps> TapArray;

	class InfoLine
	{
	public:
		DelayLineBbd   _delay;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_input;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_delay;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_fdbk;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_eq;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_mix;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_duck;
		fstb::util::NotificationFlagCascadeSingle
			            _param_change_flag_rev;
		dsp::ctrl::Ramp
		               _rev_mix   = dsp::ctrl::Ramp (0);
		bool           _duck_flag = false;
	};
	typedef std::array <InfoLine, Cst::_nbr_lines> LineArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           set_next_block ();
	void           update_duck_state ();

	static void    square_block (float dst_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);

	State          _state;

	Delay2Desc     _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_duck;
	fstb::util::NotificationFlag
	               _param_change_flag;

	TapArray       _tap_arr;
	LineArray      _line_arr;
	StageTaps      _taps;
	fv::FreeverbCore
	               _reverb;
	dsp::dyn::EnvFollowerRms
	               _env_duck;

	BufAlign       _buf_tmp_zone;       // Used by the delay lines
	std::array <BufAlign, Cst::_nbr_lines>
	               _buf_tap_arr;        // Output of the taps, input of the lines
	std::array <BufAlign, Cst::_nbr_lines>
	               _buf_line_arr;       // Intermediate results for the lines
	std::array <BufAlign, Cst::_nbr_lines>
	               _buf_fdbk_arr;
	BufAlign       _buf_duck;

	int            _nbr_lines;          // >= 0
	float          _xfdbk_cur;          // 0 = no cross-feedback, 1 = full
	float          _xfdbk_old;
	float          _duck_time;          // s. Corresponds to the attack time
	bool           _freeze_flag;
	bool           _duck_flag;          // Set if at least one of the lines uses the ducking
	bool           _quick_clean_req_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Delay2 (const Delay2 &other)            = delete;
	Delay2 &       operator = (const Delay2 &other)        = delete;
	bool           operator == (const Delay2 &other) const = delete;
	bool           operator != (const Delay2 &other) const = delete;

}; // class Delay2



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/Delay2.hpp"



#endif   // mfx_pi_dly2_Delay2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
