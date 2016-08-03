/*****************************************************************************

        MapRatio.cpp
        Author: Laurent de Soras, 2016

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

#include "fstb/def.h"
#include "mfx/pi/param/MapRatio.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MapRatio::config (double val_min, double val_max)
{
	assert (val_min > 0);
	assert (val_min <= 1);
	assert (val_min < val_max);
	assert (val_max >= 1);

	_min_l2 = log2 (val_min);
	_max_l2 = log2 (val_max);
}



double	MapRatio::conv_norm_to_nat (double norm) const
{
	const double	norm_bip   = norm - 0.5;
	const bool		neg_flag   = (norm_bip < 0);
	const double	norm_mon   = fabs (norm_bip);
	const double	shaped_mon = Mapper::desaturate (norm_mon);
	const double	shaped     = ((neg_flag) ? _max_l2 : _min_l2) * shaped_mon;
	const double	nat        = exp (shaped * fstb::LN2);

	return nat;
}



double	MapRatio::conv_nat_to_norm (double nat) const
{
	const double	logged     = log (nat);
	const bool		neg_flag   = (logged < 0);
	const double	mul        = 1.0 / (((neg_flag) ? _min_l2 : _max_l2) * fstb::LN2);
	const double	shaped_mon = logged * mul;
	const double	norm_mon   = Mapper::saturate (shaped_mon);
	const double	norm_bip   = (neg_flag) ? norm_mon : -norm_mon;
	const double	norm       = norm_bip + 0.5;

	return norm;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
