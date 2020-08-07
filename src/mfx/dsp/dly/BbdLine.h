/*****************************************************************************

        BbdLine.h
        Author: Laurent de Soras, 2016

Simple emulation of an ideal Bucket Brigade Device line.
It is advised to oversample this processor.
Does not include anti-aliasing filters for speeds < 1.

Call order in a loop:
set_speed()
estimate_max_one_shot_proc_w_feedback()
read_block()
push_block()

Reference:

Antti Huovilainen, Enhanced Digital Models for Analog Modulation Effects,
Proc. Digital Audio Effects (DAFx-05), 2005, pp. 155–160.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_BbdLine_HEADER_INCLUDED)
#define mfx_dsp_dly_BbdLine_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"
#include "mfx/dsp/dly/DelayLineData.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace dsp
{

namespace rspl
{
	class InterpolatorInterface;
}

namespace dly
{



template <int MSL2 = -4, typename AL = std::allocator <float> >
class BbdLine
{
	static_assert (MSL2 <= 0, "Log2 of the minimum BBD speed must be <= 0.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Log2 of the minimum BBD speed, <= 0. Impacts data buffer size.
	static constexpr int  _min_speed_l2 = MSL2;

	void           init (int max_bbd_size, rspl::InterpolatorInterface &interp, int ovrspl_l2);
	int            get_ovrspl_l2 () const;
	const rspl::InterpolatorInterface &
	               use_interpolator () const;

	void           set_bbd_size (int bbd_size);
	int            get_bbd_size () const;

	void           set_speed (float speed);
	float          compute_min_delay () const;
	int            estimate_max_one_shot_proc_w_feedback (float dly_min) const;
	void           read_block (float dst_ptr [], long nbr_spl, float dly_beg, float dly_end, int pos_in_block) const;
	float          read_sample (float dly) const;
	void           push_block (const float src_ptr [], int nbr_spl);
	void           push_sample (float x);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::dly::DelayLineData <
		fstb::FixedPoint,
		typename std::allocator_traits <AL>::template rebind_alloc <fstb::FixedPoint>
	> TimestampLine;
	typedef dsp::dly::DelayLineData <float, AL> DataLine;

	fstb_FORCEINLINE void
	               push_timestamps (int nbr_spl);
	fstb_FORCEINLINE float
	               read_sample (float dly_cur, int ts_mask, const fstb::FixedPoint ts_buf_ptr [], int data_mask, int data_len, const float data_buf_ptr [], int pos_in_block) const;

	int            _max_bbd_size = 0;
	TimestampLine  _line_ts;

	rspl::InterpolatorInterface *       // 0: interpolator not set.
	               _interp_ptr   = nullptr;
	DataLine       _line_data;

	float          _speed        = 1;   // Speed of the delay line relative to the in/out stream. In other words, it's a dynamic oversampling factor. Range: ]0 ; +oo[. The higher the speed, the higher the CPU load.
	float          _speed_inv    = 1;   // 1 / _speed
	int            _bbd_size     = 0;   // Number of BBD elements (samples). Must be a power of 2. 0 = not set
	float          _ts_pos_w     = 0;   // Writing position within the BBD line for the beginning of the block. [0 ; _bbd_size[
	int            _data_pos_w   = 0;   // Writing position within the data line.
	int            _ovrspl_l2    = 0;   // Base-2 logarithm of the oversampling. >= 0.
	float          _ovrspl_inv   = 0;   // 1.f / (1 << _ovrspl_l2)
	fstb::FixedPoint
	               _group_dly    = fstb::FixedPoint (0, 0);  // Interpolator group delay
	int            _imp_len      = 1;   // Interpolator impulse length, >= 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const BbdLine <MSL2, AL> &other) const = delete;
	bool           operator != (const BbdLine <MSL2, AL> &other) const = delete;

}; // class BbdLine



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/BbdLine.hpp"



#endif   // mfx_dsp_dly_BbdLine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
