/*****************************************************************************

        MapS.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_MapS_CODEHEADER_INCLUDED)
#define mfx_pi_param_MapS_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool INVFLAG>
void	MapS <INVFLAG>::config (double val_min, double val_max)
{
	assert (val_min > 0);
	assert (val_min < val_max);

	_a  = val_max - val_min;
	_b  = val_min;
	_ai = 1.0 / _a;
}



template <bool INVFLAG>
double	MapS <INVFLAG>::conv_norm_to_nat (double norm) const
{
	const double   tmp = (INVFLAG) ? map_inv (norm) : map_direct (norm);

	return tmp * _a + _b;
}



template <bool INVFLAG>
double	MapS <INVFLAG>::conv_nat_to_norm (double nat) const
{
	const double   tmp = (nat - _b) * _ai;

	return (INVFLAG) ? map_direct (tmp) : map_inv (tmp);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool INVFLAG>
double	MapS <INVFLAG>::map_direct (double x)
{
	return (x < 0.5f) ? 2 * x * x       : (4 - 2 * x) * x - 1;
}



template <bool INVFLAG>
double	MapS <INVFLAG>::map_inv (double x)
{
	return (x < 0.5f) ? sqrt (x * 0.5f) : 1 - sqrt ((1 - x) * 0.5f);
}



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_MapS_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
