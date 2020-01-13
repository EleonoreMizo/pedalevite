/*****************************************************************************

        PhaseDist.h
        Author: Laurent de Soras, 2016

Moves the mid point (0.5) of a phase, using two linear segments.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_PhaseDist_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_PhaseDist_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class PhaseDist
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    set_phase_dist (double dist);
	inline void    set_phase_dist_offset (double offset);

	inline double  process_phase (double phase) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _thr         = 0.5;   // [0 - 1]
	double         _offset      = 0;     // [0 - 1]
	double         _s1_mul      = 1;
	double         _s2_mul      = 1;
	double         _s2_add      = 0;
	bool           _active_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseDist &other) const = delete;
	bool           operator != (const PhaseDist &other) const = delete;

}; // class PhaseDist



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/lfo/PhaseDist.hpp"



#endif   // mfx_dsp_ctrl_lfo_PhaseDist_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
