/*****************************************************************************

        CtrlUnit.cpp
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
#include "mfx/CtrlUnit.h"
#include "mfx/ProcessingContext.h"

#include <cassert>
#include <cmath>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CtrlUnit::update_abs_val (float raw_val)
{
	if (_source.is_relative ())
	{
		_val += raw_val * _step;
		_val = fstb::limit (_val, 0.f, 1.f);
	}
	else
	{
		_val = raw_val;
	}
}



// param_val is the original parameter value, or modulated with the previous
// modulations in the mod chain.
// param_val and the return value are normalized but can be out of the
// [0; 1] range, so clipping happend only on the final value.
float	CtrlUnit::evaluate (float param_val) const
{
	float          mod_val = _val;
	if (_u2b_flag)
	{
		mod_val = mod_val * 2 - 1;
	}

	mod_val  = apply_curve (mod_val, _curve);
	mod_val *= _amp;

	if (_abs_flag)
	{
		param_val = _base + mod_val;
	}
	else
	{
		param_val += mod_val;
	}

	return param_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Input is not range-restricted, it can be bipolar too.
float	CtrlUnit::apply_curve (float val, ControlCurve curve)
{
	switch (curve)
	{
	case ControlCurve_LINEAR:
		// Nothing
		break;

	case ControlCurve_SQ:
		val = fabs (val) * val;
		break;

	case ControlCurve_CB:
		val = val * val * val;
		break;

	case ControlCurve_SAT2:
		{
			const float    vx = 1 - fabs (val);
			val = std::copysign (1 - vx * vx, val);
		}
		break;

	case ControlCurve_SAT3:
		{
			const float    vx = 1 - fabs (val);
			val = std::copysign (1 - vx * vx * vx, val);
		}
		break;

	case ControlCurve_S1:
		val = std::copysign (val * val * (3 - 2 * fabs (val)), val);
		break;

	case ControlCurve_S2:
		val = apply_curve (val, ControlCurve_S1);
		val = apply_curve (val, ControlCurve_S1);
		break;

	default:
		assert (false);
		break;
	}

	return val;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
