/*****************************************************************************

        DelayLineData.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelayLineData_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineData_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, typename AL>
DelayLineData <T, AL>::DelayLineData (const AllocatorType &al)
:	_buf (al)
,	_buf_ptr (nullptr)
,	_sample_freq (0)
,	_max_time (0)
,	_unroll_pre (0)
,	_unroll_post (0)
,	_extra_len (0)
,	_buf_len (0)
,	_buf_mask (0)
{
   // Nothing
}



template <typename T, typename AL>
DelayLineData <T, AL>::DelayLineData (const DelayLineData &other)
:	_buf (other._buf)
,	_buf_ptr (nullptr)
,	_sample_freq (other._sample_freq)
,	_max_time (other._max_time)
,	_unroll_pre (other._unroll_pre)
,	_unroll_post (other._unroll_post)
,	_extra_len (other._extra_len)
,	_buf_len (other._buf_len)
,	_buf_mask (other._buf_mask)
{
	if (_buf.size () > size_t { _unroll_pre })
	{
		_buf_ptr = &_buf [_unroll_pre];
	}
}



template <typename T, typename AL>
DelayLineData <T, AL> &	DelayLineData <T, AL>::operator = (const DelayLineData &other)
{
	if (this != &other)
	{
		_buf         = other._buf;
		_buf_ptr     = nullptr;
		_sample_freq = other._sample_freq;
		_max_time    = other._max_time;
		_unroll_pre  = other._unroll_pre;
		_unroll_post = other._unroll_post;
		_extra_len   = other._extra_len;
		_buf_len     = other._buf_len;
		_buf_mask    = other._buf_mask;
		if (_buf.size () > size_t { _unroll_pre })
		{
			_buf_ptr = &_buf [_unroll_pre];
		}
	}
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::set_extra_len (int nbr_spl)
{
	assert (nbr_spl >= 0);
	_extra_len = nbr_spl;
	invalidate_buf ();
}



template <typename T, typename AL>
int	DelayLineData <T, AL>::get_extra_len () const
{
	return (_extra_len);
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::set_unroll_pre (int nbr_spl)
{
	assert (nbr_spl >= 0);
	_unroll_pre = nbr_spl;
	invalidate_buf ();
}



template <typename T, typename AL>
int	DelayLineData <T, AL>::get_unroll_pre () const
{
	return (_unroll_pre);
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::set_unroll_post (int nbr_spl)
{
	assert (nbr_spl >= 0);
	_unroll_post = nbr_spl;
	invalidate_buf ();
}



template <typename T, typename AL>
int	DelayLineData <T, AL>::get_unroll_post () const
{
	return (_unroll_post);
}



// Throws std::bad_alloc
template <typename T, typename AL>
void	DelayLineData <T, AL>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);
	_sample_freq = sample_freq;
	update_buffer_size ();
}



// Throws std::bad_alloc
template <typename T, typename AL>
void	DelayLineData <T, AL>::set_max_delay_time (double max_time)
{
	assert (max_time > 0);
	_max_time = max_time;
	update_buffer_size ();
}



template <typename T, typename AL>
double	DelayLineData <T, AL>::get_max_delay_time () const
{
	return (_max_time);
}



// Throws std::bad_alloc
template <typename T, typename AL>
void	DelayLineData <T, AL>::update_buffer_size ()
{
	if (_max_time > 0 && _sample_freq > 0)
	{
		const int   max_delay_len  = fstb::ceil_int (_max_time * _sample_freq);
		const int   min_buf_size   = max_delay_len + _extra_len;
		const int   new_buf_size   = 1L << fstb::get_next_pow_2 (min_buf_size);
		const int   new_total_size = new_buf_size + _unroll_pre + _unroll_post;
		if (new_total_size != int (_buf.size ()))
		{
			invalidate_buf ();
			_buf.resize (new_total_size);
		}
		_buf_ptr  = &_buf [_unroll_pre];
		_buf_len  = new_buf_size;
		_buf_mask = _buf_len - 1;
	}
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::update_unroll ()
{
	assert (_buf_ptr != nullptr);
	update_unroll_pre ();
	update_unroll_post ();
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::update_unroll_pre ()
{
	assert (_buf_ptr != nullptr);
	assert (_unroll_pre <= _buf_len);

	for (int pos = -_unroll_pre; pos < 0; ++pos)
	{
		_buf_ptr [pos] = _buf_ptr [_buf_len + pos];
	}
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::update_unroll_post ()
{
	assert (_buf_ptr != nullptr);

	for (int pos = 0; pos < _unroll_post; ++pos)
	{
		_buf_ptr [_buf_len + pos] = _buf_ptr [pos];
	}
}



template <typename T, typename AL>
int	DelayLineData <T, AL>::get_len () const
{
	assert (_buf_ptr != nullptr);

	return _buf_len;
}



template <typename T, typename AL>
int	DelayLineData <T, AL>::get_mask () const
{
	return _buf_mask;
}



template <typename T, typename AL>
typename DelayLineData <T, AL>::ValueType *	DelayLineData <T, AL>::get_buffer ()
{
	assert (_buf_ptr != nullptr);

	return _buf_ptr;
}



template <typename T, typename AL>
const typename DelayLineData <T, AL>::ValueType *	DelayLineData <T, AL>::get_buffer () const
{
	assert (_buf_ptr != nullptr);

	return _buf_ptr;
}



template <typename T, typename AL>
void	DelayLineData <T, AL>::clear_buffers ()
{
	assert (_buf_ptr != nullptr);

	const size_t   len = _buf.size ();
	for (size_t pos = 0; pos < len; ++pos)
	{
		_buf [pos] = ValueType (int32_t (0));
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, typename AL>
void	DelayLineData <T, AL>::invalidate_buf ()
{
	_buf_ptr  = nullptr;
	_buf_len  = 0;
	_buf_mask = 0;
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelayLineData_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
