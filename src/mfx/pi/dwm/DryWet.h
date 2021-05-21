/*****************************************************************************

        DryWet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dwm_DryWet_HEADER_INCLUDED)
#define mfx_pi_dwm_DryWet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define mfx_pi_dwm_DryWet_GAIN_WET_ONLY

#include "fstb/util/NotificationFlag.h"
#include "mfx/cmd/DelayInterface.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"



namespace mfx
{
namespace pi
{
namespace dwm
{



class DryWet final
:	public cmd::DelayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DryWet (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;

	// mfx::cmd::DelayInterface
	void           do_set_aux_param (int dly_spl, int pin_mult) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Channel
	{
	public:
		mfx::dsp::dly::DelaySimple <float>
		               _delay;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;
	typedef std::array <ChannelArray, _max_nbr_pins> PinArray;

	void           clear_buffers ();
	void           clear_dly_buf_quick ();

	void           set_dw_param (float &dry, float &wet, bool end_flag) const;
	void           copy (int pin_idx, const piapi::ProcInfo &proc, int chn_ofs, float lvl);
	void           mix (int pin_idx, const piapi::ProcInfo &proc, float lvl_wet_beg, float lvl_wet_end, float lvl_dry_beg, float lvl_dry_end);

	piapi::HostInterface &
	               _host;
	State          _state;

	DryWetDesc     _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;

	float          _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	PinArray       _pin_arr;
	int            _nbr_pins;
	int            _dly_spl;

	float          _level_wet;          // For steady state
	float          _level_dry;          // For steady state



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DryWet ()                               = delete;
	               DryWet (const DryWet &other)            = delete;
	               DryWet (DryWet &&other)                 = delete;
	DryWet &       operator = (const DryWet &other)        = delete;
	DryWet &       operator = (DryWet &&other)             = delete;
	bool           operator == (const DryWet &other) const = delete;
	bool           operator != (const DryWet &other) const = delete;

}; // class DryWet



}  // namespace dwm
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dwm/DryWet.hpp"



#endif   // mfx_pi_dwm_DryWet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
