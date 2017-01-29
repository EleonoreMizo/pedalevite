/*****************************************************************************

        BandParam.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/DesignEq2p.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/pi/peq/BandParam.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace peq
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BandParam::set_gain (float gain)
{
	assert (gain > 0);

	_gain = gain;
}



float	BandParam::get_gain () const
{
	return _gain;
}



void	BandParam::set_freq (float freq)
{
	assert (freq > 0);

	_freq = freq;
}



float	BandParam::get_freq () const
{
	return _freq;
}



void	BandParam::set_q (float q)
{
	assert (q > 0);

	_q = q;
}



float	BandParam::get_q () const
{
	return _q;
}



void	BandParam::set_type (PEqType type)
{
	assert (type >= 0);
	assert (type < PEqType_NBR_ELT);

	_type = type;
}



PEqType	BandParam::get_type () const
{
	return _type;
}



void	BandParam::set_bypass (bool bypass_flag)
{
	_bypass_flag = bypass_flag;
}



bool	BandParam::is_bypass () const
{
	return _bypass_flag;
}



void	BandParam::create_filter (float bz [3], float az [3], float fs, float inv_fs) const
{
	assert (fs > 0);
	assert (inv_fs > 0);
	assert (fabs (fs * inv_fs - 1) < 1e-4);

	float          bs [3];
	float          as [3];
	bool           z_flag = false;

	switch (_type)
	{
	case	PEqType_PEAK:
#if 1	// This design requires more calculations
		dsp::iir::DesignEq2p::make_nyq_peak (
			bz,
			az,
			_q,
			_gain,
			_freq,
			fs
		);
		z_flag = true;
#else
		dsp::iir::DesignEq2p::make_mid_peak (bs, as, _q, _gain);
#endif
		break;
	case	PEqType_SHELF_LO:
		dsp::iir::DesignEq2p::make_mid_shelf_lo (bs, as, _q, _gain);
		break;
	case	PEqType_HP:
		dsp::iir::DesignEq2p::make_hi_pass (bs, as, _q);
		break;
	case	PEqType_SHELF_HI:
		dsp::iir::DesignEq2p::make_mid_shelf_hi (bs, as, _q, _gain);
		break;
	case	PEqType_LP:
		dsp::iir::DesignEq2p::make_low_pass (bs, as, _q);
		break;
	default:
		assert (false);
		break;
	}

	if (! z_flag)
	{
		const float    k =
			dsp::iir::TransSZBilin::compute_k_approx (_freq * inv_fs);
		dsp::iir::TransSZBilin::map_s_to_z_approx (bz, az, bs, as, k);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace peq
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
