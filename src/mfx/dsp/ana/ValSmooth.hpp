/*****************************************************************************

        ValSmooth.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ana_ValSmooth_CODEHEADER_INCLUDED)
#define mfx_dsp_ana_ValSmooth_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ana
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int NV>
void	ValSmooth <T, NV>::reset (T x) noexcept
{
	_val_last = x;
	_val_smth = x;
}



template <typename T, int NV>
T	ValSmooth <T, NV>::proc_val (T x) noexcept
{
	if (_val_last == ValType (NV))
	{
		_val_smth = x;
	}
	else
	{
		const ValType  dif = x - _val_last;
		if (fabs (dif) >= _thr * x)
		{
			_val_smth = x;
		}
		else
		{
			_val_smth = _val_last + dif * _resp;
		}
	}

	_val_last = x;

	return _val_smth;
}



template <typename T, int NV>
T	ValSmooth <T, NV>::get_val () const noexcept
{
	return _val_smth;
}



template <typename T, int NV>
T	ValSmooth <T, NV>::get_raw_val () const noexcept
{
	return _val_last;
}



template <typename T, int NV>
void	ValSmooth <T, NV>::clear_buffers () noexcept
{
	_val_smth = _val_last;
}



template <typename T, int NV>
void	ValSmooth <T, NV>::set_responsiveness (T resp) noexcept
{
	assert (resp >  ValType (0));
	assert (resp <= ValType (1));

	_resp = resp;
}



template <typename T, int NV>
void	ValSmooth <T, NV>::set_threshold (T thr) noexcept
{
	assert (thr >= ValType (0));

	_thr = thr;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ana_ValSmooth_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
