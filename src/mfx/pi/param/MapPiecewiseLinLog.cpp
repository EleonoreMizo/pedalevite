/*****************************************************************************

        MapPiecewiseLinLog.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MapPiecewiseLinLog::MapPiecewiseLinLog ()
:	_seg_list ()
,	_val_min (0)
,	_val_max (0)
{
	// Nothing
}



void	MapPiecewiseLinLog::config (double val_min, double val_max)
{
	assert (val_min < val_max);

	_val_min = val_min;
	_val_max = val_max;
}



double	MapPiecewiseLinLog::conv_norm_to_nat (double norm) const
{
	assert (is_ok ());

	int				index = 0;
	while (norm > _seg_list [index]._val_u_nrm)
	{
		++ index;
	}
	assert (index < int (_seg_list.size ()));

	const Segment& seg = _seg_list [index];
	double         nat = norm * seg._a_no2na + seg._b_no2na;
	if (seg._log_flag)
	{
		nat = exp (nat);
		if (seg._neg_flag)
		{
			nat = -nat;
		}
	}

	return nat;
}



double	MapPiecewiseLinLog::conv_nat_to_norm (double nat) const
{
	assert (is_ok ());

	int				index = 0;
	if (nat < _seg_list [index]._val_l_nat)
	{
		while (nat < _seg_list [index]._val_u_nat)
		{
			++ index;
		}
	}
	else
	{
		while (nat > _seg_list [index]._val_u_nat)
		{
			++ index;
		}
	}
	assert (index < int (_seg_list.size ()));

	const Segment& seg = _seg_list [index];
	if (seg._log_flag)
	{
		nat = log (fabs (nat));
	}
	const double	norm = (nat - seg._b_no2na) * seg._a_na2no;

	return norm;
}



void	MapPiecewiseLinLog::set_first_value (double nat)
{
	assert (nat == _val_min || nat == _val_max);

	_seg_list.clear ();
	_seg_list.push_back ({
		0, nat,
		0, nat,
		0,
		0,
		0,
		false,
		false
	});
}



void	MapPiecewiseLinLog::add_segment (double norm, double nat, bool log_flag)
{
	assert (nat >= _val_min);
	assert (nat <= _val_max);
	assert (! _seg_list.empty ());

	double         l_nrm = _seg_list.back ()._val_u_nrm;
	double         l_nat = _seg_list.back ()._val_u_nat;
	if (l_nrm == 0)
	{
		l_nat = _seg_list.back ()._val_l_nat;
		_seg_list.clear ();
		assert (l_nat == _val_min || l_nat == _val_max);
	}

	assert (norm > l_nrm);
	assert (nat != l_nat);
	assert (norm < 1 || (nat == _val_max || nat == _val_min));

	// Log values should be non-zero and have the same sign
	assert (! log_flag || nat * l_nat > 0);

	const bool     neg_flag = (nat < 0);
	const double   v0       = (log_flag) ? log (fabs (l_nat)) : l_nat;
	const double   v1       = (log_flag) ? log (fabs (  nat)) :   nat;
	const double   dna      = v1 - v0;
	const double   dno      = norm - l_nrm;
	const double   a_no2na  = dna / dno;
	const double   a_na2no  = dno / dna;
	const double   b_no2na  = v0 - l_nrm * a_no2na;

	_seg_list.push_back ({
		l_nrm, l_nat,
		norm, nat,
		a_no2na,
		b_no2na,
		a_na2no,
		log_flag,
		neg_flag
	});

	// Ensures monotonic function on the whole range
	assert (  (_seg_list.front ()._val_u_nat - _seg_list.front ()._val_l_nat)
	        * (_seg_list.back ()._val_u_nat  - _seg_list.back ()._val_l_nat) > 0);
}



bool	MapPiecewiseLinLog::is_ok () const
{
	return (! _seg_list.empty () && _seg_list.back ()._val_u_nrm == 1.0);
}



// Only works with positive values for now.
// If the first point is 0, the first segment is a ramp. The rest of the curve
// is log and nbr_seg is related to the logbase unit, like a pseudo-log curve.
void	MapPiecewiseLinLog::gen_log (int nbr_seg, double logbase)
{
	assert (nbr_seg >= 2);
	assert (logbase > 1 || _val_min != 0);
	assert (_val_min >= 0);

	double         vmin    = _val_min;
	int            seg_beg = 0;
	if (_val_min == 0)
	{
		vmin = _val_max * fstb::ipow (logbase, -nbr_seg);
		set_first_value (_val_min);
		add_segment (1.0 / nbr_seg, vmin * logbase, false);
		seg_beg = 1;
	}
	else
	{
		set_first_value (_val_min);
	}

	const double   lmi = log (vmin);
	const double   lma = log (_val_max);
	for (int seg = seg_beg; seg < nbr_seg - 1; ++seg)
	{
		const double   nrm = double (seg + 1) / nbr_seg;
		const double   nat = exp (lmi + nrm * (lma - lmi));
		add_segment (nrm, nat, false);
	}

	add_segment (1, _val_max, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
