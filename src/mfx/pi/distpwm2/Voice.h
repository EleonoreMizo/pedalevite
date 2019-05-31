/*****************************************************************************

        Voice.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distpwm2_Voice_HEADER_INCLUDED)
#define mfx_pi_distpwm2_Voice_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/distpwm2/PulseType.h"



namespace mfx
{
namespace pi
{
namespace distpwm2
{



class Voice
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Voice ()  = default;
	virtual        ~Voice () = default;

	void           set_pulse_type (PulseType type);
	void           set_pulse_width (float pw);

	void           sync (float age_frac);
	float          process_sample ();

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static inline float
	               gen_wf (float pos);
	static inline float
	               gen_wf_multi (float pos);
	static inline float
	               gen_poly (float x);

	const float    _lvl       = 0.125f; // Pulse amplitude
	PulseType      _pt        = PulseType_RECT;
	float          _pw        = 100; // Pulse duration or half a period, samples, > 1
	float          _pw_inv    = 1 / _pw;
	float          _dur_cycle = 0;   // Cycle duration at the current sample, in samples



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Voice (const Voice &other)             = delete;
	Voice &        operator = (const Voice &other)        = delete;
	bool           operator == (const Voice &other) const = delete;
	bool           operator != (const Voice &other) const = delete;

}; // class Voice



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distpwm2/Voice.hpp"



#endif   // mfx_pi_distpwm2_Voice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
