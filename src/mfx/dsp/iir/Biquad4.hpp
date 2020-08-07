/*****************************************************************************

        Biquad4.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Biquad4_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>

#include <cmath>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Biquad4::process_block_serial_latency (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	process_block_serial_immediate (dst_ptr, src_ptr, nbr_spl);
}



float	Biquad4::process_sample_serial_latency (float x_s)
{
	return process_sample_serial_immediate (x_s);
}



float	Biquad4::process_sample_serial_immediate (float x_s)
{
	for (int cnt = 0; cnt < _nbr_units; ++cnt)
	{
		x_s = _biq_arr [cnt].process_sample (x_s);
	}

	return x_s;
}



void	Biquad4::process_block_2x2_latency (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	process_block_2x2_immediate (dst_ptr, src_ptr, nbr_spl);
}



// |a1| < a2 + 1 && |a2| < 1
bool	Biquad4::check_stability (float a1, float a2)
{
	const float    margin = 5e-6f;

	return (fabs (a1) < a2 + (1 + margin) && fabs (a2) < (1 + margin));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Biquad4_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
