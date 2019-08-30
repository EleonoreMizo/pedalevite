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
void	OscSinCosStable <T>::set_phase (DataType angle_rad)
{
	_pos_cos = DataType (cos (angle_rad));
	_pos_sin = DataType (sin (angle_rad));
}



template <class T>
void	OscSinCosStable <T>::set_step (DataType angle_rad)
{
	compute_step (_alpha, _beta, angle_rad);
}



template <class T>
void	OscSinCosStable <T>::step ()
{
	step (_alpha, _beta);
}



template <class T>
void	OscSinCosStable <T>::step (DataType angle_rad)
{
	DataType       alpha;
	DataType       beta;

	compute_step (alpha, beta, angle_rad);
	step (alpha, beta);
}



template <class T>
typename OscSinCosStable <T>::DataType	OscSinCosStable <T>::get_cos () const
{
	return _pos_cos;
}



template <class T>
typename OscSinCosStable <T>::DataType	OscSinCosStable <T>::get_sin () const
{
	return _pos_sin;
}



template <class T>
void	OscSinCosStable <T>::clear_buffers ()
{
	_pos_cos = DataType (1);
	_pos_sin = DataType (0);
}



template <class T>
void	OscSinCosStable <T>::correct ()
{
	const double   norm_sq = double (_pos_cos * _pos_cos + _pos_sin * _pos_sin);
	const DataType mult    = DataType (1.0 / sqrt (norm_sq));

	_pos_cos *= mult;
	_pos_sin *= mult;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
void	OscSinCosStable <T>::step (DataType alpha, DataType beta)
{
	const DataType old_cos = _pos_cos;
	const DataType old_sin = _pos_sin;

	_pos_cos = old_cos - (alpha * old_cos + beta * old_sin);
	_pos_sin = old_sin - (alpha * old_sin - beta * old_cos);
}



template <class T>
void	OscSinCosStable <T>::compute_step (DataType &alpha, DataType &beta, DataType angle_rad)
{
	const double	s = sin (angle_rad * 0.5f);
	alpha = DataType (s * s * 2);
	beta  = DataType (sin (angle_rad));
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSinCosStable_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
