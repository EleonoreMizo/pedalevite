/*****************************************************************************

        FrameOverlapAna.hpp
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spec_FrameOverlapAna_CODEHEADER_INCLUDED)
#define mfx_dsp_spec_FrameOverlapAna_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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
Name: reserve
Description:
	Allocates memory for a maximum frame size. This is useful when the frame
	size is dynamic and is changed in a real-time thread, so setup() is
	guaranteed not throwing.
Input parameters:
	- frame_size: maximum length of the frame, in samples. > 0
Throws: depends on std::vector
==============================================================================
*/

template <typename T>
void	FrameOverlapAna <T>::reserve (int frame_size)
{
	assert (frame_size > 0);

	const int      buf_len = 1 << fstb::get_next_pow_2 (frame_size);
	_buf.reserve (buf_len);
}



/*
==============================================================================
Name: setup
Description:
	Sets the frame parameters, namely the frame size and the hop size.
	It is possible to synchronize the position where a frame is ready. This
	is helpful to reduce the global analysis-synthesis latency when the audio
	callback has a known, fixed size. Therefore it is possible to make a frame
	ready for analysis at the very end of the processing block and to start the
	corresponding synthesis frame at the begin of the block.
	The internal buffer is not cleared and may contain garbage.
	Call to this function is mandatory before calling anything else.
Input parameters:
	- frame_size: length of the frame, in samples. > 0
	- hop_size: time interval between two frames, in samples. ]0 ; frame_size]
	- offset: sets the moment where a frame will be ready, in samples.
		0 will trigger an analysis frame immediately, or by a hop_size distance.
		The offset value is stored so it is set again when clear_buffers() is
		called. [0 ; frame_size[.
Throws: depends on std::vector
==============================================================================
*/

template <typename T>
void	FrameOverlapAna <T>::setup (int frame_size, int hop_size, int offset)
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

	_buf_len    = 1 << fstb::get_next_pow_2 (frame_size);
	_buf_msk    = _buf_len - 1;
	_buf_pos   &= _buf_msk;
	_buf.resize (_buf_len, T (0)); // Fills new samples with zeros
}



/*
==============================================================================
Name: get_len_before_next_frame
Description:
	Returns the number of processing samples before the next frame is ready.
	If a frame is ready during the call, it returns the full distance to the
	next frame (hop size).
Returns: distance in sample, > 0
Throws: Nothing
==============================================================================
*/

template <typename T>
int	FrameOverlapAna <T>::get_len_before_next_frame () const noexcept
{
	assert (_frame_size > 0);

	return _hop_size - _hop_pos;
}



/*
==============================================================================
Name: is_frame_proc_required
Description:
	Indicates if a frame is ready and should be retrieved for processing.
Returns: true if one is ready
Throws: Nothing
==============================================================================
*/

template <typename T>
bool	FrameOverlapAna <T>::is_frame_proc_required () const noexcept
{
	assert (_frame_size > 0);

	return (_hop_pos == 0);
}



/*
==============================================================================
Name: process_sample
Description:
	Feeds an input sample.
Input parameters:
	- x: The sample
Returns: true if a frame is ready and should be retrieved for processing
Throws: Nothing
==============================================================================
*/

template <typename T>
bool	FrameOverlapAna <T>::process_sample (T x) noexcept
{
	assert (_frame_size > 0);

	_buf [_buf_pos] = x;

	_buf_pos = (_buf_pos + 1) & _buf_msk;
	++ _hop_pos;

	const int      hop_rem    = _hop_size - _hop_pos;
	const bool     frame_flag = (hop_rem <= 0);
	check_frame_ready (frame_flag);

	return frame_flag;
}



/*
==============================================================================
Name: process_block
Description:
	Feeds a block of input sample. If a frame is ready in the middle of the
	course, the feeding is stopped and the number of remaining (unprocessed)
	samples is returned. Client should call process_block() again with this
	value to continue processing after retrieving the frame.
Input parameters:
	- src_ptr: pointer on the block of input samples
	- nbr_spl: number of samples to insert.
Returns: a structure containing:
	- _nbr_spl_rem: number of remaining samples. Usually 0, or something below
		nbr_spl if a frame has to be processed in between.
	- _frame_flag: indicates if a new frame is issued and should be retrieved.
Throws: Nothing
==============================================================================
*/

template <typename T>
typename FrameOverlapAna <T>::ResProcBlock	FrameOverlapAna <T>::process_block (const T src_ptr [], int nbr_spl) noexcept
{
	assert (_frame_size > 0);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	const int      hop_rem    = _hop_size - _hop_pos;
	const int      proc_len   = std::min (nbr_spl, hop_rem);

	dly::RingBufVectorizer  rbv (_buf_len);
	int            pos_src    = 0;
	for (rbv.start (proc_len, _buf_pos); rbv.end (); rbv.next ())
	{
		const int      blk_len = rbv.get_seg_len ();
		const int      blk_pos = rbv.get_curs_pos (0);

		const auto     it_src  = src_ptr + pos_src;
		std::copy (it_src, it_src + blk_len, _buf.begin () + blk_pos);

		pos_src += blk_len;
	}
	_buf_pos = rbv.get_curs_pos (0);
	_hop_pos += proc_len;

	const bool     frame_flag = (_hop_pos >= _hop_size);
	check_frame_ready (frame_flag);

	return ResProcBlock { nbr_spl - proc_len, frame_flag };
}



/*
==============================================================================
Name: get_frame
Description:
	Retrieves the frame content, when it is ready.
Output parameters:
	- frame_ptr: pointer on the frame data. Buffer should be allocated by the
		client, and big enough to receive all the data.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	FrameOverlapAna <T>::get_frame (T frame_ptr []) const noexcept
{
	assert (_frame_size > 0);
	assert (_hop_pos == 0);
	assert (frame_ptr != nullptr);

	dly::RingBufVectorizer  rbv (_buf_len);
	int            pos_dst = 0;
	for (rbv.start (_frame_size, _read_pos); rbv.end (); rbv.next ())
	{
		const int      blk_len = rbv.get_seg_len ();
		const int      blk_pos = rbv.get_curs_pos (0);

		const auto     it_src  = _buf.begin () + blk_pos;
		std::copy (it_src, it_src + blk_len, frame_ptr + pos_dst);

		pos_dst += blk_len;
	}
}



/*
==============================================================================
Name: get_frame
Description:
	Retrieves a single sample of the frame content, when it is ready.
Input parameters:
	- idx: index of the sample to retrieve within the frame. [0 ; _frame_size[.
Returns:
	The requested sample value.
Throws: Nothing
==============================================================================
*/

template <typename T>
T	FrameOverlapAna <T>::get_frame_sample (int idx) const noexcept
{
	assert (_frame_size > 0);
	assert (_hop_pos == 0);
	assert (idx >= 0);
	assert (idx < _frame_size);

	const int      pos_buf = (_read_pos + idx) & _buf_msk;

	return _buf [pos_buf];
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears the input samples. Frame synchronisation position is set again
	with the offset value from the setup() call.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	FrameOverlapAna <T>::clear_buffers () noexcept
{
	assert (_frame_size > 0);

	std::fill (_buf.begin (), _buf.end (), T (0));
	_buf_pos = 0;
	_hop_pos = _hop_reset;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	FrameOverlapAna <T>::check_frame_ready (bool frame_flag) noexcept
{
	if (frame_flag)
	{
		_hop_pos  = 0;
		_read_pos = (_buf_pos - _frame_size) & _buf_msk;
	}
}



}  // namespace spec
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spec_FrameOverlapAna_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
