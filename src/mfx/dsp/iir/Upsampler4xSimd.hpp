/*****************************************************************************

        Upsampler4xSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Upsampler4xSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Upsampler4xSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::set_coefs (const double coef_42 [NC42], const double coef_21 [NC21])
{
	_us_42.set_coefs (coef_42);
	_us_21.set_coefs (coef_21);
}



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::process_sample (float dst_ptr [4], float src)
{
	assert (dst_ptr != nullptr);

	float          buf_x2 [2];
	_us_21.process_sample (buf_x2 [0] , buf_x2 [1] , src       );
	_us_42.process_sample (dst_ptr [0], dst_ptr [1], buf_x2 [0]);
	_us_42.process_sample (dst_ptr [2], dst_ptr [3], buf_x2 [1]);
}



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (&dst_ptr [pos * 4], src_ptr [pos]);
	}
}



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::process_sample_2x (float dst_ptr [2], float src)
{
	assert (dst_ptr != nullptr);

	_us_21.process_sample (dst_ptr [0], dst_ptr [1], src);
}



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_us_21.process_block (dst_ptr, src_ptr, nbr_spl);
}



template <int NC42, int NC21>
void	Upsampler4xSimd <NC42, NC21>::clear_buffers ()
{
	_us_21.clear_buffers ();
	_us_42.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Upsampler4xSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
