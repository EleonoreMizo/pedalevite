/*****************************************************************************

        MapSat.h
        Author: Laurent de Soras, 2020

Template parameters:

- NATS: natural maximum value, as an std::ratio

- C: curvature, as an std::ratio. See dsp::shape::MapSaturate.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_MapSat_HEADER_INCLUDED)
#define mfx_pi_param_MapSat_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/shape/MapSaturate.h"



namespace mfx
{
namespace pi
{
namespace param
{



template <class NATS = std::ratio <1, 1>, class C = std::ratio <1, 2> >
class MapSat
{
	static_assert (NATS::num != 0, "");
	static_assert (NATS::den != 0, "");
	static_assert (std::ratio_greater <C, std::ratio <0, 1> >::value, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    config (double val_min, double val_max);
	inline double  conv_norm_to_nat (double norm) const;
	inline double  conv_nat_to_norm (double nat) const;

	static inline double 
	               get_nat_min ();
	static inline double 
	               get_nat_max ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::shape::MapSaturate <
		double, C, std::ratio <1, 1>, NATS
	> Mapper;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MapSat &other) const = delete;
	bool           operator != (const MapSat &other) const = delete;

}; // class MapSat



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/MapSat.hpp"



#endif   // mfx_pi_param_MapSat_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
