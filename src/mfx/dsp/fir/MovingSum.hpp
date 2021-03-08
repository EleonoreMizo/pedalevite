/*****************************************************************************

        MovingSum.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_fir_MovingSum_CODEHEADER_INCLUDED)
#define mfx_dsp_fir_MovingSum_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>
#include <utility>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Filter memory may be lost if the size is actually changed
template <typename DT, typename ST>
void	MovingSum <DT, ST>::set_win_len (int len)
{
	assert (len > 0);

	if (len != _win_len)
	{
		_buf.resize (len);
		_win_len = len;
		clear_buffers ();
	}
}



template <typename DT, typename ST>
typename MovingSum <DT, ST>::SumType	MovingSum <DT, ST>::process_sample (DataType x)
{
	_sum_u += x;
	_sum_d -= _buf [_pos_rw];
	_buf [_pos_rw] = x;

	++ _pos_rw;
	if (_pos_rw >= _win_len)
	{
		_sum_d  = _sum_u;
		_sum_u  = 0;
		_pos_rw = 0;
	}

	return _sum_u + _sum_d;
}



template <typename DT, typename ST>
void	MovingSum <DT, ST>::process_block (SumType dst_ptr [], const DataType src_ptr [], int nbr_spl)
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		int            work_len = nbr_spl - pos;
		work_len = std::min (work_len, _win_len - _pos_rw);

		for (int k = 0; k < work_len; ++k)
		{
			const DataType x = src_ptr [pos + k];
			_sum_u += x;
			_sum_d -= _buf [_pos_rw];
			_buf [_pos_rw] = x;
			dst_ptr [pos + k] = _sum_u + _sum_d;
			++ _pos_rw;
		}

		if (_pos_rw >= _win_len)
		{
			_sum_d  = _sum_u;
			_sum_u  = 0;
			_pos_rw = 0;
		}

		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <typename DT, typename ST>
void	MovingSum <DT, ST>::clear_buffers ()
{
	std::fill (_buf.begin (), _buf.end (), DataType (0));
	_pos_rw = 0;
	_sum_u  = 0;
	_sum_d  = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_fir_MovingSum_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
