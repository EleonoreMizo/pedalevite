/*****************************************************************************

        Upsampler8xSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Upsampler8xSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Upsampler8xSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::set_coefs (const double coef_84 [NC84], const double coef_42 [NC42], const double coef_21 [NC21]) noexcept
{
	_us_84.set_coefs (coef_84);
	_us_42.set_coefs (coef_42);
	_us_21.set_coefs (coef_21);
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_sample (float dst_ptr [8], float src) noexcept
{
	assert (dst_ptr != nullptr);

	float          buf_x4 [4];
	float          buf_x2 [2];
	_us_21.process_sample (buf_x2 [0] , buf_x2 [1] , src       );
	_us_42.process_sample (buf_x4 [0] , buf_x4 [1] , buf_x2 [0]);
	_us_42.process_sample (buf_x4 [2] , buf_x4 [3] , buf_x2 [1]);
	_us_84.process_sample (dst_ptr [0], dst_ptr [1], buf_x4 [0]);
	_us_84.process_sample (dst_ptr [2], dst_ptr [3], buf_x4 [1]);
	_us_84.process_sample (dst_ptr [4], dst_ptr [5], buf_x4 [2]);
	_us_84.process_sample (dst_ptr [6], dst_ptr [7], buf_x4 [3]);
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (&dst_ptr [pos * 8], src_ptr [pos]);
	}
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_sample_4x (float dst_ptr [4], float src) noexcept
{
	assert (dst_ptr != nullptr);

	float          buf_x2 [2];
	_us_21.process_sample (buf_x2 [0] , buf_x2 [1] , src       );
	_us_42.process_sample (dst_ptr [0], dst_ptr [1], buf_x2 [0]);
	_us_42.process_sample (dst_ptr [2], dst_ptr [3], buf_x2 [1]);
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_block_4x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample_4x (&dst_ptr [pos * 4], src_ptr [pos]);
	}
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_sample_2x (float dst_ptr [2], float src) noexcept
{
	assert (dst_ptr != nullptr);

	_us_21.process_sample (dst_ptr [0], dst_ptr [1], src);
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	_us_21.process_block (dst_ptr, src_ptr, nbr_spl);
}



template <int NC84, int NC42, int NC21>
void	Upsampler8xSimd <NC84, NC42, NC21>::clear_buffers () noexcept
{
	_us_21.clear_buffers ();
	_us_42.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Upsampler8xSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
