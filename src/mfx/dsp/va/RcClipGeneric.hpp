/*****************************************************************************

        RcClipGeneric.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_RcClipGeneric_CODEHEADER_INCLUDED)
#define mfx_dsp_va_RcClipGeneric_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <utility>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class F>
RcClipGeneric <F>::RcClipGeneric (IvFunc &&fnc)
:	_fnc (std::move (fnc))
{
	// Nothing
}



template <class F>
typename RcClipGeneric <F>::IvFunc &	RcClipGeneric <F>::use_fnc ()
{
	return _fnc;
}



template <class F>
typename const RcClipGeneric <F>::IvFunc &	RcClipGeneric <F>::use_fnc () const
{
	return _fnc;
}



template <class F>
void	RcClipGeneric <F>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	update_internal_coef_rc ();
}



template <class F>
void	RcClipGeneric <F>::set_capa (float c)
{
	assert (c > 0);

	_c = c;
	update_internal_coef_rc ();
}



template <class F>
void	RcClipGeneric <F>::set_cutoff_freq (float f)
{
	assert (f > 0);

	set_capa (1.0f / (float (2 * fstb::PI) * _r * f));
}



template <class F>
float	RcClipGeneric <F>::process_sample (float x)
{
	assert (_sample_freq > 0);

	const float    gr_x = _gr * x;
	const float    ieqc = -_v2 * _geqc - _ic;

	// Newton-Raphson iterations
	float          v2        = _v2; // Starts with the previous V2 value
	float          v2_old    = v2;
	int            nbr_it    = 0;
	do
	{
		// Check for convergence failure
		if (nbr_it >= _max_it)
		{
			v2 = _v2; // Reuses the previous value
			break;
		}

		const float    max_step = _fnc.get_max_step (v2);
		float          id;
		float          geqd;
		_fnc.eval (id, geqd, v2);

		const float    ieqd = id - geqd * v2;
		v2_old = v2;
		v2 = (gr_x - ieqc - ieqd) / (_gr_p_geqc + geqd);
		v2 = fstb::limit (v2, v2_old - max_step, v2_old + max_step);
		++ nbr_it;
	}
	while (fabs (v2 - v2_old) > _max_dif_a);

	// Updates the Ic state (integration) and saves v2
	_ic = _geqc * v2 + ieqc;
	_v2 = v2;

	return v2;
}



template <class F>
void	RcClipGeneric <F>::clear_buffers ()
{
	_ic = 0;
	_v2 = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class F>
void	RcClipGeneric <F>::update_internal_coef_rc ()
{
	_geqc      = 2 * _c * _sample_freq;
//	_gr        = 1.f / _r;
	_gr_p_geqc = _gr + _geqc;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_RcClipGeneric_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
