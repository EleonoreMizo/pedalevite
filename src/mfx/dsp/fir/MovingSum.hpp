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



template <typename DT, typename ST>
void	MovingSum <DT, ST>::reserve (int len)
{
	assert (len > 0);

	_buf.reserve (len);
}



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
typename MovingSum <DT, ST>::SumType	MovingSum <DT, ST>::process_sample (DataType x) noexcept
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
void	MovingSum <DT, ST>::process_block (SumType dst_ptr [], const DataType src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);

	process_block_internal (
		dst_ptr, src_ptr, nbr_spl,
		[] (SumType *d_ptr, SumType val) { *d_ptr = val; }
	);
}



template <typename DT, typename ST>
void	MovingSum <DT, ST>::analyse_block (const DataType src_ptr [], int nbr_spl) noexcept
{
	process_block_internal (
		nullptr, src_ptr, nbr_spl,
		[] (SumType *d_ptr, SumType val) { fstb::unused (d_ptr, val); }
	);
}



template <typename DT, typename ST>
typename MovingSum <DT, ST>::SumType	MovingSum <DT, ST>::get_val () const noexcept
{
	return _sum_u + _sum_d;
}



template <typename DT, typename ST>
void	MovingSum <DT, ST>::clear_buffers () noexcept
{
	std::fill (_buf.begin (), _buf.end (), DataType (0));
	_pos_rw = 0;
	_sum_u  = 0;
	_sum_d  = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// dst_ptr may be null if the results are not stored.
// void store_result (SumType &dst, SumType val)
template <typename DT, typename ST>
template <typename F>
void	MovingSum <DT, ST>::process_block_internal (SumType dst_ptr [], const DataType src_ptr [], int nbr_spl, F store_result) noexcept
{
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		int            work_len = nbr_spl - pos;
		work_len = std::min (work_len, _win_len - _pos_rw);

		for (int k = 0; k < work_len; ++k)
		{
			const auto     ofs = pos + k;
			const auto     x   = src_ptr [ofs];
			_sum_u += x;
			_sum_d -= _buf [_pos_rw];
			_buf [_pos_rw] = x;
			store_result (dst_ptr + ofs, _sum_u + _sum_d);
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



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_fir_MovingSum_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
