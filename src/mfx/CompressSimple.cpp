/*****************************************************************************

        CompressSimple.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/CompressSimple.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Returns the total size of the compressed frame (including the header)
size_t	CompressSimple::compress_frame (uint8_t *cmp_ptr, const uint8_t *raw_ptr, size_t len_raw)
{
	assert (cmp_ptr != nullptr);
	assert (raw_ptr != nullptr || len_raw == 0);
	assert (len_raw >= 0);

	uint8_t *      base_ptr = cmp_ptr;
	cmp_ptr += _frame_header_len;

	uint8_t *      org_ptr  = cmp_ptr;
	size_t         pos_seg  = 0;
	while (pos_seg < len_raw)
	{
		size_t         pos       = 0; // Relative to pos_seg
		bool           cont_flag = true;
		uint8_t        val_last  = raw_ptr [0];
		int            rep_len   = 0; // Length of the last seq of same bytes
		size_t         rem_len   = len_raw - pos_seg;
		bool           rep_flag  = true;
		do
		{
			// Byte is the same as the previous one
			if (raw_ptr [pos] == val_last)
			{
				++ rep_len;

				// We've decided to assume a copy-sequence and this byte was
				// repeated long enough: rewinds and terminates the sequence
				// just before the first byte of this constant sequence.
				if (! rep_flag && rep_len >= _rep_min)
				{
					pos -= rep_len - 1;
					cont_flag = false;
				}
			}

			// Byte is different
			else
			{
				// We're testing a repeat-sequence
				if (rep_flag)
				{
					// Greater than the minimum repeat-sequence size:
					// keeps it and stop here.
					if (rep_len >= _rep_min)
					{
						cont_flag = false;
					}
					// Too short: switches to a copy-sequence
					else
					{
						rep_flag = false;
					}
				}

				// Resets potential repeat-sequence
				val_last = raw_ptr [0];
				rep_len  = 1;
			}

			// Next byte
			if (cont_flag)
			{
				if (rep_flag && pos >= _rep_min + 127)
				{
					cont_flag = false;
				}
				else if (! rep_flag && pos >= 128)
				{
					cont_flag = false;
				}
				else
				{
					++ pos;
					if (pos >= rem_len)
					{
						cont_flag = false;
						if (rep_flag && pos < _rep_min)
						{
							rep_flag = false;
						}
					}
				}
			}
		}
		while (cont_flag);

		// Choice done, now encodes the sequence
		if (rep_flag)
		{
			assert (pos >= _rep_min);
			assert (pos <= _rep_min + 127);
			cmp_ptr [0] = uint8_t (pos - _rep_min);
			cmp_ptr [1] = raw_ptr [0];
			cmp_ptr += 2;
		}
		else
		{
			assert (pos >= 1);
			assert (pos <= 128);
			cmp_ptr [0] = uint8_t (pos + 127);
			for (size_t i = 0; i < pos; ++i)
			{
				cmp_ptr [1 + i] = raw_ptr [i];
			}
			cmp_ptr += 1 + pos;
		}

		raw_ptr += pos;
		pos_seg += pos;
	}

	*reinterpret_cast <uint32_t *> (base_ptr) = uint32_t (cmp_ptr - org_ptr);

	return size_t (cmp_ptr - base_ptr);
}



// Requires at least _frame_header_len bytes.
size_t	CompressSimple::read_compressed_frame_size (const uint8_t *cmp_ptr)
{
	assert (cmp_ptr != nullptr);

	return *reinterpret_cast <const uint32_t *> (cmp_ptr);
}



// Check read_compressed_frame_size() to know how long the cmp_ptr buffer
// should be.
size_t	CompressSimple::compute_raw_frame_size (const uint8_t *cmp_ptr)
{
	assert (cmp_ptr != nullptr);

	const size_t   frame_size = read_compressed_frame_size (cmp_ptr);
	cmp_ptr += _frame_header_len;

	const uint8_t* org_ptr = cmp_ptr;
	size_t         len     = 0;
	while (cmp_ptr - org_ptr < ptrdiff_t (frame_size))
	{
		len += compute_raw_seg_size (cmp_ptr);
	}

	assert (cmp_ptr - org_ptr == ptrdiff_t (frame_size));

	return len;
}



// Assumes raw_ptr points on a large enough memory area.
// Check compute_raw_frame_size ()
void	CompressSimple::decompress_frame (uint8_t *raw_ptr, const uint8_t *cmp_ptr)
{
	assert (raw_ptr != nullptr);
	assert (cmp_ptr != nullptr);

	const size_t   frame_size = read_compressed_frame_size (cmp_ptr);
	cmp_ptr += _frame_header_len;

	const uint8_t* org_ptr = cmp_ptr;

	while (cmp_ptr - org_ptr < ptrdiff_t (frame_size))
	{
		decompress_seg (raw_ptr, cmp_ptr);
	}

	assert (cmp_ptr - org_ptr == ptrdiff_t (frame_size));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	CompressSimple::decompress_seg (uint8_t * &raw_ptr, const uint8_t * &cmp_ptr)
{
	const int      count_code = *cmp_ptr;
	++ cmp_ptr;

	if (count_code < 128)
	{
		const int      len = count_code + _rep_min;
		const uint8_t  val = *cmp_ptr;
		++ cmp_ptr;

		for (int pos = 0; pos < len; ++pos)
		{
			raw_ptr [pos] = val;
		}
		raw_ptr += len;
	}
	else
	{
		const int      len = count_code - 127;
		for (int pos = 0; pos < len; ++pos)
		{
			raw_ptr [pos] = cmp_ptr [pos];
		}
		cmp_ptr += len;
		raw_ptr += len;
	}
}



size_t	CompressSimple::compute_raw_seg_size (const uint8_t * &cmp_ptr)
{
	size_t         len = 0;

	const int      count_code = *cmp_ptr;
	++ cmp_ptr;

	if (count_code < 128)
	{
		len = count_code + _rep_min;
		++ cmp_ptr;
	}
	else
	{
		len = count_code - 127;
		cmp_ptr += len;
	}

	return len;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
