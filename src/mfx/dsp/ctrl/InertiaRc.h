/*****************************************************************************

        InertiaRc.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_InertiaRc_HEADER_INCLUDED)
#define mfx_dsp_ctrl_InertiaRc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/env/SegmentRc.h"



namespace mfx
{
namespace dsp
{
namespace ctrl
{



class InertiaRc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               InertiaRc () noexcept;
	               InertiaRc (const InertiaRc &other)  = default;
	               InertiaRc (InertiaRc &&other)       = default;
	virtual        ~InertiaRc ()                       = default;

	InertiaRc &    operator = (const InertiaRc &other) = default;
	InertiaRc &    operator = (InertiaRc &&other)      = default;

	void           set_sample_freq (float sample_freq) noexcept;
	void           set_inertia_time (float t) noexcept;
	float          get_inertia_time () const noexcept;
	void           set_threshold (float threshold) noexcept;

	void           set_val (float val) noexcept;
	void           force_val (float val) noexcept;
	float          get_val () const noexcept;
	float          get_target_val () const noexcept;
	void           tick (long nbr_spl) noexcept;
	bool           is_ramping () const noexcept;
   void           stop () noexcept;
	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_inertia_time () noexcept;

	float          _sample_freq;        // Hz, > 0
	float          _inertia_time;       // s, >= 0
	float          _start_val;
	float          _final_val;
	float          _threshold;          // > 0
	env::SegmentRc _rc;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const InertiaRc &other) const = delete;
	bool           operator != (const InertiaRc &other) const = delete;

}; // class InertiaRc



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/InertiaRc.hpp"



#endif   // mfx_dsp_ctrl_InertiaRc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
