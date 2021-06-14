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
#include "mfx/doc/CtrlLink.h"
#include "mfx/CtrlUnit.h"
#include "mfx/ProcessingContext.h"

#include <cassert>
#include <cmath>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CtrlUnit::CtrlUnit (const doc::CtrlLink &other, bool abs_flag)
:	_source (other._source)
,	_step (other._step)
,	_val (0)
,	_curve (other._curve)
,	_u2b_flag (other._u2b_flag)
,	_abs_flag (abs_flag)
,	_base (other._base)
,	_amp (other._amp)
,	_notch_list (other._notch_list)
{
	set_clip (
		other._clip_flag,
		other._clip_src_beg, other._clip_src_end,
		other._clip_dst_beg, other._clip_dst_end
	);
}



void	CtrlUnit::set_clip (bool enable_flag, float src_beg, float src_end, float dst_beg, float dst_end)
{
	assert (! enable_flag || src_beg < src_end);
	assert (! enable_flag || dst_beg < dst_end);

	_clip_flag     = enable_flag;
	_clip_src_beg  = src_beg;
	_clip_src_end  = src_end;
	_clip_dst_beg  = dst_beg;
	_clip_dst_end  = dst_end;

	if (_clip_flag)
	{
		_clip_mul =
			  (_clip_dst_end - _clip_dst_beg)
			/ (_clip_src_end - _clip_src_beg);
		_clip_add = _clip_dst_beg - _clip_src_beg * _clip_mul;
	}
	else
	{
		_clip_mul = 1;
		_clip_add = 0;
	}
}



bool	CtrlUnit::is_src_clipped () const
{
	return _clip_flag;
}



float	CtrlUnit::get_src_beg () const
{
	return _clip_src_beg;
}



float	CtrlUnit::get_src_end () const
{
	return _clip_src_end;
}



float	CtrlUnit::get_dst_beg () const
{
	return _clip_dst_beg;
}



float	CtrlUnit::get_dst_end () const
{
	return _clip_dst_end;
}



void	CtrlUnit::update_internal_val (float val_nrm)
{
	assert (_abs_flag);

	float               mod_val = val_nrm - _base;
	if (_amp != 0)
	{
		mod_val /= _amp;
	}
	mod_val = fstb::limit (mod_val, 0.f, 1.f);

	// Inverse curve
	mod_val = ControlCurve_apply_curve (mod_val, _curve, true);

	// Inverse clip mapping
	if (_clip_flag)
	{
		mod_val -= _clip_add;
		if (_clip_mul != 0)
		{
			mod_val /= _clip_mul;
		}
	}

	_val = mod_val;
}



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
// [0; 1] range, so clipping happens only on the final value.
float	CtrlUnit::evaluate (float param_val) const
{
	float          mod_val = _val;
	if (_u2b_flag)
	{
		mod_val = mod_val * 2 - 1;
	}

	if (_clip_flag)
	{
		mod_val = fstb::limit (mod_val, _clip_src_beg, _clip_src_end);
		mod_val *= _clip_mul;
		mod_val += _clip_add;
	}

	mod_val  = ControlCurve_apply_curve (mod_val, _curve, false);
	mod_val *= _amp;

	if (_abs_flag)
	{
		param_val = _base + mod_val;
	}
	else
	{
		param_val += mod_val;
	}

	if (! _notch_list.empty ())
	{
		auto           it_l = _notch_list.lower_bound (param_val);
		if (it_l == _notch_list.end ())
		{
			param_val = *_notch_list.rbegin ();
		}
		else if (it_l == _notch_list.begin ())
		{
			param_val = *it_l;
		}
		else
		{
			assert (_notch_list.size () >= 2);
			const float    v1 = *it_l;
			-- it_l;
			const float    v0 = *it_l;
			const float    d1 = fabsf (param_val - v1);
			const float    d0 = fabsf (param_val - v0);
			param_val = (d0 < d1) ? v0 : v1;
		}
	}

	return param_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
