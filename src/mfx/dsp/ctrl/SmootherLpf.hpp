/*****************************************************************************

        SmootherLpf.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_SmootherLpf_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_SmootherLpf_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	SmootherLpf <T>::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
}



template <typename T>
void	SmootherLpf <T>::set_time (float t) noexcept
{
	assert (_sample_freq > 0);
	assert (t >= 0);

	// Approximates c = 1 - exp (-1 / (fs * t))
	// Not very accurate when time < 2 samples, but still consistent.
	const float    p = t * _sample_freq;
	const float    u = 1.f / (0.501f + p);
	const float    c = std::min (u, 1.f);

	_lpf.set_coef (c);
}



template <typename T>
void	SmootherLpf <T>::set_val (T x) noexcept
{
	_val_tgt = x;
}



template <typename T>
T	SmootherLpf <T>::process_sample () noexcept
{
	assert (_sample_freq > 0);

	return _lpf.process_sample (_val_tgt);
}



template <typename T>
T	SmootherLpf <T>::skip_block (int nbr_spl) noexcept
{
	return _lpf.constant_block (_val_tgt, nbr_spl);
}



template <typename T>
T	SmootherLpf <T>::get_val_cur () const noexcept
{
	return _lpf.use_state ();
}



template <typename T>
T	SmootherLpf <T>::get_val_tgt () const noexcept
{
	return _val_tgt;
}



template <typename T>
void	SmootherLpf <T>::clear_buffers () noexcept
{
	_lpf.use_state () = _val_tgt;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_SmootherLpf_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
