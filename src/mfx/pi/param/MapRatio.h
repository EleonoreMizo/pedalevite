/*****************************************************************************

        MapRatio.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_MapRatio_HEADER_INCLUDED)
#define mfx_pi_param_MapRatio_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/shape/MapSaturate.h"



namespace mfx
{
namespace pi
{
namespace param
{



class MapRatio
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MapRatio ()  = default;
	virtual        ~MapRatio () = default;

	void           config (double val_min, double val_max);
	double         conv_norm_to_nat (double norm) const;
	double         conv_nat_to_norm (double nat) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _min_l2 = 0;   // < 0. Negative = flat
	double         _max_l2 = 0;   // > 0. Positive = steep

	typedef dsp::shape::MapSaturate <
		double,
		std::ratio <1, 2>,
		std::ratio <1, 1>,
		std::ratio <1, 2>
	> Mapper;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MapRatio (const MapRatio &other)          = delete;
	MapRatio &     operator = (const MapRatio &other)        = delete;
	bool           operator == (const MapRatio &other) const = delete;
	bool           operator != (const MapRatio &other) const = delete;

}; // class MapRatio



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/MapRatio.hpp"



#endif   // mfx_pi_param_MapRatio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
