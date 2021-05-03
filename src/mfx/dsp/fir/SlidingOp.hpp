/*****************************************************************************

        SlidingOp.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_fir_SlidingOp_CODEHEADER_INCLUDED)
#define mfx_dsp_fir_SlidingOp_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Description:
	The object is constructed with a default size of 1, so processing is a
	pass-through.
Throws: depends on std::vector and T::T()
==============================================================================
*/

template <typename T, typename OP>
SlidingOp <T, OP>::SlidingOp ()
:	_lvl_arr ()
,	_pos_w (0)
,	_len (0)
,	_nbr_avail (0)
,	_op ()
{
	set_length (1);
}



/*
==============================================================================
Name: use_ftor
Description:
	Gives access to the functor element.
Returns: the functor object.
Throws: Nothing
==============================================================================
*/

template <typename T, typename OP>
typename SlidingOp <T, OP>::Operator &	SlidingOp <T, OP>::use_ftor () noexcept
{
	return _op;
}



/*
==============================================================================
Name: set_length
Description:
	Sets the size of the window. Past samples are lost.
Input parameters:
	- len: size of the window, > 0.
Throws: depends on std::vector and T::T()
==============================================================================
*/

template <typename T, typename OP>
void	SlidingOp <T, OP>::set_length (int len)
{
	assert (len > 0);

	_len = len;

	// Levels have 2 children per parent
	const int      nbr_lvl = fstb::get_next_pow_2 (len) + 1;
	_lvl_arr.resize (nbr_lvl);

	int            cur_len = len;
	for (int lvl_idx = nbr_lvl - 1; lvl_idx >= 0; --lvl_idx)
	{
		_lvl_arr [lvl_idx].resize (cur_len);
		cur_len = (cur_len + 1) >> 1; // Rounded up
	}
	assert (cur_len == 1);

	clear_buffers ();
}



/*
==============================================================================
Name: clear_buffers
Description:
	Erase all past samples.
Throws: Nothing
==============================================================================
*/

template <typename T, typename OP>
void	SlidingOp <T, OP>::clear_buffers () noexcept
{
	_pos_w     = 0;
	_nbr_avail = 0;
}



/*
==============================================================================
Name: fill
Description:
	Fills the internal state with a given value, like if the past input samples
	were all equal to this value.
Input parameters:
	- val: initialisation value
Throws: Nothing
==============================================================================
*/

template <typename T, typename OP>
void	SlidingOp <T, OP>::fill (const DataType &val) noexcept
{
	for (auto &lvl : _lvl_arr)
	{
		for (auto &elt : lvl)
		{
			elt = val;
		}
	}

	_pos_w     = 0;   // Not necessary
	_nbr_avail = _len;
}



/*
==============================================================================
Name: process_sample
Description:
	Inserts a new sample, shifts the window and computes the operation.
	If there is not enough past samples to fill a full window, the window is
	shortened to the available samples.
Input parameters:
	- x: The new sample
Returns: the result of the operation on all the operands from the new window.
Throws: Nothing
==============================================================================
*/

template <typename T, typename OP>
typename SlidingOp <T, OP>::DataType	SlidingOp <T, OP>::process_sample (DataType x) noexcept
{
	// A new sample arrived
	if (_nbr_avail < _len)
	{
		++ _nbr_avail;
	}

	// Recursive operation through the levels, excepted the first one which is
	// skipped.
	int            lvl_idx       = int (_lvl_arr.size ()) - 2;
	int            pos_lvl_cur   = _pos_w;
	int            nbr_avail_nxt = _nbr_avail;

	while (lvl_idx >= 0)
	{
		Level &        lvl_nxt = _lvl_arr [lvl_idx + 1];
		lvl_nxt [pos_lvl_cur] = x;

		const int      pos_lvl_nxt = pos_lvl_cur & ~1;
		pos_lvl_cur >>= 1;
		if (nbr_avail_nxt - pos_lvl_nxt > 1)
		{
			x = _op (lvl_nxt [pos_lvl_nxt], lvl_nxt [pos_lvl_nxt + 1]);
		}

		nbr_avail_nxt = (nbr_avail_nxt + 1) >> 1; // Rounded up
		-- lvl_idx;
	}

	// Updates the position for the next upcoming input sample
	++ _pos_w;
	if (_pos_w >= _len)
	{
		_pos_w = 0;
	}

	return x;
}



/*
==============================================================================
Name: process_block
Description:
	Inserts new samples and for each sample, shifts the window and computes the
	operation.
	Can work in-place.
Input parameters:
	- src_ptr: Pointer on the input buffer. Not null.
	- nbr_spl: Number of samples to process. > 0.
Output parameters:
	- dst_ptr: Pointer on the preallocated result buffer. Not null.
Throws: Nothing
==============================================================================
*/

template <typename T, typename OP>
void	SlidingOp <T, OP>::process_block (DataType dst_ptr [], const DataType src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != 0);
	assert (src_ptr != 0);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_fir_SlidingOp_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
