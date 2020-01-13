/*****************************************************************************

        DelayLineFracFir4Base.h
        Author: Laurent de Soras, 2016

Intended to be used in a sample per sample way.
Client should provide the FIR data.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLineFracFir4Base_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineFracFir4Base_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/dly/DelayLineData.h"



namespace mfx
{
namespace dsp
{
namespace dly
{



template <typename DT, typename AL = std::allocator <DT> >
class DelayLineFracFir4Base
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef DT DataType;
	typedef AL AllocatorType;

	explicit			DelayLineFracFir4Base (const AllocatorType &al = AllocatorType ());
	               DelayLineFracFir4Base (const DelayLineFracFir4Base &other) = default;
	               DelayLineFracFir4Base (DelayLineFracFir4Base &&other)      = default;

	               ~DelayLineFracFir4Base ()                       = default;

	DelayLineFracFir4Base &
	               operator = (const DelayLineFracFir4Base &other) = default;
	DelayLineFracFir4Base &
	               operator = (DelayLineFracFir4Base &&other)      = default;

	void           set_max_time (long max_delay_time);
	inline long    get_max_time () const;

	void           set_time (long delay_time);
	inline long    get_time () const;

	inline long    get_line_length () const;
	inline long    get_write_pos () const;

	fstb_FORCEINLINE DataType
	               read_sample (const DataType fir_data []) const;
	fstb_FORCEINLINE DataType
	               read_sample_unroll (const DataType fir_data []) const;

	fstb_FORCEINLINE void
	               write_sample (DataType sample);
	fstb_FORCEINLINE void
	               write_sample_unroll (DataType sample);

	fstb_FORCEINLINE void
	               step_one_sample ();

	void           clear_buffers ();
	void           clear_buffers_fast ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  FIR_LEN     = 4;
	static const int  UNROLL_POST = FIR_LEN - 1;

	DelayLineData <DataType, AllocatorType>
	               _buf;
	long           _delay_time_max;  // In samples
	long           _delay_time_int;
	long           _write_pos;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayLineFracFir4Base &other) const     = delete;
	bool           operator != (const DelayLineFracFir4Base &other) const     = delete;

}; // class DelayLineFracFir4Base



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelayLineFracFir4Base.hpp"



#endif   // mfx_dsp_dly_DelayLineFracFir4Base_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
