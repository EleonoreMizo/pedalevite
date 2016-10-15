/*****************************************************************************

        OscNPhase.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscNPhase_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscNPhase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/OscInterface.h"
#include "mfx/dsp/ctrl/lfo/PhaseDist.h"
#include "mfx/dsp/ctrl/lfo/PhaseGenChaos.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class OscNPhase
:	public OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OscNPhase (bool biphase_flag = false);
	               OscNPhase (const OscNPhase &other)  = default;
	virtual        ~OscNPhase ()                       = default;
	OscNPhase &    operator = (const OscNPhase &other) = default;



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

	void				update_period ();

	double         _sample_freq;		// Hz, > 0
	double         _phase;				// [0 ; 1[
	double         _step;
	double         _k_phase;			// [0 ; _nc[
	double         _k_step;
	std::array <double, 2>
	               _variation_arr;
	double         _period;				// s, > 0
	double         _inv_np;				// ]0 ; 1]
	float          _nc;					// >= 1
	int            _np;					// >= 1
	PhaseDist      _phase_dist;
	bool           _inv_flag      = false;
	bool           _unipolar_flag = false;
	bool           _biphase_flag  = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscNPhase &other) const = delete;
	bool           operator != (const OscNPhase &other) const = delete;

}; // class OscNPhase



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/OscNPhase.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscNPhase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
