/*****************************************************************************

        FncFiniteAsym.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_shape_FncFiniteAsym_CODEHEADER_INCLUDED)
#define mfx_dsp_shape_FncFiniteAsym_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"



namespace mfx
{
namespace dsp
{
namespace shape
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int BL, int BU, class GF>
FncFiniteAsym <BL, BU, GF>::FncFiniteAsym ()
{
	if (! _init_flag)
	{
		init_coefs ();
	}
}



template <int BL, int BU, class GF>
float	FncFiniteAsym <BL, BU, GF>::operator () (float x) const
{
	assert (_init_flag);

	const int      	pos = fstb::floor_int (x - BL);
   if (pos < 0)
   {
      return (_val_min);
   }
   else if (pos >= _table_size)
   {
      return (_val_max);
   }

	const Curve &  curve = _coef_arr [pos];
	return        curve [0]
	       + x * (curve [1]
	       + x * (curve [2]
	       + x *  curve [3]));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int BL, int BU, class GF>
void	FncFiniteAsym <BL, BU, GF>::init_coefs ()
{
	std::array <double, _table_size + 1>     y;
	std::array <double, _table_size + 1> slope;
	GF             fnc;

	const float    delta = 0.5f / _prec_frac;
	for (int pos = 0; pos <= _table_size; ++pos)
	{
		const double   x  = pos + BL;

		const double   y0 = double (fnc (x        ));
		const double   ym = double (fnc (x - delta));
		const double   yp = double (fnc (x + delta));

		y [pos]     = y0;
		slope [pos] = (yp - ym) * _prec_frac;
	}

	_val_min = float (y [          0]);
	_val_max = float (y [_table_size]);

	for (int pos = 0; pos < _table_size; ++pos)
	{
		Curve &        curve = _coef_arr [pos];

		const int      x = pos + BL;
		const double   p = slope [pos    ];
		const double   q = slope [pos + 1];
		const double   k = y [pos + 1] - y [pos];

		// Coefficients in [0 ; 1]
		const double   A =  q +   p - 2*k;
		const double   B = -q - 2*p + 3*k;
		const double   C = p;
		const double   D = y [pos];

		// Shift to [x ; x+1]
		const double   a =    A;
		const double   b = -3*A*x + B;
		const double   c = (3*A*x - B*2) * x + C;
		const double   d = ((-A*x + B)   * x - C) * x + D;

		curve [0] = float (d);
		curve [1] = float (c);
		curve [2] = float (b);
		curve [3] = float (a);
	}

	_init_flag = true;
}



template <int BL, int BU, class GF>
typename FncFiniteAsym <BL, BU, GF>::CurveTable	FncFiniteAsym <BL, BU, GF>::_coef_arr;
template <int BL, int BU, class GF>
float	FncFiniteAsym <BL, BU, GF>::_val_min = 0;
template <int BL, int BU, class GF>
float	FncFiniteAsym <BL, BU, GF>::_val_max = 0;
template <int BL, int BU, class GF>
bool	FncFiniteAsym <BL, BU, GF>::_init_flag = false;



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_FncFiniteAsym_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
