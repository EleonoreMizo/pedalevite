/*****************************************************************************

        DelayLineData.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLineData_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <memory>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace dly
{



template <typename T, typename AL = std::allocator <T> >
class DelayLineData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T  ValueType;
	typedef AL AllocatorType;

	               DelayLineData (const AllocatorType &al = AllocatorType ());
	virtual        ~DelayLineData () = default;

	void           set_extra_len (int nbr_spl);
	int            get_extra_len () const;
	void           set_unroll_pre (int nbr_spl);
	int            get_unroll_pre () const;
	void           set_unroll_post (int nbr_spl);
	int            get_unroll_post () const;

	void           set_sample_freq (double sample_freq);
	void           set_max_delay_time (double max_time);
	double         get_max_delay_time () const;

	void           update_buffer_size ();
	void           update_unroll ();
	void           update_unroll_pre ();
	void           update_unroll_post ();

	int            get_len () const;
	int            get_mask () const;
	ValueType *    get_buffer ();
	const ValueType *
	               get_buffer () const;

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <ValueType, AllocatorType> Buffer;

	void           invalidate_buf ();

	Buffer         _buf;
	ValueType *    _buf_ptr;         // Points on virtual buffer beginning (after _head_len)
	                                 // 0 means buffer needs update

	double         _sample_freq;     // Hz, > 0. 0 if not set.
	double         _max_time;        // s, > 0. 0 if not set.
	int            _unroll_pre;      // Samples, >= 0
	int            _unroll_post;     // Samples, >= 0
	int            _extra_len;       // Samples, >= 0
	int            _buf_len;         // Power of 2, >= len + extra. 0 if the buffer is not ready.
	int            _buf_mask;        // _buf_len - 1. Always >= 0, even when the buffer is not ready.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLineData (const DelayLineData &other)     = delete;
	DelayLineData &
	               operator = (const DelayLineData &other)        = delete;
	bool           operator == (const DelayLineData &other) const = delete;
	bool           operator != (const DelayLineData &other) const = delete;

}; // class DelayLineData



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelayLineData.hpp"



#endif   // mfx_dsp_dly_DelayLineData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
