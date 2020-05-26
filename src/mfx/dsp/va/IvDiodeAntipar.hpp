/*****************************************************************************

        IvDiodeAntipar.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_IvDiodeAntipar_CODEHEADER_INCLUDED)
#define mfx_dsp_va_IvDiodeAntipar_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	IvDiodeAntipar::set_d1_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is1 = is;
}



void	IvDiodeAntipar::set_d2_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is2 = is;
}



void	IvDiodeAntipar::set_d1_n (float n)
{
	assert (n > 0);

	_n1         = n;
	_max_step_1 = compute_max_step (_n1);
	_mv1        = +compute_mv (_n1);
}



void	IvDiodeAntipar::set_d2_n (float n)
{
	assert (n > 0);

	_n2         = n;
	_max_step_2 = compute_max_step (_n2);
	_mv2        = -compute_mv (_n2);
}



void	IvDiodeAntipar::eval (float &y, float &dy, float x) const
{
	const float    me = (x >= 0) ? _is1 : -_is2;
	const float    mv = (x >= 0) ? _mv1 :  _mv2;
	y  = me * (fstb::Approx::exp2 (x * mv * float (fstb::LOG2_E)) - 1);
	dy = mv * y;
}



float	IvDiodeAntipar::get_max_step (float x) const
{
	return (x < 0) ? _max_step_1 : _max_step_2;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	IvDiodeAntipar::compute_max_step (float n) const
{
	return _vt * n * 4;
}



float	IvDiodeAntipar::compute_mv (float n) const
{
	return 1 / (_vt * n);
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_IvDiodeAntipar_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
