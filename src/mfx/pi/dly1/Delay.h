/*****************************************************************************

        Delay.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly1_Delay_HEADER_INCLUDED)
#define mfx_pi_dly1_Delay_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/ctrl/InertiaLin.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/pi/dly1/Cst.h"
#include "mfx/pi/dly1/DelayChn.h"
#include "mfx/pi/dly1/DelayDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{
namespace dly1
{



class Delay
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Delay ();
	virtual        ~Delay () = default;



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

	static const int  _update_resol =   64;   // Must be a multiple of 4
	static const int  _tmp_zone_len = 1024;   // Shared temporary zone for all the delay channels

	typedef std::shared_ptr <DelayChn> ChnSPtr;
	typedef std::array <ChnSPtr, Cst::_nbr_lines> ChnArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_param_filter ();
	void           update_param_vol ();
	void           update_param_time ();
	void           process_block_part (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end);
	void           process_block_part_standard (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end);
	void           process_block_part_mono_link (float * const out_ptr_arr [], const float * const in_ptr_arr [], int pos_beg, int pos_end);
	void           update_times (int nbr_spl);
	float          compute_delay_time (int chn) const;

	State          _state;

	DelayDesc      _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_filter_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_vol_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_time_flag;
	fstb::util::NotificationFlagCascadeSingle
						_param_change_link_flag;

	dsp::rspl::InterpolatorHermite43
	               _interp;
	ChnArray       _chn_arr;
	std::vector <float>						// For the channel processors. Size = TMP_ZONE_LEN
	               _tmp_chn_buf;
	std::array <std::vector <float>, Cst::_nbr_lines>  // Mixing buffer. Size = TMP_ZONE_LEN
	               _tmp_buf_arr;
	std::array <dsp::ctrl::InertiaLin, Cst::_nbr_lines>   // Feedback level. Natural value
	               _gain_fdbk_arr;
	dsp::ctrl::InertiaLin
	               _cross_fdbk;
	dsp::ctrl::InertiaLin
	               _lvl_in;
	dsp::ctrl::InertiaLin
	               _lvl_out;
	std::array <float, Cst::_nbr_lines>
	               _delay_time_arr;
	bool           _link_flag;
	bool           _quick_clean_req_flag;

	// Cached
	int				_nbr_chn_in;			// > 0. 0 = not set
	int				_nbr_chn_out;			// > 0. 0 = not set



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Delay (const Delay &other)             = delete;
	Delay &        operator = (const Delay &other)        = delete;
	bool           operator == (const Delay &other) const = delete;
	bool           operator != (const Delay &other) const = delete;

}; // class Delay



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly1/Delay.hpp"



#endif   // mfx_pi_dly1_Delay_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
