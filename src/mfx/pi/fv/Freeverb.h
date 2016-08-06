/*****************************************************************************

        Freeverb.h
        Author: Laurent de Soras, 2016

Based on Freeverb by Jezar at Dreampoint (June 2000)

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_fv_Freeverb_HEADER_INCLUDED)
#define mfx_pi_fv_Freeverb_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/pi/fv/FreeverbDesc.h"
#include "mfx/pi/fv/DelayAllPassSimd.h"
#include "mfx/pi/fv/DelayComb.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace fv
{



class Freeverb
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Freeverb ();
	virtual        ~Freeverb () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int
	               _nbr_comb = 8;
	static const int
	               _nbr_ap   = 4;

	typedef std::array <DelayComb, _nbr_comb> CombArray;
	typedef std::array <DelayAllPassSimd, _nbr_ap> AllPassArray;

	class Channel
	{
	public:
		CombArray      _comb_arr;
		AllPassArray   _ap_arr;
		std::array <std::vector <float, fstb::AllocAlign <float, 16> >, 3>
		               _buf_arr;         // 0 = input, 1 = rendering, 2 = accumulate
	};

	typedef std::array <Channel, _max_nbr_chn> ChnArray;


	void           update_param ();

	State          _state;

	FreeverbDesc   _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_lvl;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_flt;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_other;

	ChnArray       _chn_arr;
	float          _src_lvl;
	float          _dry_lvl;
	float          _wet_lvl_direct;
	float          _wet_lvl_cross;

	static const float
	               _scalewet;
	static const int
	               _stereospread;
	static const std::array <int, _nbr_comb>
	               _comb_len_arr;
	static const std::array <int, _nbr_ap>
	               _ap_len_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Freeverb (const Freeverb &other)          = delete;
	Freeverb &     operator = (const Freeverb &other)        = delete;
	bool           operator == (const Freeverb &other) const = delete;
	bool           operator != (const Freeverb &other) const = delete;

}; // class Freeverb



}  // namespace fv
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/fv/Freeverb.hpp"



#endif   // mfx_pi_fv_Freeverb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
