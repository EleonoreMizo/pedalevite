/*****************************************************************************

        OscStepSeq.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscStepSeq_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscStepSeq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/OscInterface.h"
#include "mfx/dsp/ctrl/lfo/PhaseDist.h"
#include "mfx/dsp/ctrl/lfo/PhaseGenChaos.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class OscStepSeq
:	public OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_steps = 32;

	enum Curve
	{
		Curve_HARD = 0,
		Curve_SMOOTH1,
		Curve_SMOOTH2,
		Curve_SMOOTH3,
		Curve_LINEAR,

		Curve_NBR_ELT
	};

	               OscStepSeq ()                        = default;
	               OscStepSeq (const OscStepSeq &other) = default;
	virtual        ~OscStepSeq ()                       = default;
	OscStepSeq &   operator = (const OscStepSeq &other) = default;

	void           set_nbr_steps (int nbr_steps);
	void           set_val (int index, float val);
	void           set_curve (int index, Curve c);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::dsp::ctrl::lfo::OscInterface
	virtual void   do_set_sample_freq (double sample_freq);
	virtual void   do_set_period (double per);
	virtual void   do_set_phase (double phase);
	virtual void   do_set_chaos (double chaos);
	virtual void   do_set_phase_dist (double dist);
	virtual void   do_set_phase_dist_offset (double ofs);
	virtual void   do_set_sign (bool inv_flag);
	virtual void   do_set_polarity (bool unipolar_flag);
	virtual void   do_set_variation (int param, double val);
	virtual bool   do_is_using_variation (int param) const;
	virtual void   do_tick (int nbr_spl);
	virtual double do_get_val () const;
	virtual double do_get_phase () const;
	virtual void   do_clear_buffers ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Step
	{
	public:
		Curve          _curve = Curve_HARD;
		float          _val   = 0;
	};
	typedef std::array <Step, _max_nbr_steps> StepArray;

	static double  map_pos (double pos, Curve c);
	static double  smooth (double x);

	PhaseGenChaos  _phase_gen;
	PhaseDist      _phase_dist;
	bool           _inv_flag      = false;
	bool           _unipolar_flag = false;
	int            _nbr_steps     = 16; // [2 ; _max_nbr_steps]
	StepArray      _step_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscStepSeq &other) const = delete;
	bool           operator != (const OscStepSeq &other) const = delete;

}; // class OscStepSeq



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/OscStepSeq.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscStepSeq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
