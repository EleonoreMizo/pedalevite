/*****************************************************************************

        Lattice.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_ltc_Lattice_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_ltc_Lattice_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace ltc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class D, int N>
D &	Lattice <D, N>::use_delay (int idx)
{
	assert (idx >= 0);
	assert (idx < _nbr_proc);

	return _delay_arr [idx];
}



template <class D, int N>
void	Lattice <D, N>::set_coef (int idx, DataType c)
{
	assert (idx >= 0);
	assert (idx < _nbr_proc);
	assert (c >= DataType (-1.f));
	assert (c <= DataType (+1.f));

	_coef_arr [idx] = c;
}



template <class D, int N>
typename Lattice <D, N>::DataType	Lattice <D, N>::process_sample (DataType x)
{
	// Feedforward values multiplied by their coefficient
	ScalarArray    tmp_arr;

	for (int k = 0; k < _nbr_proc; ++k)
	{
		const DataType coef { _coef_arr [k] };
		x -= _dly_out_arr [k] * coef;
		tmp_arr [k] = x * coef;
	}

	for (int k = _nbr_proc - 1; k >= 0; --k)
	{
		Delay &        delay = _delay_arr [k];
		const DataType y = delay.process_sample (x);
		x = _dly_out_arr [k] + tmp_arr [k];
		_dly_out_arr [k] = y;
	}

	return x;
}


template <class D, int N>
typename Lattice <D, N>::DataType &	Lattice <D, N>::use_delay_output (int idx)
{
	assert (idx >= 0);
	assert (idx < _nbr_proc);

	return _dly_out_arr [idx];
}



template <class D, int N>
void	Lattice <D, N>::clear_buffers ()
{
	for (auto &delay : _delay_arr)
	{
		delay.clear_buffers ();
	}
	_dly_out_arr.fill (DataType (0.f));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_ltc_Lattice_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
