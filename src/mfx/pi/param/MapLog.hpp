/*****************************************************************************

        MapLog.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_MapLog_CODEHEADER_INCLUDED)
#define mfx_pi_param_MapLog_CODEHEADER_INCLUDED



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



void	MapLog::config (double val_min, double val_max)
{
	assert (val_min > 0);
	assert (val_min < val_max);

	const double   val_min_log = log (val_min);
	const double   val_max_log = log (val_max);
	_a = val_max_log - val_min_log;
	_b = val_min_log;
}



double	MapLog::conv_norm_to_nat (double norm) const
{
	const double   nat_log = norm * _a + _b;
	double         nat = exp (nat_log);

	return (nat);
}



double	MapLog::conv_nat_to_norm (double nat) const
{
	const double   nat_log = log (nat);
	double         norm = (nat_log - _b) / _a;

	return (norm);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_MapLog_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
