/*****************************************************************************

        MapSatBipolar.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_MapSatBipolar_CODEHEADER_INCLUDED)
#define mfx_pi_param_MapSatBipolar_CODEHEADER_INCLUDED



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
void	MapSatBipolar <NATS, C>::config (double val_min, double val_max)
{
	if (val_min != -val_max || val_max != Mapper::get_ys ())
	{
		throw std::range_error ("ParamMapFdbk: invalid range");
	}
}



template <class NATS, class C>
double	MapSatBipolar <NATS, C>::conv_norm_to_nat (double norm) const
{
	const double	norm_bip = norm - 0.5;
	const double	norm_mon = fabs (norm_bip);
	const double	nat_mon  = Mapper::saturate (norm_mon);
	const double	nat      = std::copysign (nat_mon, norm_bip);

	return (nat);
}



template <class NATS, class C>
double	MapSatBipolar <NATS, C>::conv_nat_to_norm (double nat) const
{
	const double	nat_mon  = fabs (nat);
	const double	norm_mon = Mapper::desaturate (nat_mon);
	const double	norm_bip = std::copysign (norm_mon, nat);
	const double	norm     = norm_bip + 0.5;

	return (norm);
}



template <class NATS, class C>
double	MapSatBipolar <NATS, C>::get_nat_min ()
{
	return -get_nat_max ();
}



template <class NATS, class C>
double	MapSatBipolar <NATS, C>::get_nat_max ()
{
	return Mapper::get_ys ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_MapSatBipolar_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
