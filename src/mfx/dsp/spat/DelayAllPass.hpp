/*****************************************************************************

        DelayAllPass.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_max_len (int len)
{
	_delay.set_max_len (len);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_delay_flt (float len_spl)
{
	set_delay_fix (fstb::conv_int_fast (len_spl * _nbr_phases));
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_delay_fix (int len_fixp)
{
	assert (len_fixp >= _delay_min * _nbr_phases);

	_delay.set_delay_fix (len_fixp);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::set_coef (T coef)
{
	assert (coef >= T (-1.f));
	assert (coef <= T (+1.f));

	_coef = coef;
}



/*
Returns:
first: allpass output y(n)
second: v(n) to feed to write()

Usage, equivalent to y = process_sample (x):
std::tie (y, v) = read (x);
write (v);
step ();
*/

template <typename T, int NPL2>
std::pair <T, T>	DelayAllPass <T, NPL2>::read (T x) const
{
	const T        u { _delay.read () };
	const T        v { x - u * _coef };
	const T        y { u + v * _coef };

	return std::make_pair (y, v);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::write (T v)
{
	_delay.write (v);
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::step ()
{
	_delay.step ();
}



template <typename T, int NPL2>
T	DelayAllPass <T, NPL2>::process_sample (T x)
{
	T              y { _delay.read () };
	x -= y * _coef;
	_delay.write (x);
	y += x * _coef;
	_delay.step ();

	return y;
}



template <typename T, int NPL2>
void	DelayAllPass <T, NPL2>::clear_buffers ()
{
	_delay.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_DelayAllPass_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
