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
RcClipGeneric <F>::RcClipGeneric (IvFunc &&fnc) noexcept
:	_fnc (std::move (fnc))
{
	// Nothing
}



template <class F>
typename RcClipGeneric <F>::IvFunc &	RcClipGeneric <F>::use_fnc () noexcept
{
	return _fnc;
}



template <class F>
const typename RcClipGeneric <F>::IvFunc &	RcClipGeneric <F>::use_fnc () const noexcept
{
	return _fnc;
}



template <class F>
void	RcClipGeneric <F>::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	update_internal_coef_rc ();
}



template <class F>
void	RcClipGeneric <F>::set_capa (float c) noexcept
{
	assert (c > 0);

	_c = c;
	update_internal_coef_rc ();
}



template <class F>
void	RcClipGeneric <F>::set_cutoff_freq (float f) noexcept
{
	assert (f > 0);

	set_capa (1.0f / (float (2 * fstb::PI) * _r * f));
}



template <class F>
float	RcClipGeneric <F>::process_sample (float x) noexcept
{
	assert (_sample_freq > 0);

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	int            nbr_f0 = 0;
	int            nbr_f1 = 0;
#endif // mfx_dsp_va_RcClipGeneric_STAT

	const float    gr_x = _gr * x;

	// Initial estimate
	float          v2 = _v2; // Starts with the previous V2 value
	if (std::signbit (_iceq - gr_x) == std::signbit (v2))
	{
		// The sign of the offset iceq - gR * x should be opposite to the
		// sign of the solution. If they are of the same sign, it is likely
		// that the estimate will have to travel long before being meaningful,
		// especially when coming from a zone with a huge derivative.
		// So we fix v2 roughly by changing its sign.
		// The additional 0.5 ratio proves to be a further improvement for both
		// average convergence and the baddest cases.
		v2 *= -0.5f;
	}

	// Newton-Raphson iterations
	float          v2_old = v2;
	int            nbr_it = 0;
	do
	{
		// Checks for convergence failure
		if (nbr_it >= _max_it)
		{
			if (fabsf (_v2) < fabsf (v2))
			{
				// We are really lost. Reuses the previous output value.
				v2 = _v2;
			}
			break;
		}

		const float    max_step = _fnc.get_max_step (v2);
		float          id;
		float          geqd;
		_fnc.eval (id, geqd, v2);
#if defined (mfx_dsp_va_RcClipGeneric_STAT)
		++ nbr_f0;
		++ nbr_f1;
#endif // mfx_dsp_va_RcClipGeneric_STAT

		const float    ieqd = id - geqd * v2;
		v2_old = v2;
		v2 = (gr_x - _iceq - ieqd) / (_gr_p_geqc + geqd);
		v2 = fstb::limit (v2, v2_old - max_step, v2_old + max_step);
		++ nbr_it;
	}
	while (fabsf (v2 - v2_old) > _max_dif_a);

	// Updates the Ic state (integration) and saves v2
	_v2   = v2;
	_iceq = -2 * v2 * _geqc - _iceq;

#if defined (mfx_dsp_va_RcClipGeneric_STAT)
	++ _st._hist_it [nbr_it];
	++ _st._hist_f0 [nbr_f0];
	++ _st._hist_f1 [nbr_f1];
	++ _st._nbr_spl_proc;
#endif // mfx_dsp_va_RcClipGeneric_STAT

	return v2;
}



template <class F>
void	RcClipGeneric <F>::clear_buffers () noexcept
{
	_iceq = 0;
	_v2   = 0;
}



#if defined (mfx_dsp_va_RcClipGeneric_STAT)

template <class F>
void	RcClipGeneric <F>::reset_stat () noexcept
{
	_st._hist_it.fill (0);
	_st._hist_f0.fill (0);
	_st._hist_f1.fill (0);
	_st._nbr_spl_proc = 0;
}



template <class F>
void	RcClipGeneric <F>::get_stats (Stat &stat) const noexcept
{
	stat = _st;
}

#endif // mfx_dsp_va_RcClipGeneric_STAT



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class F>
void	RcClipGeneric <F>::update_internal_coef_rc () noexcept
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
