/*****************************************************************************

        ProcHann.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_wnd_ProcHann_CODEHEADER_INCLUDED)
#define mfx_dsp_wnd_ProcHann_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace wnd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: setup
Description:
	This function must be called before any other.
Input parameters:
	- length: length of the window, in samples
Throws: Nothing
==============================================================================
*/

template <typename T, bool HSFLAG>
void	ProcHann <T, HSFLAG>::setup (int length) noexcept
{
	assert (length > 0);

	const double   omega = fstb::PI / length;
	_length = length;
	_k      = T (4 * fstb::sq (sin (omega * 0.5)));
	_v_org  = T (sin (omega));

	start ();
}



/*
==============================================================================
Name: start
Description:
	Starts the frame, for sample-per-sample processing
Throws: Nothing
==============================================================================
*/

template <typename T, bool HSFLAG>
void	ProcHann <T, HSFLAG>::start () noexcept
{
	assert (_length > 0);

	_u = 0;
	_v = _v_org;
}



/*
==============================================================================
Name: process_sample
Description:
	Returns the next window sample. start() or setup () must have been called
	at the beginning of the frame.
	There is no check if the frame is already terminated, it is the caller
	responsibility to count samples.
Returns: the window data
Throws: Nothing
==============================================================================
*/

template <typename T, bool HSFLAG>
T	ProcHann <T, HSFLAG>::process_sample () noexcept
{
	assert (_length > 0);

	return iterate (_u, _v, _k);
}



/*
==============================================================================
Name: process_frame_mul
Description:
	Multiplies a frame data buffer with the window. In-place version.
Input parameters:
	- spl_ptr: pointer on a buffer of _length samples, which is multiplied
		with the window.
Throws: Nothing
==============================================================================
*/

template <typename T, bool HSFLAG>
void	ProcHann <T, HSFLAG>::process_frame_mul (T spl_ptr []) const noexcept
{
	assert (spl_ptr != nullptr);

	T              u = 0;
	auto           v = _v_org;
	const auto     k = _k;

	for (int pos = 0; pos < _length; ++pos)
	{
		const auto     u_n = iterate (u, v, k);
		spl_ptr [pos] *= u_n;
	}
}



/*
==============================================================================
Name: process_frame_mul
Description:
	Multiplies a frame data buffer with the window. Out-of-place version.
Input parameters:
	- src_ptr: input data buffer of _length samples
Output parameters:
	- dst_ptr: output data buffer receiving the frame multiplied with the
		window, distinct from src_ptr.
Throws: Nothing
==============================================================================
*/

template <typename T, bool HSFLAG>
void	ProcHann <T, HSFLAG>::process_frame_mul (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr) const noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (src_ptr != dst_ptr);

	T              u = 0;
	auto           v = _v_org;
	const auto     k = _k;

	for (int pos = 0; pos < _length; ++pos)
	{
		const auto     u_n = iterate (u, v, k);
		dst_ptr [pos] = src_ptr [pos] * u_n;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, bool HSFLAG>
T	ProcHann <T, HSFLAG>::iterate (T &u, T &v, T k)
{
	const auto     u_n = u;
	u += v;
	v -= u * k;

	if (HSFLAG)
	{
		return u_n;
	}

	return fstb::sq (u_n);
}



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_wnd_ProcHann_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
