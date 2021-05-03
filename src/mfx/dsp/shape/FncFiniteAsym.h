/*****************************************************************************

        FncFiniteAsym.h
        Author: Laurent de Soras, 2016

Template parameters:

- BL: Lower bound for the function definition

- BU: Upper bound

- GF: functor of the interpolated function. Requires:
	GF::GF () noexcept;
	double GF::operator () (double x) const noexcept;

- RES: internal resolution, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_FncFiniteAsym_HEADER_INCLUDED)
#define mfx_dsp_shape_FncFiniteAsym_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ToolsSimd.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace shape
{



template <int BL, int BU, class GF, int RES = 1>
class FncFiniteAsym
{

	static_assert (BL < BU, "");
	static_assert (RES >= 1, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               FncFiniteAsym () noexcept;
	               FncFiniteAsym (const FncFiniteAsym <BL, BU, GF, RES> &other) = default;
	               FncFiniteAsym (FncFiniteAsym <BL, BU, GF, RES> &&other) = default;
	virtual        ~FncFiniteAsym () = default;
	FncFiniteAsym <BL, BU, GF, RES> &
	               operator = (const FncFiniteAsym <BL, BU, GF, RES> &other) = default;
	FncFiniteAsym <BL, BU, GF, RES> &
	               operator = (FncFiniteAsym <BL, BU, GF, RES> &&other) = default;

	inline float   operator () (float x) const noexcept;
	inline fstb::ToolsSimd::VectF32
	               operator () (fstb::ToolsSimd::VectF32 x) const noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _table_size = (BU - BL) * RES;
	static const int  _order      = 3;
	static const int  _prec_frac  = 1024 * RES;

	typedef std::array <float, _order + 1> Curve;
	typedef std::array <Curve, _table_size + 1> CurveTable;	// +1 to secure rounding errors

	static void    init_coefs () noexcept;

	static CurveTable
	               _coef_arr;
	static float   _val_min;
	static float   _val_max;
	static bool    _init_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FncFiniteAsym <BL, BU, GF, RES> &other) const = delete;
	bool           operator != (const FncFiniteAsym <BL, BU, GF, RES> &other) const = delete;

}; // class FncFiniteAsym



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/FncFiniteAsym.hpp"



#endif   // mfx_dsp_shape_FncFiniteAsym_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
