/*****************************************************************************

        RingBufVectorizer.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_size
Description:
	Sets a new size for the circular buffer.
	All cursors are reset.
	Current segment is left.
Input parameters:
	- size: Buffer size, > 0.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::set_size (int size) noexcept
{
	assert (size > 0);

	_size = size;
	for (int curs = 0; curs < _nbr_curs; ++curs)
	{
		_pos [curs] = 0;
	}
	_len     = 0;
	_seg_len = 0;
}



/*
==============================================================================
Name: set_curs
Description:
	Initializes some cursors without starting a run (do it with a subsequent
	restart ()).
Input parameters:
	- nbr_curs: Number of cursors, [1 ; _max_nbr_curs]
	- pos_arr: Pointer on an array containing the initial cursor positions,
		in [0 ; size[.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::set_curs (int nbr_curs, const int pos_arr []) noexcept
{
	assert (nbr_curs > 0);
	assert (nbr_curs <= _max_nbr_curs);

	_nbr_curs = nbr_curs;
	for (int curs = 0; curs < _nbr_curs; ++curs)
	{
		assert (pos_arr [curs] >= 0);
		assert (pos_arr [curs] < _size);
		_pos [curs] = pos_arr [curs];
	}
}



/*
==============================================================================
Name: start
Description:
	Start a single cursor.
Input parameters:
	- len: Length of the run, > 0.
	- pos: Current cursor 0 position, in [0 ; size[.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::start (int len, int pos) noexcept
{
	assert (len > 0);
	assert (pos >= 0);
	assert (pos < _size);

	_nbr_curs = 1;
	_pos [0]  = pos;
	_len      = len;

	compute_seg ();
}



/*
==============================================================================
Name: start
Description:
	Starts two cursors.
Input parameters:
	- len: Length of the run, > 0.
	- pos_0: Current cursor 0 position, in [0 ; size[.
	- pos_1: Current cursor 1 position, in [0 ; size[.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::start (int len, int pos_0, int pos_1) noexcept
{
	assert (len > 0);
	assert (pos_0 >= 0);
	assert (pos_0 < _size);
	assert (pos_1 >= 0);
	assert (pos_1 < _size);

	_nbr_curs = 2;
	_pos [0]  = pos_0;
	_pos [1]  = pos_1;
	_len      = len;

	compute_seg ();
}



/*
==============================================================================
Name: start
Description:
	Starts an arbitrary number of cursors.
Input parameters:
	- len: Length of the run, > 0
	- nbr_curs: Number of cursors to run, [1 ; _max_nbr_curs]
	- pos_arr: Pointer on an array containing the initial cursor positions,
		in [0 ; size[.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::start (int len, int nbr_curs, const int pos_arr []) noexcept
{
	assert (len > 0);

	set_curs (nbr_curs, pos_arr);
	_len = len;

	compute_seg ();
}



/*
==============================================================================
Name: restart
Description:
	Runs the cursor again, starting from their last known position.
	This function should be called only after a start() or a restart().
Input parameters:
	- len: New length of the run, > 0
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::restart (int len) noexcept
{
	assert (_nbr_curs > 0);
	assert (len > 0);

	_len = len;

	compute_seg ();
}



/*
==============================================================================
Name: next
Description:
	Computes and sets the next segment.
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::next () noexcept
{
	assert (_nbr_curs > 0);
	assert (_len > 0);

	_len -= _seg_len;
	for (int curs = 0; curs < _nbr_curs; ++curs)
	{
		_pos [curs] += _seg_len;

		assert (_pos [curs] <= _size);
		assert (_pos [curs] >= 0);
		
		if (_pos [curs] == _size)
		{
			_pos [curs] = 0;
		}
	}

	compute_seg ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: compute_seg
Description:
	Computes the next segment. This function is called after start(), restart()
	or next().
Throws: Nothing
==============================================================================
*/

void	RingBufVectorizer::compute_seg () noexcept
{
	assert (_nbr_curs > 0);

	_seg_len = _len;
	for (int curs = 0; curs < _nbr_curs; ++curs)
	{
		const int      margin = _size - _pos [curs];
		_seg_len = std::min (_seg_len, margin);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
