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



template <int BL, int BU, class GF, int RES>
FncFiniteAsym <BL, BU, GF, RES>::FncFiniteAsym ()
{
	if (! _init_flag)
	{
		init_coefs ();
	}
}



template <int BL, int BU, class GF, int RES>
float	FncFiniteAsym <BL, BU, GF, RES>::operator () (float x) const
{
	assert (_init_flag);

	const int      pos = fstb::floor_int ((x - BL) * RES);
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



template <int BL, int BU, class GF, int RES>
fstb::ToolsSimd::VectF32	FncFiniteAsym <BL, BU, GF, RES>::operator () (fstb::ToolsSimd::VectF32 x) const
{
	assert (_init_flag);

	const auto     bl   = fstb::ToolsSimd::set1_f32 (float (BL));
	const auto     bu   = fstb::ToolsSimd::set1_f32 (float (BU));
	const auto     zero = fstb::ToolsSimd::set_f32_zero ();
	const auto     ts   = fstb::ToolsSimd::set1_f32 (float (_table_size));
	x = fstb::ToolsSimd::max_f32 (x, bl);
	x = fstb::ToolsSimd::min_f32 (x, bu);
	auto           xo   = x - bl;
	if (RES > 1)
	{
		xo *= fstb::ToolsSimd::set1_f32 (float (RES));
	}
	auto           pos  = fstb::ToolsSimd::floor_f32_to_s32 (xo);

	/*** To do: check if a matrix transposition is faster ***/
	auto           c0   = zero;
	auto           c1   = zero;
	auto           c2   = zero;
	auto           c3   = zero;
	for (int k = 0; k < 4; ++k)
	{
		const int      p     = fstb::ToolsSimd::Shift <0>::extract (pos);
		const Curve &  curve = _coef_arr [p];
		fstb::ToolsSimd::Shift <0>::insert (c0, curve [0]);
		fstb::ToolsSimd::Shift <0>::insert (c1, curve [1]);
		fstb::ToolsSimd::Shift <0>::insert (c2, curve [2]);
		fstb::ToolsSimd::Shift <0>::insert (c3, curve [3]);
		c0  = fstb::ToolsSimd::Shift <1>::rotate (c0);
		c1  = fstb::ToolsSimd::Shift <1>::rotate (c1);
		c2  = fstb::ToolsSimd::Shift <1>::rotate (c2);
		c3  = fstb::ToolsSimd::Shift <1>::rotate (c3);
		pos = fstb::ToolsSimd::Shift <1>::rotate (pos);
	}
	const auto     y    = c0 + x * (c1 + x * (c2 + x * c3));

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int BL, int BU, class GF, int RES>
void	FncFiniteAsym <BL, BU, GF, RES>::init_coefs ()
{
	std::array <double, _table_size + 2>     y;
	std::array <double, _table_size + 2> slope;
	GF             fnc;

	const float    delta = 0.5f / _prec_frac;
	for (int pos = 0; pos < _table_size + 1; ++pos)
	{
		const double   x  = double (pos) / RES + BL;

		const double   y0 = double (fnc (x        ));
		const double   ym = double (fnc (x - delta));
		const double   yp = double (fnc (x + delta));

		y [pos]     = y0;
		slope [pos] = (yp - ym) * _prec_frac;
	}

	y [_table_size + 1]     = y [_table_size];
	slope [_table_size + 1] = 0;

	_val_min = float (y [          0]);
	_val_max = float (y [_table_size]);

	const double   RES2 = RES * RES;
	const double   RES3 = RES * RES2;
	for (int pos = 0; pos < _table_size + 1; ++pos)
	{
		Curve &        curve = _coef_arr [pos];

		const double   x = double (pos) / RES + BL;
		const double   p = slope [pos    ];
		const double   q = slope [pos + 1];
		const double   k = y [pos + 1] - y [pos];

		// Coefficients in [0 ; 1/RES]
		const double   A = ( q +     p) * RES2 - 2 * k * RES3;
		const double   B = (-q - 2 * p) * RES  + 3 * k * RES2;
		const double   C = p;
		const double   D = y [pos];

		// Shift to [x ; x+1/RES]
		const double   a =         A;
		const double   b =   -3 *  A * x + B;
		const double   c = (  3 *  A * x - B * 2) * x + C;
		const double   d = ((     -A * x + B    ) * x - C) * x + D;

		curve [0] = float (d);
		curve [1] = float (c);
		curve [2] = float (b);
		curve [3] = float (a);
	}

	_init_flag = true;
}



template <int BL, int BU, class GF, int RES>
typename FncFiniteAsym <BL, BU, GF, RES>::CurveTable	FncFiniteAsym <BL, BU, GF, RES>::_coef_arr;
template <int BL, int BU, class GF, int RES>
float	FncFiniteAsym <BL, BU, GF, RES>::_val_min = 0;
template <int BL, int BU, class GF, int RES>
float	FncFiniteAsym <BL, BU, GF, RES>::_val_max = 0;
template <int BL, int BU, class GF, int RES>
bool	FncFiniteAsym <BL, BU, GF, RES>::_init_flag = false;



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_shape_FncFiniteAsym_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
