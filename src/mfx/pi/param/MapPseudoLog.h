/*****************************************************************************

        MapPseudoLog.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_MapPseudoLog_HEADER_INCLUDED)
#define mfx_pi_param_MapPseudoLog_HEADER_INCLUDED

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



class MapPseudoLog
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline void    config (double val_min, double val_max);
	inline void    set_curvature (double c);
	inline double  conv_norm_to_nat (double norm) const;
	inline double  conv_nat_to_norm (double nat) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	double         _a  = 1;    // nat = fnc (norm) * _a
	double         _ai = 1;    // 1 / _a
	double         _c  = 8;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MapPseudoLog &other) const = delete;
	bool           operator != (const MapPseudoLog &other) const = delete;

}; // class MapPseudoLog



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/MapPseudoLog.hpp"



#endif   // mfx_pi_param_MapPseudoLog_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
