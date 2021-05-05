/*****************************************************************************

        FrameOverlapAna.h
        Author: Laurent de Soras, 2021

Analysis frame splitting with overlap.
Windowing should be performed on the client side.
Frame output is done as soon as enough data is accumulated.

Template parameters:

- T: sample data type

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spec_FrameOverlapAna_HEADER_INCLUDED)
#define mfx_dsp_spec_FrameOverlapAna_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace spec
{



template <typename T>
class FrameOverlapAna
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T DataType;
	typedef FrameOverlapAna <T> ThisType;

	class ResProcBlock
	{
	public:
		int            _nbr_spl_rem = 0;
		bool           _frame_flag  = false;
	};

	void           reserve (int frame_size);
	void           setup (int frame_size, int hop_size, int offset);

	inline int     get_len_before_next_frame () const noexcept;
	inline bool    is_frame_proc_required () const noexcept;

	inline bool    process_sample (T x) noexcept;
	ResProcBlock   process_block (const T src_ptr [], int nbr_spl) noexcept;
	void           get_frame (T frame_ptr []) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <T> RingBuffer;

	// Input ring buffer
	RingBuffer     _buf;

	// Length of the buffer in samples, >= _frame_size. Must be a power of 2.
	int            _buf_len    = 0;

	// Bit mask to wrap indexes in the buffer. Usually _buf_len - 1.
	int            _buf_msk    = 0;

	// Write position within the buffer [0 ; _buf_len[
	int            _buf_pos    = 0;

	// Frame length, in samples, > 0
	int            _frame_size = 0;

	// Distance in samples between two frames, ]0 ; _frame_size]
	int            _hop_size   = 0;

	// Distance in samples from the previous frame to the current point.
	// [0 ; _hop_size[
	int            _hop_pos    = 0;

	// Initial value indicating the distance in samples between the current
	// point and the next frame. [0 ; _hop_size[
	int            _hop_reset  = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FrameOverlapAna &other) const = delete;
	bool           operator != (const FrameOverlapAna &other) const = delete;

}; // class FrameOverlapAna



}  // namespace spec
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spec/FrameOverlapAna.hpp"



#endif   // mfx_dsp_spec_FrameOverlapAna_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
