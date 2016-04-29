/*****************************************************************************

        MapSq.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_MapSq_HEADER_INCLUDED)
#define mfx_pi_param_MapSq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace param
{



template <bool INVFLAG>
class MapSq
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MapSq ()  = default;
	virtual        ~MapSq () = default;

	inline void    config (double val_min, double val_max);
	inline double  conv_norm_to_nat (double norm) const;
	inline double  conv_nat_to_norm (double nat) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _a  = 1;
	double         _b  = 0;
	double         _ai = 1;    // 1 / _a



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MapSq (const MapSq &other)             = delete;
	MapSq &        operator = (const MapSq &other)        = delete;
	bool           operator == (const MapSq &other) const = delete;
	bool           operator != (const MapSq &other) const = delete;

}; // class MapSq



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/MapSq.hpp"



#endif   // mfx_pi_param_MapSq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
