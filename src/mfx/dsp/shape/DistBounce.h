/*****************************************************************************

        DistBounce.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_DistBounce_HEADER_INCLUDED)
#define mfx_dsp_shape_DistBounce_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



class DistBounce
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           clear_buffers ();
	inline float   process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	fstb_FORCEINLINE float
	               bounce (float val);

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

	bool           operator == (const DistBounce &other) const = delete;
	bool           operator != (const DistBounce &other) const = delete;

}; // class DistBounce



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/DistBounce.hpp"



#endif   // mfx_dsp_shape_DistBounce_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
