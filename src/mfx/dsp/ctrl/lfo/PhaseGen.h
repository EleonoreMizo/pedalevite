/*****************************************************************************

        PhaseGen.h
        Author: Laurent de Soras, 2010

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_PhaseGen_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_PhaseGen_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class PhaseGen
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline         PhaseGen ();
	               PhaseGen (const PhaseGen &other)   = default;
	virtual        ~PhaseGen ()                       = default;
	PhaseGen &     operator = (const PhaseGen &other) = default;

	inline void    set_sample_freq (double sample_freq);
	inline void    set_period (double per);
	inline void    set_phase (double phase);
	fstb_FORCEINLINE void
	               tick (int nbr_spl);
	fstb_FORCEINLINE double
	               get_sample_freq () const;
	fstb_FORCEINLINE double
	               get_period () const;
	fstb_FORCEINLINE double
	               get_phase () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    update_period ();

	double         _phase;           // [0 ; 1[
	double         _step;
	double         _sample_freq;     // Hz, > 0
	double         _period;          // s, > 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseGen &other) const = delete;
	bool           operator != (const PhaseGen &other) const = delete;

}; // class PhaseGen



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/lfo/PhaseGen.hpp"



#endif   // mfx_dsp_ctrl_lfo_PhaseGen_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
