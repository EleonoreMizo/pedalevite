/*****************************************************************************

        DistoDspBounce.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist2_DistoDspBounce_HEADER_INCLUDED)
#define mfx_pi_dist2_DistoDspBounce_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace pi
{
namespace dist2
{



class DistoDspBounce
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistoDspBounce ()  = default;
	virtual        ~DistoDspBounce () = default;

	void           set_sample_freq (double sample_freq);
	void           clear_buffers ();
	inline float   process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE void
	               bounce (float &val, float &speed_max_loc);

	float          _sample_freq   = 44100;
	float          _pos           = 0;
	float          _speed         = 0;
	float          _grav          = 25000000.0f / (_sample_freq * _sample_freq);
	float          _bounce_rate   = 0.5f;  // [0 ; 1[
	float          _prev_val      = 0;
	float          _speed_max     = 2000.0f / _sample_freq;
	float          _tunnel_height = 20.0f;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistoDspBounce (const DistoDspBounce &other)    = delete;
	DistoDspBounce &
	               operator = (const DistoDspBounce &other)        = delete;
	bool           operator == (const DistoDspBounce &other) const = delete;
	bool           operator != (const DistoDspBounce &other) const = delete;

}; // class DistoDspBounce



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/dist2/DistoDspBounce.hpp"



#endif   // mfx_pi_dist2_DistoDspBounce_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
