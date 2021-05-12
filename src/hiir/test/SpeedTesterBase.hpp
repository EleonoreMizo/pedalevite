/*****************************************************************************

        SpeedTesterBase.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_SpeedTesterBase_CODEHEADER_INCLUDED)
#define hiir_test_SpeedTesterBase_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/conf.h"

#include <cassert>
#include <cstdlib>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
SpeedTesterBase <TO>::SpeedTesterBase () noexcept
:	_src_arr ()
,	_dest_arr ()
,	_block_len (MAX_BLOCK_LEN)
,	_nbr_blocks (hiir_test_nbr_blocks)
{
	for (size_t buf_cnt = 0; buf_cnt < _src_arr.size (); ++buf_cnt)
	{
		Buffer &       buf = _src_arr [buf_cnt];
		for (size_t pos = 0; pos < buf.size (); ++pos)
		{
			const float    val =
				(float (rand ())) * float (2.0 / RAND_MAX) - 1.0f;
			buf [pos] = val;
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_SpeedTesterBase_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
