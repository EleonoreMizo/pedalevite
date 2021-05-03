/*****************************************************************************

        OscSinCosStable.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSinCosStable_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSinCosStable_CODEHEADER_INCLUDED



// 1 = Jorg Arndt
// 2 = Martin Vicanek
#define mfx_dsp_osc_OscSinCosStable_TYPE 2



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	OscSinCosStable <T>::set_phase (DataType angle_rad) noexcept
{
	_pos_cos = DataType (cos (angle_rad));
	_pos_sin = DataType (sin (angle_rad));
}



template <class T>
void	OscSinCosStable <T>::set_step (DataType angle_rad) noexcept
{
	compute_step (_alpha, _beta, angle_rad);
}



template <class T>
void	OscSinCosStable <T>::step () noexcept
{
	step (_alpha, _beta);
}



template <class T>
void	OscSinCosStable <T>::step (DataType angle_rad) noexcept
{
	DataType       alpha;
	DataType       beta;

	compute_step (alpha, beta, angle_rad);
	step (alpha, beta);
}



template <class T>
typename OscSinCosStable <T>::DataType	OscSinCosStable <T>::get_cos () const noexcept
{
	return _pos_cos;
}



template <class T>
typename OscSinCosStable <T>::DataType	OscSinCosStable <T>::get_sin () const noexcept
{
	return _pos_sin;
}



template <class T>
void	OscSinCosStable <T>::clear_buffers () noexcept
{
	_pos_cos = DataType (1);
	_pos_sin = DataType (0);
}



// Given the stability of the oscillator, this function is almost useless.
// If really needed, calling correct_fast() regularly is more than enough.
template <class T>
void	OscSinCosStable <T>::correct () noexcept
{
	const double   norm_sq = double (_pos_cos * _pos_cos + _pos_sin * _pos_sin);
	const DataType mult    = DataType (1.0 / sqrt (norm_sq));

	_pos_cos *= mult;
	_pos_sin *= mult;
}



// Uses a single-step Newton-Raphson approximation of 1 / sqrt (1 + r ^ 2)
// with 1 as initial guess for the corrective term.
template <class T>
void	OscSinCosStable <T>::correct_fast () noexcept
{
	const double   norm_sq = double (_pos_cos * _pos_cos + _pos_sin * _pos_sin);
	const DataType mult    = DataType ((3.0 - norm_sq) * 0.5);
	// 2nd order approx: mult = (3/8) * norm_sq^2 - (10/8) * norm_sq + (15/8)

	_pos_cos *= mult;
	_pos_sin *= mult;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	OscSinCosStable <T>::step (DataType alpha, DataType beta) noexcept
{
#if mfx_dsp_osc_OscSinCosStable_TYPE == 1
	const DataType old_cos = _pos_cos;
	const DataType old_sin = _pos_sin;

	_pos_cos = old_cos - (alpha * old_cos + beta * old_sin);
	_pos_sin = old_sin - (alpha * old_sin - beta * old_cos);
#else
	const DataType tmp = _pos_cos - alpha * _pos_sin;
	_pos_sin += beta * tmp;
	_pos_cos  = tmp - alpha * _pos_sin;
#endif
}



template <class T>
void	OscSinCosStable <T>::compute_step (DataType &alpha, DataType &beta, DataType angle_rad) noexcept
{
#if mfx_dsp_osc_OscSinCosStable_TYPE == 1
	const double	s = sin (angle_rad * 0.5f);
	alpha = DataType (s * s * 2);
	beta  = DataType (sin (angle_rad));
#else
	alpha = DataType (tan (angle_rad * 0.5f));
	beta  = DataType (sin (angle_rad));
//	beta  = DataType (2 * alpha / (1 + alpha * alpha));
#endif
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSinCosStable_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
