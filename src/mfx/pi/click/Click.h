/*****************************************************************************

        Click.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_click_Click_HEADER_INCLUDED)
#define mfx_pi_click_Click_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/env/AdsrRc.h"
#include "mfx/pi/click/ClickDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <random>
#include <vector>



namespace mfx
{
namespace pi
{
namespace click
{



class Click
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Click ();
	virtual        ~Click () = default;



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

	void           update_rates ();

	State          _state;

	ClickDesc      _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	double         _pos_beat;           // >= 0
	double         _tempo;              // BPM
	double         _beat_per_spl;
	int            _beat_per_bar;
	float          _vol;
	float          _gain_bar;           // Relative volume for the first beat of a bar, generally slightly louder

	int            _cur_beat;           // Index of the beat currently playing
	float          _osc_pos;
	float          _osc_inc;
	float          _gain_cur;
	std::minstd_rand
	               _rnd_gen;
	float          _rnd_pos;
	float          _rnd_inc;
	float          _rnd_val;
	float          _rnd_mix;
	uint64_t       _chn_mask;
	dsp::ctrl::env::AdsrRc
	               _env;
	BufAlign       _buf_env;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Click (const Click &other)             = delete;
	Click &        operator = (const Click &other)        = delete;
	bool           operator == (const Click &other) const = delete;
	bool           operator != (const Click &other) const = delete;

}; // class Click



}  // namespace click
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/click/Click.hpp"



#endif   // mfx_pi_click_Click_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
