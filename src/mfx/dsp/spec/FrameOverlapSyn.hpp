/*****************************************************************************

        FrameOverlapSyn.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spec_FrameOverlapSyn_CODEHEADER_INCLUDED)
#define mfx_dsp_spec_FrameOverlapSyn_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spec
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: setup
Description:
	Sets the frame parameters, namely the frame size and the hop size.
	It is possible to synchronize the position where a new frame is required.
	This 	is helpful to reduce the global analysis-synthesis latency when the
	audio callback has a known, fixed size. Therefore it is possible to make a
	frame ready for analysis at the very end of the processing block and to
	start the corresponding synthesis frame at the begin of the block.
	The internal buffer is not cleared and may contain garbage.
	Call to this function is mandatory before calling anything else.
Input parameters:
	- frame_size: length of the frame, in samples. > 0
	- hop_size: time interval between two frames, in samples. ]0 ; frame_size]
	- offset: sets the moment where a frame will be requested, in samples.
		0 will trigger a synthesis frame immediately, or by a hop_size distance.
		The offset value is stored so it is set again when clear_buffers() is
		called. [0 ; frame_size[.
Throws: depends on std::vector
==============================================================================
*/

template <typename T>
void	FrameOverlapSyn <T>::setup (int frame_size, int hop_size, int offset)
{
	assert (frame_size > 0);
	assert (hop_size > 0);
	assert (hop_size <= frame_size);
	assert (offset >= 0);
	assert (offset < frame_size);

	_frame_size = frame_size;
	_hop_size   = hop_size;
	_hop_reset  = (hop_size - (offset % hop_size)) % hop_size;
	_hop_pos    = _hop_reset;

	_buf_len    = 1 << fstb::get_next_pow_2 (frame_size + hop_size);
	_buf_msk    = _buf_len - 1;
	_buf_pos   &= _buf_msk;
	_buf.resize (_buf_len, T (0)); // Fills new samples with zeros
}



/*
==============================================================================
Name: get_len_before_next_frame
	Returns the number of processing samples before a new frame is required.
	If a frame is already required during the call, it returns the full
	distance to the next frame (hop size).
Returns: distance in sample, > 0
Throws: Nothing
==============================================================================
*/

template <typename T>
int	FrameOverlapSyn <T>::get_len_before_next_frame () const noexcept
{
	assert (_frame_size > 0);

	return _hop_size - _hop_pos;
}



/*
==============================================================================
Name: is_frame_proc_required
Description:
	Indicates if the caller is required to provide a new frame.
Returns: true if it is required now.
Throws: Nothing
==============================================================================
*/

template <typename T>
bool	FrameOverlapSyn <T>::is_frame_proc_required () const noexcept
{
	assert (_frame_size > 0);

	return (_hop_pos == 0);
}



/*
==============================================================================
Name: process_sample
Description:
	Retrieves a sample of the overlapped frames already fed to the object.
Returns: a structure containing:
	- _val: the sample value
	- _frame_flag: indicates if the caller is required to provide a new
		frame before retrieving new samples.
Throws: Nothing
==============================================================================
*/

template <typename T>
typename FrameOverlapSyn <T>::ResProcSpl	FrameOverlapSyn <T>::process_sample () noexcept
{
	assert (_frame_size > 0);

	const T        x = _buf [_buf_pos];
	_buf [_buf_pos] = T (0);

	_buf_pos = (_buf_pos + 1) & _buf_msk;
	++ _hop_pos;

	const int      hop_rem    = _hop_size - _hop_pos;
	const bool     frame_flag = (hop_rem <= 0);
	if (frame_flag)
	{
		_hop_pos = 0;
	}

	return ResProcSpl { x, frame_flag };
}



/*
==============================================================================
Name: process_block
Description:
	Retrieves a block of samples of the overlapped frames. If a new frame is
	required in the middle of the course, the retrieval is stopped and the
	number of remaining (not read) samples is returned. Client should call
	process_block() again with this value to continue processing after sending
	the frame data.
Input parameters:
	- dst_ptr: pointer on the block where the output samples will be stored.
		Should be allocated by the caller.
	- nbr_spl: number of samples to read.
Returns: a strcture containing:
	- _nbr_spl_rem: number of remaining samples. Usually 0, or something below
		nbr_spl if a frame has to be fed in between.
	- _frame_flag: indicates if a frame must be fed to the object.
Throws: Nothing
==============================================================================
*/

template <typename T>
typename FrameOverlapSyn <T>::ResProcBlock	FrameOverlapSyn <T>::process_block (T dst_ptr [], int nbr_spl) noexcept
{
	assert (_frame_size > 0);
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	const int      hop_rem    = _hop_size - _hop_pos;
	const int      proc_len   = std::min (nbr_spl, hop_rem);

	dly::RingBufVectorizer  rbv (_buf_len);
	int            pos_dst    = 0;
	for (rbv.start (proc_len, _buf_pos); rbv.end (); rbv.next ())
	{
		const int      blk_len = rbv.get_seg_len ();
		const int      blk_pos = rbv.get_curs_pos (0);

		const auto     it_src  = _buf.begin () + blk_pos;
		std::copy (it_src, it_src + blk_len, dst_ptr + pos_dst);
		std::fill (it_src, it_src + blk_len, T (0));

		pos_dst += blk_len;
	}
	_buf_pos = rbv.get_curs_pos (0);
	_hop_pos += proc_len;

	const bool     frame_flag = (_hop_pos >= _hop_size);
	if (frame_flag)
	{
		_hop_pos = 0;
	}

	return ResProcBlock { nbr_spl - proc_len, frame_flag };
}



/*
==============================================================================
Name: set_frame
Description:
	Feeds the object with a new input frame, if required.
Input parameters:
	- frame_ptr: pointer on the frame sample data.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	FrameOverlapSyn <T>::set_frame (const T frame_ptr []) noexcept
{
	assert (_frame_size > 0);
	assert (_hop_pos == 0);
	assert (frame_ptr != nullptr);

	dly::RingBufVectorizer  rbv (_buf_len);
	int            pos_src = 0;
	for (rbv.start (_frame_size, _buf_pos); rbv.end (); rbv.next ())
	{
		const int      blk_len = rbv.get_seg_len ();
		const int      blk_pos = rbv.get_curs_pos (0);

		const T * fstb_RESTRICT src_ptr = frame_ptr    + pos_src;
		T *       fstb_RESTRICT dst_ptr = _buf.data () + blk_pos;
		for (int pos = 0; pos < blk_len; ++pos)
		{
			dst_ptr [pos] += src_ptr [pos];
		}

		pos_src += blk_len;
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears the output samples. Frame synchronisation position is set again
	with the offset value from the setup() call.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	FrameOverlapSyn <T>::clear_buffers () noexcept
{
	assert (_frame_size > 0);

	std::fill (_buf.begin (), _buf.end (), T (0));
	_buf_pos = 0;
	_hop_pos = _hop_reset;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spec
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spec_FrameOverlapSyn_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
