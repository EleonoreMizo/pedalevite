/*****************************************************************************

        Shaper.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Shaper_HEADER_INCLUDED)
#define mfx_pi_dist3_Shaper_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dyn/LimiterRms.h"
#include "mfx/dsp/shape/DistAttract.h"
#include "mfx/dsp/shape/DistBounce.h"
#include "mfx/dsp/shape/DistRandWalk.h"
#include "mfx/pi/cdsp/ShaperBag.h"



namespace mfx
{
namespace pi
{
namespace dist3
{



class Shaper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Shaper ();
	               ~Shaper () = default;

	enum Type
	{
		Type_ASINH = 0,
		Type_ASYM1,
		Type_PROG1,
		Type_PROG2,
		Type_PROG3,
		Type_SUDDEN,
		Type_HARDCLIP,
		Type_PUNCHER1,
		Type_PUNCHER2,
		Type_PUNCHER3,
		Type_OVERSHOOT,
		Type_BITCRUSH,
		Type_LOPSIDED,
		Type_PORRIDGE,
		Type_SMARTE1,
		Type_SMARTE2,
		Type_ATTRACT,
		Type_RANDWALK,
		Type_SQRT,
		Type_BELT,
		Type_BADMOOD,
		Type_BOUNCE,
		Type_LIGHT1,
		Type_LIGHT2,
		Type_LIGHT3,
		Type_TANH,
		Type_TANHLIN,
		Type_BREAK,
		Type_ASYM2,

		Type_NBR_ELT
	};

	void           reset (double sample_freq, int max_block_size, double &latency);
	void           set_type (Type type);
	float          process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	template <typename S>
	void           distort_block_shaper (S &shaper, float dst_ptr [], const float src_ptr [], int nbr_spl);

	cdsp::ShaperBag &
	               _sbag;
	float          _sample_freq;  // Hz, > 0. 0 = not initialised
	float          _inv_fs;       // s, > 0. 0 = not initialised
	Type           _type;

	dsp::dyn::LimiterRms
	               _porridge_limiter;
	dsp::shape::DistAttract
	               _attractor;
	dsp::shape::DistRandWalk
	               _random_walk;
	dsp::shape::DistBounce
	               _bounce;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Shaper (const Shaper &other)            = delete;
	               Shaper (Shaper &&other)                 = delete;
	Shaper &       operator = (const Shaper &other)        = delete;
	Shaper &       operator = (Shaper &&other)             = delete;
	bool           operator == (const Shaper &other) const = delete;
	bool           operator != (const Shaper &other) const = delete;

}; // class Shaper



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Shaper.hpp"



#endif   // mfx_pi_dist3_Shaper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

