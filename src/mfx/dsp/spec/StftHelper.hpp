/*****************************************************************************

        StftHelper.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spec_StftHelper_CODEHEADER_INCLUDED)
#define mfx_dsp_spec_StftHelper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/RingBufVectorizer.h"
#include "mfx/dsp/mix/Generic.h"

#include <cstring>



namespace mfx
{
namespace dsp
{
namespace spec
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class FFT>
StftHelper <FFT>::StftHelper ()
:	_fft ()
,	_win ()
,	_buf_acc ()
,	_buf_in (FftType::FFT_LEN)
,	_hop_size (FftType::FFT_LEN / 2)
,	_hop_pos (0)
{
	mix::Generic::setup ();
	_buf_acc.set_sample_freq (1);
	_buf_acc.set_max_delay_time (FftType::FFT_LEN);
	_buf_acc.update_buffer_size ();
}



template <class FFT>
void	StftHelper <FFT>::set_hop_size (int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_hop_size = nbr_spl;
}



// 0 = removes the window (rectangular)
template <class FFT>
void	StftHelper <FFT>::set_win (const float win_ptr [FftType::FFT_LEN])
{
	if (win_ptr == nullptr)
	{
		_win.clear ();
	}

	else
	{
		_win.resize (FftType::FFT_LEN);
		mix::Generic::copy_1_1 (&_win [0], &win_ptr [0], FftType::FFT_LEN);
	}
}



template <class FFT>
void	StftHelper <FFT>::clear_buffers () noexcept
{
	_buf_pos = 0;
	_hop_pos = 0;
	_buf_acc.clear_buffers ();
}



// On output:
// trans_flag indicates that a new tranform has been issued
// nbr_spl_proc indicates how many input samples have been processed.
// Repeat the call until all samples are processed.
template <class FFT>
void	StftHelper <FFT>::process_block (const float spl_ptr [], float freq_ptr [FftType::FFT_LEN], int nbr_spl, bool &trans_flag, int &nbr_spl_proc) noexcept
{
	assert (spl_ptr != nullptr);
	assert (freq_ptr != nullptr);
	assert (nbr_spl > 0);

	int            work_len = nbr_spl;
	work_len = std::min (work_len, _hop_size - _hop_pos);
	
	// Stores new data into the input buffer
	float * const  buf_acc_ptr = _buf_acc.get_buffer ();
	int            pos_in      = 0;
	dly::RingBufVectorizer  rbv (_buf_acc.get_len ());
	for (rbv.start (work_len, _buf_pos); rbv.end (); rbv.next ())
	{
		const int      len_blk = rbv.get_seg_len ();
		const int      pos_buf = rbv.get_curs_pos (0);
		mix::Generic::copy_1_1 (
			buf_acc_ptr + pos_buf,
			spl_ptr + pos_in,
			len_blk
		);
		pos_in += len_blk;
	}

	const int      mask = _buf_acc.get_mask ();
	_buf_pos += work_len;
	_buf_pos &= mask;

	_hop_pos += work_len;
	trans_flag = (_hop_pos >= _hop_size);
	if (trans_flag)
	{
		_hop_pos -= _hop_size;

		// Copies data from the input buffer to the time-domain buffer
		// Applies the window if required
		const int      read_pos  = (_buf_pos - FftType::FFT_LEN) & mask;
		int            write_pos = 0;
		for (rbv.start (FftType::FFT_LEN, read_pos); rbv.end (); rbv.next ())
		{
			const int      len_blk = rbv.get_seg_len ();
			const int      pos_buf = rbv.get_curs_pos (0);

			// Without window
			if (_win.empty ())
			{
				mix::Generic::copy_1_1 (
					&_buf_in [write_pos],
					buf_acc_ptr + pos_buf,
					len_blk
				);
			}

			// Uses a window
			else
			{
				mix::Generic::mult_1_1 (
					&_buf_in [write_pos],
					&buf_acc_ptr [pos_buf],
					&_win [write_pos],
					len_blk
				);
			}

			write_pos += len_blk;
		}

		// Transform without scaling
		_fft.do_fft (&freq_ptr [0], &_buf_in [0]);
	}

	nbr_spl_proc = work_len;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spec
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spec_StftHelper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
