/*****************************************************************************

        MovingSum.h
        Author: Laurent de Soras, 2019

Computes the running sum of the last N samples.
Can process integer and floating point data.

Template parameters:

- DT: Main data type (input).

- ST: Sum type (output). Should be same or more accurate than DT.

Algorithm: Evan Balster on the Music-DSP mailing list,
"Re: [music-dsp] highly optimised variable rms and more", 2016-07-19

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_MovingSum_HEADER_INCLUDED)
#define mfx_dsp_fir_MovingSum_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace fir
{



template <typename DT = float, typename ST = double>
class MovingSum
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef DT DataType;
	typedef ST SumType;

	void           set_win_len (int len);

	inline SumType process_sample (DataType x);
	void           process_block (SumType dst_ptr [], const DataType src_ptr [], int nbr_spl);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <DataType> SplArray;

	SplArray      _buf     = SplArray (1, 0);
	int           _win_len = 1;         // Samples, > 0
	int           _pos_rw  = 0;         // Reading and writing pos
	SumType       _sum_u   = 0;
	SumType       _sum_d   = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const MovingSum <DT, ST> &other) const = delete;
	bool           operator != (const MovingSum <DT, ST> &other) const = delete;

}; // class MovingSum



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/fir/MovingSum.hpp"



#endif   // mfx_dsp_fir_MovingSum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
