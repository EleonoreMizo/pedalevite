/*****************************************************************************

        CompressSimple.h
        Author: Laurent de Soras, 2020

Frame format
------------
uint32  frame_size
[block0] [block1] ... [blockN-1]

frame_size is the sum of the compressed block sizes (not including this field).
If frame_size is 0, there are no block and the uncompressed data size is
assumed to be 0.

Block format
------------
uint8    seg_code
[segment0]... [segmentN-1]

Number of segments = seg_code + 1

Segment format
--------------
uint8   count
[payload]

If count >= 128, payload = count - 127 raw uncompressed bytes
If count < 128, payload = 1 byte, repeated count + 3

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_CompressSimple_HEADER_INCLUDED)
#define mfx_CompressSimple_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{



class CompressSimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const size_t _frame_header_len = sizeof (uint32_t);

	static size_t  compress_frame (uint8_t *cmp_ptr, const uint8_t *raw_ptr, size_t len_raw);
	static size_t  read_compressed_frame_size (const uint8_t *cmp_ptr);
	static size_t  compute_raw_frame_size (const uint8_t *cmp_ptr);
	static void    decompress_frame (uint8_t *raw_ptr, const uint8_t *cmp_ptr);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _rep_min = 3;

	static void    decompress_seg (uint8_t * &raw_ptr, const uint8_t * &cmp_ptr);
	static size_t  compute_raw_seg_size (const uint8_t * &cmp_ptr);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               CompressSimple ()                               = delete;
	               CompressSimple (const CompressSimple &other)    = delete;
	               ~CompressSimple ()                              = delete;
	               CompressSimple (CompressSimple &&other)         = delete;
	CompressSimple &
	               operator = (const CompressSimple &other)        = delete;
	CompressSimple &
	               operator = (CompressSimple &&other)             = delete;
	bool           operator == (const CompressSimple &other) const = delete;
	bool           operator != (const CompressSimple &other) const = delete;

}; // class CompressSimple



}  // namespace mfx



//#include "mfx/CompressSimple.hpp"



#endif   // mfx_CompressSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

