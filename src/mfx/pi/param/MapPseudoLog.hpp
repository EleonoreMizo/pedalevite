/*****************************************************************************

        MapPseudoLog.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_MapPseudoLog_CODEHEADER_INCLUDED)
#define mfx_pi_param_MapPseudoLog_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	MapPseudoLog::config (double val_min, double val_max)
{
	assert (val_min == 0);
	assert (val_min < val_max);

	_a  = val_max;
	_ai = 1.0 / _a;
}



double	MapPseudoLog::conv_norm_to_nat (double norm) const
{
	const double   nat = fstb::pseudo_exp (norm, _c) * _a;

	return nat;
}



double	MapPseudoLog::conv_nat_to_norm (double nat) const
{
	const double   nrm = fstb::pseudo_log (nat * _ai, _c);

	return nrm;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_MapPseudoLog_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
