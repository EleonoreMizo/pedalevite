/*****************************************************************************

        DelayLineFracFir4Base.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelayLineFracFir4Base_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineFracFir4Base_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>

#include <algorithm>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class DT, typename AL>
DelayLineFracFir4Base <DT, AL>::DelayLineFracFir4Base (const AllocatorType &al)
:	_buf (al)
,	_delay_time_max (1)
,	_delay_time_int (1)
,	_write_pos (0)
{
	_buf.set_extra_len (FIR_LEN - 1);
	_buf.set_unroll_pre (0);
	_buf.set_unroll_post (UNROLL_POST);
	_buf.set_sample_freq (1);

	set_max_time (FIR_LEN);
}



// Data is not preserved
template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::set_max_time (long max_delay_time)
{
	assert (max_delay_time > 0);

	_delay_time_max = max_delay_time;
	_buf.set_max_delay_time (float (max_delay_time));
	clear_buffers ();
}



template <class DT, typename AL>
long	DelayLineFracFir4Base <DT, AL>::get_max_time () const noexcept
{
	return (_delay_time_max);
}



template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::set_time (long delay_time) noexcept
{
	assert (delay_time > 0);
	assert (delay_time <= _delay_time_max);

	_delay_time_int = delay_time;
}



template <class DT, typename AL>
long	DelayLineFracFir4Base <DT, AL>::get_time () const noexcept
{
	return (_delay_time_int);
}



template <class DT, typename AL>
long	DelayLineFracFir4Base <DT, AL>::get_line_length () const noexcept
{
	return (_buf.get_len ());
}



template <class DT, typename AL>
long	DelayLineFracFir4Base <DT, AL>::get_write_pos () const noexcept
{
	return (_write_pos);
}



template <class DT, typename AL>
typename DelayLineFracFir4Base <DT, AL>::DataType	DelayLineFracFir4Base <DT, AL>::read_sample (const DataType fir_data []) const noexcept
{
	const DataType * const  buf_ptr = _buf.get_buffer ();
	const long     buf_mask = _buf.get_mask ();
	const long     read_pos = _write_pos - _delay_time_int;

	const DataType x0 = buf_ptr [ read_pos      & buf_mask];
	const DataType x1 = buf_ptr [(read_pos + 1) & buf_mask];
	const DataType x2 = buf_ptr [(read_pos + 2) & buf_mask];
	const DataType x3 = buf_ptr [(read_pos + 3) & buf_mask];

	const DataType read_val =   (  x0 * fir_data [0]
	                             + x1 * fir_data [1])
	                          + (  x2 * fir_data [2]
	                             + x3 * fir_data [3]);

	return (read_val);
}



template <class DT, typename AL>
typename DelayLineFracFir4Base <DT, AL>::DataType	DelayLineFracFir4Base <DT, AL>::read_sample_unroll (const DataType fir_data []) const noexcept
{
	const DataType * const  buf_ptr = _buf.get_buffer ();
	const long     buf_mask = _buf.get_mask ();
	const long     read_pos = (_write_pos - _delay_time_int) & buf_mask;

	const DataType x0 = buf_ptr [ read_pos     ];
	const DataType x1 = buf_ptr [(read_pos + 1)];
	const DataType x2 = buf_ptr [(read_pos + 2)];
	const DataType x3 = buf_ptr [(read_pos + 3)];

	const DataType read_val =   (  x0 * fir_data [0]
	                             + x1 * fir_data [1])
	                          + (  x2 * fir_data [2]
	                             + x3 * fir_data [3]);

	return (read_val);
}



template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::write_sample (DataType sample) noexcept
{
	DataType * const  buf_ptr = _buf.get_buffer ();

	buf_ptr [_write_pos] = sample;
}



template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::write_sample_unroll (DataType sample) noexcept
{
	assert (_write_pos <= UNROLL_POST);

	DataType * const  buf_ptr = _buf.get_buffer ();
	const long        buf_len = _buf.get_len ();

	buf_ptr [_write_pos] = sample;
	buf_ptr [_write_pos + buf_len] = sample;
}



template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::step_one_sample () noexcept
{
	const long     buf_mask = _buf.get_mask ();
	_write_pos = (_write_pos + 1) & buf_mask;
}



template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::clear_buffers () noexcept
{
	_buf.clear_buffers ();
	_buf.update_unroll_post ();
	_write_pos = 0;
}



/*
==============================================================================
Name: clear_buffers_fast
Description:
	Erase only the first buffer part for the current period (not the whole
	buffer)
Throws: Nothing
==============================================================================
*/

template <class DT, typename AL>
void	DelayLineFracFir4Base <DT, AL>::clear_buffers_fast () noexcept
{
	const long     buf_len   = _buf.get_len ();
	const long     erase_len = std::min (
		static_cast <long> (_delay_time_int + 4),
		buf_len
	);
	DataType * const	buf_ptr = _buf.get_buffer ();
	std::fill (buf_ptr, buf_ptr + erase_len, DataType (0));

	_write_pos = _delay_time_int;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelayLineFracFir4Base_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
