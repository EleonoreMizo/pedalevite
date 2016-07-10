/*****************************************************************************

        ParamMapFdbk.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_ParamMapFdbk_CODEHEADER_INCLUDED)
#define	mfx_pi_ParamMapFdbk_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/math/fnc.h"

#include <stdexcept>

#include <cmath>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ParamMapFdbk::config (double val_min, double val_max)
{
	if (val_min != 0 || val_max != Mapper::get_ys ())
	{
		throw std::range_error ("ParamMapFdbk: invalid range");
	}
}



double	ParamMapFdbk::conv_norm_to_nat (double norm) const
{
	return (Mapper::saturate (norm));
}



double	ParamMapFdbk::conv_nat_to_norm (double nat) const
{
	return (Mapper::desaturate (nat));
}



double	ParamMapFdbk::get_nat_min () const
{
	return 0;
}



double	ParamMapFdbk::get_nat_max () const
{
	return Mapper::get_ys ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace pi
}	// namespace mfx



#endif	// mfx_pi_ParamMapFdbk_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
