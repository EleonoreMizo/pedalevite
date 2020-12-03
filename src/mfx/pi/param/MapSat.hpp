/*****************************************************************************

        MapSat.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_MapSat_CODEHEADER_INCLUDED)
#define mfx_pi_param_MapSat_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <stdexcept>

#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class NATS, class C>
void	MapSat <NATS, C>::config (double val_min, double val_max)
{
	if (val_min != 0 || val_max != Mapper::get_ys ())
	{
		throw std::range_error ("MapSat: invalid range");
	}
}



template <class NATS, class C>
double	MapSat <NATS, C>::conv_norm_to_nat (double norm) const
{
	return Mapper::saturate (norm);
}



template <class NATS, class C>
double	MapSat <NATS, C>::conv_nat_to_norm (double nat) const
{
	return Mapper::desaturate (nat);
}



template <class NATS, class C>
double	MapSat <NATS, C>::get_nat_min ()
{
	return 0;
}



template <class NATS, class C>
double	MapSat <NATS, C>::get_nat_max ()
{
	return Mapper::get_ys ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_MapSat_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

