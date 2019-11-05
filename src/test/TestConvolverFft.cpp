/*****************************************************************************

        TestConvolverFft.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/fnc.h"
#include "mfx/dsp/fir/ConvolverFft.h"
#include "mfx/FileOpWav.h"
#include "test/TestConvolverFft.h"

#include <algorithm>
#include <vector>

#include <cassert>
#include <cstring>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	TestConvolverFft::perform_test ()
{
	using namespace mfx::dsp::fir;

	const int      src_len     = 8192;
	const int      impulse_len = 39;//129;
	const int      dest_len    = src_len + impulse_len - 1;

	ConvolverFft   convolver;

	// Build impulse
	{
		std::vector <float>  impulse (impulse_len, 0);
		const long		half_impulse_len = (impulse_len + 1) / 2;
		for (long pos = 0; pos < half_impulse_len; ++pos)
		{
			const float		val = float (pos + 1) / half_impulse_len;
			impulse [                  pos] = val;
			impulse [impulse_len - 1 - pos] = val;
		}

		convolver.set_impulse (impulse_len, &impulse [0]);
		assert (convolver.is_valid ());
	}

	const int      block_len = convolver.get_block_len ();

	// Build signal
	std::vector <float>  src (src_len, 0);
	{
		int            inc = 24;
		for (int pos = 0; pos < src_len / 2; pos += inc)
		{
			src [pos] = 1;
			inc += 5;
		}

		const int      div  = impulse_len / 2;
		float          val  = -(1.0f / div);
		float          step =  (4.0f / div) / src_len;
		for (int pos = src_len / 2; pos < src_len; ++pos)
		{
			src [pos] = val;
			val += step;
		}
	}

	// Destination signal
	std::vector <float>  dest (dest_len, 0);

	// Overlap-save convolution
	float * const        input_buf_ptr  = convolver.get_input_buffer ();
	const float * const  output_buf_ptr = convolver.get_output_buffer ();
	for (int block_pos = 0
	;	block_pos < dest_len
	;	block_pos += block_len)
	{
		// Copy input data and pad end with zeros
		const int      copy_src_len =
			fstb::limit (src_len - block_pos, 0, block_len);
		if (copy_src_len > 0)
		{
			memcpy (
				input_buf_ptr,
				&src [block_pos],
				copy_src_len * sizeof (*input_buf_ptr)
			);
		}
		const int      zero_len = block_len - copy_src_len;
		if (zero_len > 0)
		{
			memset (
				input_buf_ptr + copy_src_len,
				0,
				zero_len * sizeof (*input_buf_ptr)
			);
		}

		// Convolve
		convolver.process ();

		// Copy ouput data
		const int      copy_dest_len =
			std::min (dest_len - block_pos, block_len);
		memcpy (
			&dest [block_pos],
			output_buf_ptr,
			copy_dest_len * sizeof (dest [0])
		);
	}

	std::vector <std::vector <float> > file_content = { src, dest };
	file_content [0].resize (dest.size (), 0);
	mfx::FileOpWav::save (
		"results/convolverfft0.wav", file_content, 44100, 0.5
	);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
