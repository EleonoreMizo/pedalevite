/*****************************************************************************

        SplDataRetrievalInterface.cpp
        Author: Laurent de Soras, 2016

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

#include	"mfx/dsp/SplDataRetrievalInterface.h"

#include	<cassert>



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: get_data
Description:
	Retrieve sample data as separate channels. The number of channels is not
	specified or controlled here, it depends on the application. Anyway there
	should be at least one channel.
Input parameters:
	- pos: Block position within the whole file
	- len: Number of requested sample frames.
	- invert_flag: true if data has to be copied backward.
Output parameters:
	- chn_data_ptr_arr: Contains pointers on channel data to be filled.
Throws: Nothing
==============================================================================
*/

void	SplDataRetrievalInterface::get_data (float *chn_data_ptr_arr [], int64_t pos, int len, bool invert_flag)
{
	assert (chn_data_ptr_arr != nullptr);
	assert (chn_data_ptr_arr [0] != nullptr);
	assert (pos >= 0);
	assert (len > 0);

	do_get_data (chn_data_ptr_arr, pos, len, invert_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace dsp
}	// namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
