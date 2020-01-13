/*****************************************************************************

        Delay.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly0_Delay_HEADER_INCLUDED)
#define mfx_pi_dly0_Delay_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/cmd/DelayInterface.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/pi/dly0/DelayDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dly0
{



class Delay final
:	public cmd::DelayInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Delay ();



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

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		mfx::dsp::dly::DelaySimple
		               _delay;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;
	typedef std::array <ChannelArray, _max_nbr_pins> PinArray;

	void           clear_buffers ();
	void           clear_dly_buf_quick ();

	State          _state;

	DelayDesc      _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	PinArray       _pin_arr;
	int            _nbr_pins;
	int            _dly_spl;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Delay (const Delay &other)             = delete;
	Delay &        operator = (const Delay &other)        = delete;
	bool           operator == (const Delay &other) const = delete;
	bool           operator != (const Delay &other) const = delete;

}; // class Delay



}  // namespace dly0
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly0/Delay.hpp"



#endif   // mfx_pi_dly0_Delay_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
