/*****************************************************************************

        FlanchoChn.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_flancho_FlanchoChn_HEADER_INCLUDED)
#define mfx_pi_flancho_FlanchoChn_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/ctrl/lfo/LfoModule.h"
#include	"mfx/dsp/dly/DelayLine.h"
#include	"mfx/dsp/dly/DelayLineReader.h"
#include	"mfx/dsp/dyn/EnvFollowerRms.h"
#include	"mfx/dsp/shape/MapSaturateBipolar.h"
#include	"mfx/pi/flancho/Cst.h"
#include	"mfx/pi/flancho/WfType.h"

#include <array>



namespace mfx
{

namespace dsp
{
namespace rspl
{
	class InterpolatorInterface;
}
}


namespace pi
{
namespace flancho
{



class FlanchoChn
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FlanchoChn (dsp::rspl::InterpolatorInterface &interp, float dly_buf_ptr [], long dly_buf_len, float render_buf_ptr [], long render_buf_len);
	virtual        ~FlanchoChn () = default;

	void           set_sample_freq (double sample_freq);
	void           set_rel_phase (double rel_phase);

	void           set_nbr_voices (int nbr_voices);
	void           set_period (double period);
	void           set_speed (double freq);
	void           set_delay (double delay);
	void           set_depth (double depth);
	void           set_wf_type (WfType wf_type);
	void           set_wf_shape (double shape);
	void           set_feedback (double fdbk);
	void           resync (double base_phase);
	void           process_block (float out_ptr [], const float in_ptr [], long nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum TmpBufType
	{
		TmpBufType_DLY_READ = 0,
		TmpBufType_RENDER,

		TmpBufType_NBR_ELT
	};

	class TmpBufInfo
	{
	public:
		float *        _ptr;
		long           _len;
	};

	typedef	std::array <TmpBufInfo, TmpBufType_NBR_ELT>	TmpBufArray;

	class Voice
	{
	public:
		dsp::dly::DelayLineReader
		               _dly_reader;
		dsp::ctrl::lfo::LfoModule
		               _lfo;
		double         _rel_phase; // Relative to the channel phase. 1 is a whole round, but the variable can contain any positive value.
	};

	typedef	std::array <Voice, Cst::_max_nbr_voices>	VoiceArr;

	// c = 1/16
	// input scale = y'(0) = (1+c) / c = 17
	typedef	dsp::shape::MapSaturateBipolar <
		double,
		std::ratio < 1, 16>,
		std::ratio <17,  1>,
		std::ratio < 1,  1>
	>	SatFnc;

	double         estimate_base_phase () const;
	void           set_wf_type (dsp::ctrl::lfo::LfoModule &lfo, WfType wf_type);
	double         compute_delay_time (dsp::ctrl::lfo::LfoModule &lfo);
	void           update_phase (Voice &voice, double base_phase);
	void           update_shape ();
	void           update_shaper_data ();
	void           update_lfo_param ();
	void           update_max_proc_len ();

	dsp::dly::DelayLine
	               _dly_line;
	VoiceArr       _voice_arr;
	double         _sample_freq;  // Hz, > 0
	double         _rel_phase;    // Relative to the main absolute phase. [0 ; 1[
	TmpBufArray    _tmp_buf_arr;
	int            _nbr_voices;
	double         _period;       // s
	double         _delay;        // s
	double         _depth;        // s
	double         _feedback;
	double         _wf_shape;     // [-1 ; 1]
	WfType         _wf_type;
	long           _max_proc_len; // Maximum length. Depends on the delay.

	double         _feedback_old;

	double         _sat_in_a;
	double         _sat_in_b;
	double         _sat_out_a;
	double         _sat_out_b;

	dsp::dyn::EnvFollowerRms
	               _fdbk_env;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FlanchoChn ()                               = delete;
	               FlanchoChn (const FlanchoChn &other)        = delete;
	FlanchoChn &   operator = (const FlanchoChn &other)        = delete;
	bool           operator == (const FlanchoChn &other) const = delete;
	bool           operator != (const FlanchoChn &other) const = delete;

}; // class FlanchoChn



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/flancho/FlanchoChn.hpp"



#endif   // mfx_pi_flancho_FlanchoChn_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
