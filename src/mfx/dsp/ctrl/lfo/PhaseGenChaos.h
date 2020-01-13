/*****************************************************************************

        PhaseGenChaos.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_PhaseGenChaos_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_PhaseGenChaos_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/PhaseGen.h"

#include <random>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class PhaseGenChaos
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_period (double per);
	void           set_phase (double phase);
	void           set_chaos (double chaos);
	void           clear_buffers ();

	void           tick (int nbr_spl);
	double         get_phase () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Dest
	{
		Dest_STAY = 0,
		Dest_GO,

		Dest_NBR_ELT
	};

	enum Dir
	{
		Dir_POS = 0,
		Dir_NEG,

		Dir_NBR_ELT
	};

	inline void    update_retrig_len ();

	int            _retrig_len  = 256;
	int            _retrig_pos  =   0;
	double         _ramp_pos    =   0;  // In [-1 ; 1]
	double         _speed       =   0;  // Speed of the current transition (step per sample)
	double         _phase_shift =   0;  // In [0 ; 1]
	double         _amp         =   1;  // In [0 ; 1]
	int            _chaos       =   0;  // [0 ; 255]. 80 is a good value.
	PhaseGen       _phase_gen;
	std::minstd_rand
	               _rand_gen;
	Dest           _dest        = Dest_STAY;
	Dir            _dir         = Dir_POS;
	bool           _ramp_flag   = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PhaseGenChaos &other) const = delete;
	bool           operator != (const PhaseGenChaos &other) const = delete;

}; // class PhaseGenChaos



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/PhaseGenChaos.hpp"



#endif   // mfx_dsp_ctrl_lfo_PhaseGenChaos_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
