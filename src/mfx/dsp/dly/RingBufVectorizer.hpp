/*****************************************************************************

        RingBufVectorizer.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_RingBufVectorizer_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_RingBufVectorizer_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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
Name: ctor
Input parameters:
	- size: Buffer size, > 0.
Throws: Nothing
==============================================================================
*/

RingBufVectorizer::RingBufVectorizer (int size)
:	_size (size)
,	_nbr_curs (0)
{
	assert (size > 0);
}



/*
==============================================================================
Name: get_size
Description:
	Returns the buffer size.
Returns: the size, > 0
Throws: Nothing
==============================================================================
*/

int	RingBufVectorizer::get_size () const
{
	return (_size);
}



/*
==============================================================================
Name: end
Description:
	Checks if the started run is finished (we reached the last segment)
Returns:
	true if the required zone hasn't been fully completed, false otherwise.
Throws: Nothing
==============================================================================
*/

bool	RingBufVectorizer::end () const
{
	assert (_nbr_curs > 0);

	return (_len > 0);
}



/*
==============================================================================
Name: get_seg_len
Description:
	Returns the current segment length. To be called after start(), restart ()
	or next().
Returns: The length, > 0.
Throws: Nothing
==============================================================================
*/

int	RingBufVectorizer::get_seg_len () const
{
	assert (_nbr_curs > 0);
	assert (_seg_len > 0);

	return (_seg_len);
}



/*
==============================================================================
Name: get_curs_pos
Description:
	Returns the position of a given cursor.
Input parameters:
	- curs: Index of the requested cursor (0 to N-1).
Returns: The position, in range [0 ; size[.
Throws: Nothing
==============================================================================
*/

int	RingBufVectorizer::get_curs_pos (int curs) const
{
	assert (_nbr_curs > 0);
	assert (curs < _nbr_curs);

	return (_pos [curs]);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_RingBufVectorizer_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
