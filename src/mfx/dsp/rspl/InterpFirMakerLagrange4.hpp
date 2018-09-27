/*****************************************************************************

        InterpFirMakerLagrange4.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpFirMakerLagrange4_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFirMakerLagrange4_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename DT>
void	InterpFirMakerLagrange4 <DT>::make_interpolator (DataType fir_ptr [], float pos)
{
	assert (fir_ptr != 0);
	assert (pos >= 0);
	assert (pos <= 1);

	// 3rd order Lagrange interpolator impulse: 8 mul, 3 add, 2 neg.
	const DataType dm1 = pos - 1;
	const DataType dp1 = pos + 1;
	const DataType dm2 = pos - 2;
	const DataType d_dm1   = static_cast <DataType> ((1.0 / 6) * pos * dm1);
	const DataType dp1_dm2 = static_cast <DataType> ((1.0 / 2) * dp1 * dm2);
	fir_ptr [0] = -d_dm1   * dm2;
	fir_ptr [1] =  dp1_dm2 * dm1;
	fir_ptr [2] = -dp1_dm2 * pos;
	fir_ptr [3] =  d_dm1   * dp1;
}



template <typename DT>
int	InterpFirMakerLagrange4 <DT>::get_length ()
{
	return 4;
}



template <typename DT>
int	InterpFirMakerLagrange4 <DT>::get_delay ()
{
	return 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpFirMakerLagrange4_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
