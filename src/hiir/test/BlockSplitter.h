/*****************************************************************************

        BlockSplitter.h
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_BlockSplitter_HEADER_INCLUDED)
#define hiir_test_BlockSplitter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{
namespace test
{



class BlockSplitter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BlockSplitter (long max_block_len);
	virtual        ~BlockSplitter () = default;

	void           start (long total_len);
	bool           is_continuing () const;
	void           set_next_block ();

	long           get_pos () const;
	long           get_len () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {         NBR_LEN = 2 };

	long           _max_block_len;
	long           _total_len;
	long           _pos;

	long           _cur_len [NBR_LEN];
	int            _len_index;          // [0 ; NBR_LEN [



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BlockSplitter ()                           = delete;
	               BlockSplitter (const BlockSplitter &other) = delete;
	               BlockSplitter (BlockSplitter &&other)      = delete;
	BlockSplitter& operator = (const BlockSplitter &other)    = delete;
	BlockSplitter& operator = (BlockSplitter &&other)         = delete;
	bool           operator == (const BlockSplitter &other)   = delete;
	bool           operator != (const BlockSplitter &other)   = delete;

}; // class BlockSplitter



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_BlockSplitter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
