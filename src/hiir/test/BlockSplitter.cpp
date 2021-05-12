/*****************************************************************************

        BlockSplitter.cpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/BlockSplitter.h"

#include <cassert>



namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BlockSplitter::BlockSplitter (long max_block_len)
:	_max_block_len (max_block_len)
,	_total_len (0)
,	_pos (0)
/*,	_cur_len ()*/
,	_len_index (0)
{
	assert (max_block_len > 0);
}



void	BlockSplitter::start (long total_len)
{
	assert (total_len > 0);

	_total_len = total_len;
	_pos       = 0;
	for (int i = 0; i < NBR_LEN; ++i)
	{
		_cur_len [i] = 1;
	}
	_len_index = 0;
}



bool	BlockSplitter::is_continuing () const
{
	return (_pos < _total_len);
}



void	BlockSplitter::set_next_block ()
{
	_pos += get_len ();

	++ _len_index;
	if (_len_index >= NBR_LEN)
	{
		_len_index = 0;
		bool           cont_flag = true;
		for (int i = 0; i < NBR_LEN && cont_flag; ++i)
		{
			++ _cur_len [i];
			if (_cur_len [i] > _max_block_len)
			{
				_cur_len [i] = 1;
			}
			else
			{
				cont_flag = false;
			}
		}
	}
}



long	BlockSplitter::get_pos () const
{
	return _pos;
}



long	BlockSplitter::get_len () const
{
	long           len = _cur_len [_len_index];
	if (_pos + len > _total_len)
	{
		len = _total_len - _pos;
	}
	assert (len > 0);

	return len;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
