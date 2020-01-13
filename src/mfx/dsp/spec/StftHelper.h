/*****************************************************************************

        StftHelper.h
        Author: Laurent de Soras, 2018

Template parameters:

- FFT: an ffft::FFTRealFixLen class.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spec_StftHelper_HEADER_INCLUDED)
#define mfx_dsp_spec_StftHelper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "mfx/dsp/dly/DelayLineData.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace spec
{



template <class FFT>
class StftHelper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef FFT FftType;

	static_assert (
		(FftType::FFT_LEN & (FftType::FFT_LEN - 1)) == 0,
		"FFT length must be a power of 2"
	);

	               StftHelper ();
	virtual        ~StftHelper () = default;

	void           set_rate_mode (bool opt_flag);
	void           set_hop_size (int nbr_spl);
	void           set_win (const float win_ptr [FftType::FFT_LEN]);
	void           clear_buffers ();
	void           process_block (const float spl_ptr [], float freq_ptr [FftType::FFT_LEN], int nbr_spl, bool &trans_flag, int &nbr_spl_proc);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef dly::DelayLineData <float, fstb::AllocAlign <float, 16> > RingBuffer;

	FftType        _fft;
	BufAlign       _win;                // Empty if no window set (rectangular window)
	RingBuffer     _buf_acc;            // Raw input data
	BufAlign       _buf_in;             // Time domain
	int            _buf_pos;            // Write position within the raw input buffer
	int            _hop_size;
	int            _hop_pos;
	bool           _opt_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               StftHelper (const StftHelper <FFT> &other)        = delete;
	               StftHelper (StftHelper <FFT> &&other)             = delete;
	StftHelper <FFT> &
	               operator = (const StftHelper <FFT> &other)        = delete;
	StftHelper <FFT> &
	               operator = (StftHelper <FFT> &&other)             = delete;
	bool           operator == (const StftHelper <FFT> &other) const = delete;
	bool           operator != (const StftHelper <FFT> &other) const = delete;

}; // class StftHelper



}  // namespace spec
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spec/StftHelper.hpp"



#endif   // mfx_dsp_spec_StftHelper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
