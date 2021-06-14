/*****************************************************************************

        SolverNR.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_SolverNR_CODEHEADER_INCLUDED)
#define mfx_dsp_va_SolverNR_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <limits>
#include <utility>

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename FNC, int MAXIT, bool PVF>
SolverNR <FNC, MAXIT, PVF>::SolverNR (const FuncType &fnc, DataType prec, DataType max_dif) noexcept
:  _fnc (fnc)
,  _prec (prec)
,  _max_dif (max_dif)
,  _prev_x (DataType (0))
{
	assert (prec >= 0);
	assert (max_dif > 0);
}

template <typename FNC, int MAXIT, bool PVF>
SolverNR <FNC, MAXIT, PVF>::SolverNR (FuncType &&fnc, DataType prec, DataType max_dif) noexcept
:  _fnc (std::move (fnc))
,  _prec (prec)
,  _max_dif (max_dif)
,  _prev_x (DataType (0))
{
	assert (prec >= 0);
	assert (max_dif > 0);
}



template <typename FNC, int MAXIT, bool PVF>
typename SolverNR <FNC, MAXIT, PVF>::FuncType & SolverNR <FNC, MAXIT, PVF>::use_fnc () noexcept
{
	return _fnc;
}



template <typename FNC, int MAXIT, bool PVF>
typename SolverNR <FNC, MAXIT, PVF>::DataType SolverNR <FNC, MAXIT, PVF>::slove () noexcept
{
	DataType       x = _fnc.estimate ();
	assert (std::isfinite (x));
	int            it_cnt = 0;
	do
	{
		DataType       y;
		DataType       yd;
		_fnc.eval (y, yd, x);
		assert (std::isfinite (y));
		assert (std::isfinite (yd));
		assert (yd != 0);

 		DataType       dif_x = y / yd;
 		assert (std::isfinite (dif_x));
 		dif_x = fstb::limit (dif_x, -_max_dif, _max_dif);
 		x -= dif_x;

		if (DataType (fabs (dif_x)) <= _prec)
		{
			return x;
		}

		++ it_cnt;
	}
	while (it_cnt < _nbr_max_it);

	if (PVF)
	{
		x = _prev_x;
	}

	return x;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_SolverNR_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
