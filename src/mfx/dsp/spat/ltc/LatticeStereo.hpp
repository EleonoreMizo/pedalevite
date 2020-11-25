/*****************************************************************************

        LatticeStereo.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_ltc_LatticeStereo_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_ltc_LatticeStereo_CODEHEADER_INCLUDED



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
D &	LatticeStereo <D, N>::use_delay (int chn, int idx)
{
	assert (chn >= 0);
	assert (chn < _nbr_chn);
	assert (idx >= 0);
	assert (idx < _nbr_proc);

	return _ltc_arr [chn].use_delay (idx);
}



template <class D, int N>
void	LatticeStereo <D, N>::set_coef (int chn, int idx, DataType c)
{
	assert (chn >= 0);
	assert (chn < _nbr_chn);
	assert (idx >= 0);
	assert (idx < _nbr_proc);
	assert (c >= DataType (-1.f));
	assert (c <= DataType (+1.f));

	_ltc_arr [chn].set_coef (idx, c);
}



template <class D, int N>
void	LatticeStereo <D, N>::set_cross_feedback (DataType cf)
{
	assert (cf >= 0);
	assert (cf <= 1);

	_cross_fdbk = cf;
}



// first  = left
// second = right
template <class D, int N>
std::pair <typename LatticeStereo <D, N>::DataType, typename LatticeStereo <D, N>::DataType>	LatticeStereo <D, N>::process_sample (DataType xl, DataType xr)
{
	for (int idx = 0; idx < _nbr_proc; idx += 2)
	{
		const DataType    fl = _ltc_arr [0].use_delay_output (idx);
		const DataType    fr = _ltc_arr [1].use_delay_output (idx);
#if 0
		// Positive cross-feedback
		const DataType    cs = _cross_fdbk * (fl - fr);
		const DataType    yl = fl - cs;
		const DataType    yr = fr + cs;
#else
		// Negative cross-feedback
		const DataType    cs = _cross_fdbk * (fl + fr);
		const DataType    yl = fl - cs;
		const DataType    yr = fr - cs;
#endif
		_ltc_arr [0].use_delay_output (idx) = yl;
		_ltc_arr [1].use_delay_output (idx) = yr;
	}

	return std::make_pair (
		_ltc_arr [0].process_sample (xl),
		_ltc_arr [1].process_sample (xr)
	);
}



template <class D, int N>
void	LatticeStereo <D, N>::clear_buffers ()
{
	for (auto &lattice : _ltc_arr)
	{
		lattice.clear_buffers ();
	}
	_fdbk_arr.fill (DataType (0.f));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_ltc_LatticeStereo_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
