/*****************************************************************************

        Downsampler4xSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Downsampler4xSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Downsampler4xSimd_CODEHEADER_INCLUDED



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



template <int NC42, int NC21>
void	Downsampler4xSimd <NC42, NC21>::set_coefs (const double coef_42 [NC42], const double coef_21 [NC21])
{
	_ds_42.set_coefs (coef_42);
	_ds_21.set_coefs (coef_21);
}



template <int NC42, int NC21>
float	Downsampler4xSimd <NC42, NC21>::process_sample (const float src_ptr [4])
{
	float          buf_x2 [2];

	buf_x2 [0] = _ds_42.process_sample (&src_ptr [0]);
	buf_x2 [1] = _ds_42.process_sample (&src_ptr [2]);

	return (_ds_21.process_sample (buf_x2));
}



template <int NC42, int NC21>
void	Downsampler4xSimd <NC42, NC21>::process_block (float data_ptr [], int nbr_spl)
{
	assert (nbr_spl > 0);

	_ds_42.process_block (data_ptr, data_ptr, nbr_spl * 2);
	_ds_21.process_block (data_ptr, data_ptr, nbr_spl    );
}



template <int NC42, int NC21>
void	Downsampler4xSimd <NC42, NC21>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
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



template <int NC42, int NC21>
void	Downsampler4xSimd <NC42, NC21>::clear_buffers ()
{
	_ds_42.clear_buffers ();
	_ds_21.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Downsampler4xSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
