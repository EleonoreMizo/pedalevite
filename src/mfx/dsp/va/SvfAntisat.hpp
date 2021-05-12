/*****************************************************************************

        SvfAntisat.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_va_SvfAntisat_CODEHEADER_INCLUDED)
#define mfx_dsp_va_SvfAntisat_CODEHEADER_INCLUDED



#define mfx_dsp_va_SvfAntisat_CURVE 0

/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_sample_freq
Description:
	Sets the sampling rate. Should be called at least once before processing
	anything
Input parameters:
	- sample_freq: sampling rate in H, > 0.
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);
	update_b ();
}



/*
==============================================================================
Name: set_freq
Description:
	Sets the cutoff frequency.
	The sampling rate should have been set before.
Input parameters:
	- f: the frequency in Hz. ]0 ; Fs/2[
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::set_freq (float f) noexcept
{
	assert (_sample_freq > 0);
	assert (f > 0);
	assert (f < _sample_freq * 0.5f);

	_g     = fstb::Approx::tan_mystran (float (fstb::PI) * f * _inv_fs);
	_g_inv = 1.f / _g;
	update_b ();
}



/*
==============================================================================
Name: set_reso
Description:
	Sets the resonance r
	r = 1 - R, R is the damping
	Q = 1 / (2 * R)
	Values above 1 should make the filter self-oscillate.
	It's possible to go even further, but the filter becomes unstable at
	high frequencies. It looks reasonably safe up to r = 1.5 on the whole
	frequency range, but sometimes there are slight glitches.
Input parameters:
	- r: the resonance
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::set_reso (float r) noexcept
{
	assert (r >= 0);
	assert (r <= 1.75f);

	_k = -r;
	update_b ();
}



/*
==============================================================================
Name: process_sample
Description:
	Filters a single input sample and outputs the three bands.
	High magnitude samples may add some glitches. It's better to clip the input
	samples to +/-2 to be on the safe side.
Input parameters:
	- x: input sample
Output parameters:
	- lp: low-pass filter output
	- bp: band-pass filter output
	- hp: high-pass filter output
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::process_sample (float &lp, float &bp, float &hp, float x) noexcept
{
	assert (_sample_freq > 0);

	// Solve bp in: tanh (a + b * bp) - bp = 0
	const float    a = (_s1 + _g * (x  - _s2)) * _g_inv;
	EqBp &         fnc = _solver.use_fnc ();
	fnc.set_a (a);
	bp = _solver.slove ();
	fnc.set_estimation (bp);
	const float    at_bp = AS::eval_inv (bp);

	lp =     _g * bp + _s2;
	hp = x - _k * bp - at_bp - lp;

	// Updates states
	_s1 = bp + _g * hp;
	_s2 = lp + _g * bp;
}



/*
==============================================================================
Name: clear_buffers
Description:
	Resets the filter state.
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::clear_buffers () noexcept
{
	_s1 = 0;
	_s2 = 0;
	_solver.use_fnc ().set_estimation (0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class AS>
void	SvfAntisat <AS>::update_b () noexcept
{
	_b = (-1 - _g * (_g + _k )) * _g_inv;
	_solver.use_fnc ().set_b (_b);
}



template <class AS>
void	SvfAntisat <AS>::EqBp::set_a (float a) noexcept
{
	_a = a;
}



template <class AS>
void	SvfAntisat <AS>::EqBp::set_b (float b) noexcept
{
	assert (b != 0);

	_b = b;
	_one_over_b = 1.f / b;
	if (b != 1)
	{
		_one_over_1_m_b = 1.f / (1 - b);
	}
}



template <class AS>
void	SvfAntisat <AS>::EqBp::set_estimation (float y) noexcept
{
	_y = y;
}



/*
==============================================================================
Name: estimate
Description:
	Resturns a quick estimation of x for f(a+b*x) - x = 0. This value is fed as
	initial guess to the Newton-Raphson algorithm.
	Uses a piece-wise linear approximation for tanh (or other similar
	functions):
	           { -1 for x <= -1
	tanh (x) ~ { x  for -1 < x < 1
	           { 1  for x >= 1
	       { -1 - x for a + b * x < -1
	f(x) ~ { a + x * (b - 1) inbetween
	       { 1 - x for a + b * x > 1

	So we try to find x0 with the "inbetween" approx for f.
	Then we check if a + b * x0 (the tanh argument) is in [-1 ; 1].
	If yes, we keep the value. Otherwise, we retry with the constant part,
	depending on the a + b * x0 sign.
Returns: the estimation
Throws: Nothing
==============================================================================
*/

template <class AS>
float	SvfAntisat <AS>::EqBp::estimate () noexcept
{
	if (_b == 0)
	{
		return 0;
	}
	// x0 = _a / (1 - _b)
	// t0 = _a + _b * x0
	else if (_a < _b - 1)      // t0 < -1
	{
		return (-1 - _a) * _one_over_b;
	}
	else if (_a > 1 - _b) // t0 > 1
	{
		return ( 1 - _a) * _one_over_b;
	}
	else if (_b != 1)
	{
		return _a * _one_over_1_m_b;
	}

	return _y;
}



/*
==============================================================================
Name: eval
Description:
	Computes the function and its derivate:
	f(x)  = tanh (a + b * x)       - x
	f'(x) = b * (1 - tanh (x) ^ 2) - 1
Input parameters:
	- x: input value
Output parameters:
	- y: f(x)
	- dy: derivate f'(x)
Throws: Nothing
==============================================================================
*/

template <class AS>
void	SvfAntisat <AS>::EqBp::eval (float &y, float &dy, float x) noexcept
{
	const float    lx = _a + _b * x;
	AS::eval (y, dy, lx);
	y  =       y - x;
	dy = _b * dy - 1;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_va_SvfAntisat_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
