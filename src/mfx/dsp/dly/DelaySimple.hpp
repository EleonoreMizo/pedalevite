/*****************************************************************************

        DelaySimple.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	DelaySimple <T>::setup (int max_dly, int max_block_len)
{
	assert (max_dly >= 0);
	assert (max_block_len > 0);

	_max_dly       = max_dly;
	_max_block_len = max_block_len;

	update_buf ();
}



template <typename T>
int	DelaySimple <T>::get_max_delay () const noexcept
{
	assert (_max_dly > 0);

	return _max_dly;
}



template <typename T>
void	DelaySimple <T>::set_delay (int d) noexcept
{
	assert (d >= 0);
	assert (d <= _max_dly);

	_dly = d;
}



template <typename T>
T	DelaySimple <T>::read_at (int d) const noexcept
{
	assert (d >= 0);
	assert (d <= _max_dly);

	return _buf [(_pos_w - d) & _mask];
}



template <typename T>
T	DelaySimple <T>::process_sample (T x) noexcept
{
	write_sample (x);
	const T        y = _buf [delay (_pos_w)];
	step (1);

	return y;
}



template <typename T>
void	DelaySimple <T>::write_sample (T x) noexcept
{
	_buf [_pos_w] = x;
}



template <typename T>
void	DelaySimple <T>::read_block_at (T dst_ptr [], int d, int nbr_spl) const noexcept
{
	assert (d >= 0);
	assert (d <= _max_dly);
	assert (nbr_spl > 0);
	assert (nbr_spl <= d + 1);

	int            pos_r = (_pos_w - d) & _mask;
	const int      room  = _len - pos_r;
	const int      len_1 = std::min (nbr_spl, room);
	const int      len_2 = nbr_spl - len_1;
	fstb::copy_no_overlap (dst_ptr, &_buf [pos_r], len_1);
	if (len_2 > 0)
	{
		fstb::copy_no_overlap (dst_ptr + len_1, _buf.data (), len_2);
	}
}



// Can work in-place
template <typename T>
void	DelaySimple <T>::process_block (T dst_ptr [], const T src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_len);

	int            pos     = 0;
	T * const      buf_ptr = _buf.data ();
	RingBufVectorizer rbv (_len);

	for (rbv.start (nbr_spl, _pos_w, delay (_pos_w))
	;	rbv.end ()
	;	rbv.next ())
	{
		const int      work_len = rbv.get_seg_len ();
		const int      pos_w    = rbv.get_curs_pos (0);
		const int      pos_r    = rbv.get_curs_pos (1);

		fstb::copy_no_overlap (buf_ptr + pos_w, src_ptr + pos, work_len);
		fstb::copy_no_overlap (dst_ptr + pos, buf_ptr + pos_r, work_len);

		pos += work_len;
	}

	_pos_w = rbv.get_curs_pos (0);
}



template <typename T>
void	DelaySimple <T>::write_block (const T src_ptr [], int nbr_spl) noexcept
{
	write_block_internal (src_ptr, nbr_spl);
}



template <typename T>
void	DelaySimple <T>::push_block (const T src_ptr [], int nbr_spl) noexcept
{
	_pos_w = write_block_internal (src_ptr, nbr_spl);
}



template <typename T>
void	DelaySimple <T>::step (int nbr_spl)
{
	assert (nbr_spl >= 0);
	assert (nbr_spl <= _max_block_len);

	_pos_w = (_pos_w + nbr_spl) & _mask;
}



template <typename T>
void	DelaySimple <T>::clear_buffers () noexcept
{
	std::fill (_buf.data (), _buf.data () + _buf.size (), DataType (0.f));
	_pos_w = 0;
}



template <typename T>
void	DelaySimple <T>::clear_buffers_quick () noexcept
{
	if (_dly > 0)
	{
		std::fill (_buf.data (), _buf.data () + _dly, DataType (0.f));
	}
	_pos_w = _dly;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	DelaySimple <T>::update_buf ()
{
	const int      len_min = _max_block_len + _max_dly;
	_len    = 1 << fstb::get_next_pow_2 (len_min);
	_mask   = _len - 1;
	_pos_w &= _mask;
	_buf.resize (_len);
}



template <typename T>
int	DelaySimple <T>::delay (int pos) const noexcept
{
	return (pos - _dly) & _mask;
}



template <typename T>
int	DelaySimple <T>::write_block_internal (const T src_ptr [], int nbr_spl) noexcept
{
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);
	assert (nbr_spl <= _max_block_len);

	int            pos     = 0;
	T * const      buf_ptr = _buf.data ();
	RingBufVectorizer rbv (_len);

	for (rbv.start (nbr_spl, _pos_w)
	;	rbv.end ()
	;	rbv.next ())
	{
		const int      work_len = rbv.get_seg_len ();
		const int      pos_w    = rbv.get_curs_pos (0);

		fstb::copy_no_overlap (buf_ptr + pos_w, src_ptr + pos, work_len);

		pos += work_len;
	}

	return rbv.get_curs_pos (0);
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
