/*****************************************************************************

        RampModule.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ramp_RampModule_HEADER_INCLUDED)
#define mfx_pi_ramp_RampModule_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ramp/CurveType.h"



namespace mfx
{
namespace pi
{
namespace ramp
{



class RampModule
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               RampModule ();
	               RampModule (const RampModule &other) = default;
	               RampModule (RampModule &&other)      = default;

	               ~RampModule ()                       = default;

	RampModule &   operator = (const RampModule &other) = default;
	RampModule &   operator = (RampModule &&other)      = default;

	void           set_sample_freq (double sample_freq);
	void           set_initial_delay (double t);
	void           set_period (double per);
	void           set_phase (double phase);
	void           set_sign (bool inv_flag);
	void           set_type (CurveType type);
	void           set_snh (double ratio);
	void           set_smooth (double ratio);
	void           tick (int nbr_spl);
	double         get_val () const;
	double         get_phase () const;
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_step ();
	void           update_delay ();
	void           update_snh ();
	void           update_smooth ();
	inline void    tick_sub (int nbr_spl);
	float          get_raw_val () const;

	template <int P, int D>
	static float   accelerate (float x);
	static float   fast (float x);
	static float   slow (float x);

	// Real parameters
	double         _sample_freq;		// Hz, > 0
	double         _delay;           // s, >= 0
	double         _period;				// s, > 0
	bool           _inv_flag;
	double         _snh_ratio;       // ratio relative to _period, >= 0. 0 = no Sample & Hold
	double         _smooth;          // ratio relative to _period, >= 0. 0 = no smoothing
	CurveType      _type;            // -1 = no oscillator constructed

	// Internal variables
	int            _pos_delay;
	int            _delay_spl;
	double         _pos;
	double         _step;

	bool           _delay_flag;
	bool           _snh_flag;
	bool           _smooth_flag;

	double         _snh_pos;			// [0 ; 1[
	double         _snh_step;			// Step per sample
	double         _snh_state;

	double         _smooth_state;
	double         _smooth_spl_coef;	// Smoothing LPF coefficient, per sample. ^n for n samples.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RampModule &other) const = delete;
	bool           operator != (const RampModule &other) const = delete;

}; // class RampModule



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ramp/RampModule.hpp"



#endif   // mfx_pi_ramp_RampModule_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
