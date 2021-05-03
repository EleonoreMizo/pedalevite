/*****************************************************************************

        Downsampler8xSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Downsampler8xSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Downsampler8xSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>
#include <array>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::set_coefs (const double coef_84 [NC84], const double coef_42 [NC42], const double coef_21 [NC21]) noexcept
{
	_ds_84.set_coefs (coef_84);
	_ds_42.set_coefs (coef_42);
	_ds_21.set_coefs (coef_21);
}



template <int NC84, int NC42, int NC21>
float	Downsampler8xSimd <NC84, NC42, NC21>::process_sample (const float src_ptr [8]) noexcept
{
	float          buf_x4 [4];
	float          buf_x2 [2];

	_ds_42.process_block (buf_x4, src_ptr, 4);

	buf_x2 [0] = _ds_42.process_sample (&buf_x2 [0]);
	buf_x2 [1] = _ds_42.process_sample (&buf_x2 [2]);

	return (_ds_21.process_sample (buf_x2));
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block (float data_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_ds_84.process_block (data_ptr, data_ptr, nbr_spl * 4);
	_ds_42.process_block (data_ptr, data_ptr, nbr_spl * 2);
	_ds_21.process_block (data_ptr, data_ptr, nbr_spl    );
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	std::array <float, _buf_len * 4> buf;

	int            work_pos = 0;
	do
	{
		const int      max_len  = _buf_len;
		const int      work_len = std::min (nbr_spl - work_pos, max_len);
		float * const  buf_ptr  = &buf [0];

		_ds_84.process_block (buf_ptr, &src_ptr [work_pos * 8], work_len * 4);
		_ds_42.process_block (buf_ptr, buf_ptr, work_len * 2);
		_ds_21.process_block (&dst_ptr [work_pos], buf_ptr, work_len);

		work_pos += work_len;
	}
	while (work_pos < nbr_spl);
}



template <int NC84, int NC42, int NC21>
float	Downsampler8xSimd <NC84, NC42, NC21>::process_sample_4x (const float src_ptr [4]) noexcept
{
	float          buf_x2 [2];

	buf_x2 [0] = _ds_42.process_sample (&src_ptr [0]);
	buf_x2 [1] = _ds_42.process_sample (&src_ptr [2]);

	return (_ds_21.process_sample (buf_x2));
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block_4x (float data_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_ds_42.process_block (data_ptr, data_ptr, nbr_spl * 2);
	_ds_21.process_block (data_ptr, data_ptr, nbr_spl    );
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block_4x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	std::array <float, _buf_len * 2> buf;

	int            work_pos = 0;
	do
	{
		const int      max_len  = _buf_len;
		const int      work_len = std::min (nbr_spl - work_pos, max_len);
		float * const  buf_ptr  = &buf [0];

		_ds_42.process_block (buf_ptr, &src_ptr [work_pos * 4], work_len * 2);
		_ds_21.process_block (&dst_ptr [work_pos], buf_ptr, work_len);

		work_pos += work_len;
	}
	while (work_pos < nbr_spl);
}



template <int NC84, int NC42, int NC21>
float	Downsampler8xSimd <NC84, NC42, NC21>::process_sample_2x (const float src_ptr [2]) noexcept
{
	return _ds_21.process_sample (src_ptr);
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block_2x (float data_ptr [], int nbr_spl) noexcept
{
	_ds_21.process_block (data_ptr, data_ptr, nbr_spl);
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::process_block_2x (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	_ds_21.process_block (dst_ptr, src_ptr, nbr_spl);
}



template <int NC84, int NC42, int NC21>
void	Downsampler8xSimd <NC84, NC42, NC21>::clear_buffers () noexcept
{
	_ds_84.clear_buffers ();
	_ds_42.clear_buffers ();
	_ds_21.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Downsampler8xSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
