/*****************************************************************************

        OscVariSlope.h
        Author: Laurent de Soras, 2016

TIME sets the pulse width.
SHAPE sets the slope of the ramps (morphs between triangle and square).

Special cases:

T = 0  , S = 0: Saw up
T = 1  , S = 0: Saw down
T = 0.5, S = 0: Triangle
T = 0.5, S = 1: Square

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscVariSlope_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscVariSlope_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/OscInterface.h"
#include "mfx/dsp/ctrl/lfo/PhaseDist.h"
#include "mfx/dsp/ctrl/lfo/PhaseGenChaos.h"



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class OscVariSlope final
:	public OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::dsp::ctrl::lfo::OscInterface
	void           do_set_sample_freq (double sample_freq) noexcept final;
	void           do_set_period (double per) noexcept final;
	void           do_set_phase (double phase) noexcept final;
	void           do_set_chaos (double chaos) noexcept final;
	void           do_set_phase_dist (double dist) noexcept final;
	void           do_set_phase_dist_offset (double ofs) noexcept final;
	void           do_set_sign (bool inv_flag) noexcept final;
	void           do_set_polarity (bool unipolar_flag) noexcept final;
	void           do_set_variation (int param, double val) noexcept final;
	bool           do_is_using_variation (int param) const noexcept final;
	void           do_tick (int nbr_spl) noexcept final;
	double         do_get_val () const noexcept final;
	double         do_get_phase () const noexcept final;
	void           do_clear_buffers () noexcept final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void				update_slopes () noexcept;

	PhaseGenChaos  _phase_gen;
	PhaseDist      _phase_dist;
	bool           _inv_flag      = false;
	bool           _unipolar_flag = false;

	double			_var_time      =  0;
	double			_var_shape     =  0;
	double			_slope_0       =  0;
	double			_slope_1       =  2;
	double			_base_1        = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscVariSlope &other) const = delete;
	bool           operator != (const OscVariSlope &other) const = delete;

}; // class OscVariSlope



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/OscVariSlope.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscVariSlope_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
