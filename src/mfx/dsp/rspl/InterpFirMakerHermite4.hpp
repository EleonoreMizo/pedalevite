/*****************************************************************************

        InterpFirMakerHermite4.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpFirMakerHermite4_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFirMakerHermite4_CODEHEADER_INCLUDED



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
void	InterpFirMakerHermite4 <DT>::make_interpolator (DataType fir_ptr [], float pos)
{
	assert (fir_ptr != 0);
	assert (pos >= 0);
	assert (pos <= 1);

	const DataType pos2 = pos * pos;
	fir_ptr [0] = ((-0.5 * pos + 1  ) * pos  - 0.5) * pos;
	fir_ptr [1] =  ( 1.5 * pos + 2.5) * pos2 + 1;
	fir_ptr [2] = ((-1.5 * pos + 2  ) * pos  + 0.5) * pos;
	fir_ptr [3] =  ( 0.5 * pos - 0.5) * pos2;
}



template <typename DT>
int	InterpFirMakerHermite4 <DT>::get_length ()
{
	return 4;
}



template <typename DT>
int	InterpFirMakerHermite4 <DT>::get_delay ()
{
	return 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpFirMakerHermite4_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
