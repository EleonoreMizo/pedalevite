/*****************************************************************************

        SlidingOp.h
        Author: Laurent de Soras, 2019

Computes an operation on a sliding window.
The operation should be commutative and associative (addition, multiplication,
min, max...)

Template paramteters:

- T: sample data type

- OP: binary functor for the operation. Requires:
	OP::OP ();
	T OP::operator () (T, T);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_SlidingOp_HEADER_INCLUDED)
#define mfx_dsp_fir_SlidingOp_HEADER_INCLUDED

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



template <typename T, typename OP>
class SlidingOp
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef T  DataType;
	typedef OP Operator;

	               SlidingOp ();
	               SlidingOp (const SlidingOp &other)  = default;
	               SlidingOp (SlidingOp &&other)       = default;

	virtual        ~SlidingOp ()                       = default;

	SlidingOp &    operator = (const SlidingOp &other) = default;
	SlidingOp &    operator = (SlidingOp &&other)      = default;

	Operator &     use_ftor ();
	void           set_length (int len);
	void           clear_buffers ();
	void           fill (const DataType &val);

	DataType       process_sample (DataType x);
	void           process_block (DataType dst_ptr [], const DataType src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Last level (leaf): the original data
	// Other levels: Result values using operands from the next level
	// lvl [n] [i] = _op (lvl [n+1] [i*2], lvl [n+1] [i*2+1])
	// If the next level has only one element available, the result
	// is copied from this element.
	// First level is always 1 element-long but doesn't contain anything.
	typedef std::vector <T    > Level;
	typedef std::vector <Level> LevelArray;

	LevelArray     _lvl_arr;
	int            _pos_w;     // Writing position. <= _nbr_avail
	int            _len;       // Length of the window
	int            _nbr_avail; // Number of available past samples
	OP             _op;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SlidingOp &other) const = delete;
	bool           operator != (const SlidingOp &other) const = delete;

}; // class SlidingOp



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/fir/SlidingOp.hpp"



#endif   // mfx_dsp_fir_SlidingOp_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
