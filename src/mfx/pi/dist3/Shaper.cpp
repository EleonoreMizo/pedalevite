/*****************************************************************************

        Shaper.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/shape/WsAsym1.h"
#include "mfx/dsp/shape/WsBadmood.h"
#include "mfx/dsp/shape/WsBelt.h"
#include "mfx/dsp/shape/WsBitcrush.h"
#include "mfx/dsp/shape/WsHardclip.h"
#include "mfx/dsp/shape/WsLight1.h"
#include "mfx/dsp/shape/WsLight2.h"
#include "mfx/dsp/shape/WsLight3.h"
#include "mfx/dsp/shape/WsRcp1.h"
#include "mfx/dsp/shape/WsRcp2.h"
#include "mfx/dsp/shape/WsSqrt.h"
#include "mfx/pi/dist3/Shaper.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Shaper::Shaper ()
:	_sbag (cdsp::ShaperBag::use ())
,	_sample_freq (0)
,	_inv_fs (0)
,	_type (Type_ASINH)
,	_porridge_limiter ()
,	_attractor ()
,	_random_walk ()
,	_bounce ()
{
	dsp::mix::Align::setup ();
	_porridge_limiter.set_time (0.001f);
	_porridge_limiter.set_level (1.0f);
}



void	Shaper::reset (double sample_freq, int max_block_size, double &latency)
{
	fstb::unused (max_block_size);
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);

	_porridge_limiter.set_sample_freq (sample_freq);
	_attractor.set_sample_freq (sample_freq);
	_random_walk.set_sample_freq (sample_freq);
	_bounce.set_sample_freq (sample_freq);

	latency = 0;
}



void	Shaper::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



float	Shaper::process_sample (float x)
{
	switch (_type)
	{
	case Type_ASINH:
		x = _sbag._s_diode (x);
		break;
	case Type_ASYM1:
		x = dsp::shape::WsAsym1::process_sample (x);
		break;

	default:
		assert (false);
		break;
	}

	return x;
}



void	Shaper::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	typedef fstb::DataAlign <true> DA;

	assert (DA::check_ptr (dst_ptr));
	assert (DA::check_ptr (src_ptr));
	assert (nbr_spl > 0);

	switch (_type)
	{
	case Type_ASINH:
		distort_block_shaper (_sbag._s_diode, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ASYM1:
		dsp::shape::WsAsym1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG1:
		distort_block_shaper (_sbag._s_prog1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG2:
		distort_block_shaper (_sbag._s_prog2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PROG3:
		dsp::shape::WsRcp1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SUDDEN:
		dsp::shape::WsRcp2::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_HARDCLIP:
		dsp::shape::WsHardclip::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER1:
		distort_block_shaper (_sbag._s_puncher1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER2:
		distort_block_shaper (_sbag._s_puncher2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PUNCHER3:
		distort_block_shaper (_sbag._s_puncher3, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_OVERSHOOT:
		distort_block_shaper (_sbag._s_overshoot, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BITCRUSH:
		dsp::shape::WsBitcrush <std::ratio <4, 1>, true>::process_block <DA, DA> (
			dst_ptr, src_ptr, nbr_spl
		);
		break;
	case Type_LOPSIDED:
		distort_block_shaper (_sbag._s_lopsided, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_PORRIDGE:
		_porridge_limiter.process_block (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SMARTE1:
		distort_block_shaper (_sbag._s_smarte1, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_SMARTE2:
		distort_block_shaper (_sbag._s_smarte2, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ATTRACT:
		_attractor.process_block (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_RANDWALK:
		distort_block_shaper (_sbag._s_atan, dst_ptr, src_ptr, nbr_spl);
		_random_walk.process_block (dst_ptr, dst_ptr, nbr_spl);
		break;
	case Type_SQRT:
		dsp::shape::WsSqrt::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BELT:
		dsp::shape::WsBelt::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BADMOOD:
		dsp::shape::WsBadmood::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BOUNCE:
		_bounce.process_block (dst_ptr, src_ptr, nbr_spl);
		distort_block_shaper (_sbag._s_tanh, dst_ptr, dst_ptr, nbr_spl);
		break;
	case Type_LIGHT1:
		dsp::shape::WsLight1::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_LIGHT2:
		dsp::shape::WsLight2::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_LIGHT3:
		dsp::shape::WsLight3::process_block <DA, DA> (dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_TANH:
		distort_block_shaper (_sbag._s_tanh, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_TANHLIN:
		distort_block_shaper (_sbag._s_tanhlin, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_BREAK:
		distort_block_shaper (_sbag._s_break, dst_ptr, src_ptr, nbr_spl);
		break;
	case Type_ASYM2:
		distort_block_shaper (_sbag._s_asym2, dst_ptr, src_ptr, nbr_spl);
		break;

	default:
		assert (false);
		dsp::mix::Align::copy_1_1 (dst_ptr, src_ptr, nbr_spl);
		break;
	}
}



void	Shaper::clear_buffers ()
{
	_porridge_limiter.clear_buffers ();
	_attractor.clear_buffers ();
	_random_walk.clear_buffers ();
	_bounce.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename S>
void	Shaper::distort_block_shaper (S &shaper, float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = shaper (src_ptr [pos]);
	}
}



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
