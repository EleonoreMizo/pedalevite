/*****************************************************************************

        WavetablePolyphaseData.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_WavetablePolyphaseData_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_WavetablePolyphaseData_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int PLEN, int NPL2, typename DT>
void	WavetablePolyphaseData <PLEN, NPL2, DT>::set_sample (int phase, int pos, DataType val) noexcept
{
	assert (phase >= 0);
	assert (phase < NBR_PHASES);
	assert (pos >= 0);
	assert (pos < PHASE_LEN);

	_data_arr [phase] [pos] = val;
}



template <int PLEN, int NPL2, typename DT>
void	WavetablePolyphaseData <PLEN, NPL2, DT>::set_sample (int phase_pos, DataType val) noexcept
{
	assert (phase_pos >= 0);
	assert (phase_pos < NBR_PHASES * PHASE_LEN);

	const int      phase_mask = (1L << NBR_PHASES_LOG2) - 1;
	_data_arr [phase_pos & phase_mask] [phase_pos >> NBR_PHASES_LOG2] = val;
}



template <int PLEN, int NPL2, typename DT>
typename WavetablePolyphaseData <PLEN, NPL2, DT>::DataType	WavetablePolyphaseData <PLEN, NPL2, DT>::get_sample (int phase, int pos) const noexcept
{
	assert (phase >= 0);
	assert (phase < NBR_PHASES);
	assert (pos >= 0);
	assert (pos < PHASE_LEN);

	return (_data_arr [phase] [pos]);
}



template <int PLEN, int NPL2, typename DT>
typename WavetablePolyphaseData <PLEN, NPL2, DT>::DataType	WavetablePolyphaseData <PLEN, NPL2, DT>::get_sample (int phase_pos) const noexcept
{
	assert (phase_pos >= 0);
	assert (phase_pos < NBR_PHASES * PHASE_LEN);

	const int      phase_mask = (1L << NBR_PHASES_LOG2) - 1;
	return (_data_arr [phase_pos & phase_mask] [phase_pos >> NBR_PHASES_LOG2]);
}



template <int PLEN, int NPL2, typename DT>
typename WavetablePolyphaseData <PLEN, NPL2, DT>::DataType *	WavetablePolyphaseData <PLEN, NPL2, DT>::use_table (int phase) noexcept
{
	assert (phase >= 0);
	assert (phase < NBR_PHASES);

	return (&_data_arr [phase] [0]);
}



template <int PLEN, int NPL2, typename DT>
const typename WavetablePolyphaseData <PLEN, NPL2, DT>::DataType *	WavetablePolyphaseData <PLEN, NPL2, DT>::use_table (int phase) const noexcept
{
	assert (phase >= 0);
	assert (phase < NBR_PHASES);

	return (&_data_arr [phase] [0]);
}



template <int PLEN, int NPL2, typename DT>
void	WavetablePolyphaseData <PLEN, NPL2, DT>::clear_table (int phase) noexcept
{
	std::fill (_data_arr [phase], _data_arr [phase] + PHASE_LEN, DataType (0));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_WavetablePolyphaseData_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
