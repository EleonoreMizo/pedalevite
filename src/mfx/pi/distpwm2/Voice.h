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

	explicit       Voice ();
	virtual        ~Voice () = default;

	void           set_pulse_type (PulseType type);
	void           set_pulse_width (float pw);

	void           sync (float age_frac);
	float          process_sample ();
	void           process_block (float dst_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _fade_len = 50; // Fadeout length, in pulse duration

	void           process_block_rect (float dst_ptr [], int nbr_spl);
	void           process_block_ramp (float dst_ptr [], int nbr_spl);
	void           process_block_cycle (float dst_ptr [], int nbr_spl);
	void           process_block_sine (float dst_ptr [], int nbr_spl);
	void           process_block_saw (float dst_ptr [], int nbr_spl);

	static inline float
	               gen_wf (float pos);
	static inline float
	               gen_wf_multi (float pos);
	static inline float
	               gen_poly (float x);

	const float    _lvl;       // Pulse amplitude
	PulseType      _pt;
	float          _pw;        // Pulse duration or half a period, samples, > 1
	float          _pw_inv;    // 1 / _pw
	float          _dur_cycle; // Cycle duration at the current sample, in samples



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
