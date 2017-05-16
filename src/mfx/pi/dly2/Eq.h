/*****************************************************************************

        Eq.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dly2_Eq_HEADER_INCLUDED)
#define mfx_pi_dly2_Eq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/Biquad.h"



namespace mfx
{
namespace pi
{
namespace dly2
{



class Eq
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Eq ()  = default;
	virtual        ~Eq () = default;

	void           set_sample_freq (double sample_freq);
	void           clear_buffers ();
	void           set_freq_lo (float f);
	void           set_freq_hi (float f);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	bool           is_active () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_filter ();

	float          _sample_freq = 0;    // Hz, > 0. 0 = not initialised
	float          _inv_fs      = 0;    // 1 / _sample_freq, 0 = not initialised
	float          _freq_lo     = 16;   // Low-cut frequency, Hz, > 0
	float          _freq_hi     = 20480;   // High-cut frequency, Hz, > 0
	dsp::iir::Biquad
	               _filter;
	bool           _dirty_flag  = true;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Eq (const Eq &other)                = delete;
	Eq &           operator = (const Eq &other)        = delete;
	bool           operator == (const Eq &other) const = delete;
	bool           operator != (const Eq &other) const = delete;

}; // class Eq



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dly2/Eq.hpp"



#endif   // mfx_pi_dly2_Eq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
