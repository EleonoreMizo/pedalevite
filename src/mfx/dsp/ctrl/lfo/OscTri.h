/*****************************************************************************

        OscTri.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscTri_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscTri_HEADER_INCLUDED

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



class OscTri
:	public OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OscTri ()                        = default;
	               OscTri (const OscTri &other)     = default;
	virtual        ~OscTri ()                       = default;
	OscTri &       operator = (const OscTri &other) = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::dsp::ctrl::lfo::OscInterface
	virtual void   do_set_sample_freq (double sample_freq);
	virtual void   do_set_period (double per);
	virtual void   do_set_phase (double phase);
	virtual void   do_set_chaos (double chaos);
	virtual void   do_set_phase_dist (double dist);
	virtual void   do_set_phase_dist_offset (double ofs);
	virtual void   do_set_sign (bool inv_flag);
	virtual void   do_set_polarity (bool unipolar_flag);
	virtual void   do_set_variation (int param, double val);
	virtual bool   do_is_using_variation (int param) const;
	virtual void   do_tick (long nbr_spl);
	virtual double do_get_val () const;
	virtual double do_get_phase () const;
	virtual void   do_clear_buffers ();



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	PhaseGenChaos  _phase_gen;
	PhaseDist      _phase_dist;
	bool           _inv_flag      = false;
	bool           _unipolar_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscTri &other) const = delete;
	bool           operator != (const OscTri &other) const = delete;

}; // class OscTri



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/OscTri.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscTri_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
