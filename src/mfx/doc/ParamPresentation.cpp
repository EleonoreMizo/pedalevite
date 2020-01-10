/*****************************************************************************

        ParamPresentation.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/doc/ParamPresentation.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	ParamPresentation::operator == (const ParamPresentation &other) const
{
	return (
		   _disp_mode == other._disp_mode
		&& _ref_beats == other._ref_beats
	);
}



bool	ParamPresentation::operator != (const ParamPresentation &other) const
{
	return ! (*this == other);
}



bool	ParamPresentation::is_similar (const ParamPresentation &other) const
{
	const float    tol = 1e-5f;

	bool           same_flag = (_disp_mode == other._disp_mode);
	same_flag &= (_ref_beats * other._ref_beats >= 0);
	if (same_flag && _ref_beats >= 0)
	{
		same_flag = fstb::is_eq_rel (_ref_beats, other._ref_beats, tol);
	}

	return same_flag;
}



void	ParamPresentation::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_disp_mode);
	ser.write (_ref_beats);

	ser.end_list ();
}



void	ParamPresentation::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_disp_mode);
	ser.read (_ref_beats);

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator < (const ParamPresentation &lhs, const ParamPresentation &rhs)
{
	if (lhs._disp_mode < rhs._disp_mode) { return true; }
	else if (lhs._disp_mode == rhs._disp_mode)
	{
		return (lhs._ref_beats < rhs._ref_beats);
	}

	return false;
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
