/*****************************************************************************

        ProcessingContextNode.cpp
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

#include "fstb/Crc32.h"
#include "mfx/ProcessingContextNode.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ProcessingContextNode::ProcessingContextNode ()
:	_pi_id (-1)
,	_side_arr ()
,	_sig_buf_arr ()
,	_nbr_sig (0)
,	_aux_param_flag (false)
,	_comp_delay (0)
,	_pin_mult (1)
{
	for (auto & side : _side_arr)
	{
		side._nbr_chn     = 0;
		side._nbr_chn_tot = 0;
		for (auto & buf_index : side._buf_arr)
		{
			buf_index = -1;
		}
	}
}



void	ProcessingContextNode::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (const auto &side : _side_arr)
	{
		side.compute_graph_crc (crc);
	}
}



int &	ProcessingContextNode::Side::use_buf (int pin, int chn)
{
	assert (pin >= 0);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	const int      index = pin * _nbr_chn + chn;
	assert (index < _nbr_chn_tot);

	return _buf_arr [index];
}



const int &	ProcessingContextNode::Side::use_buf (int pin, int chn) const
{
	assert (pin >= 0);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	const int      index = pin * _nbr_chn + chn;
	assert (index < _nbr_chn_tot);

	return _buf_arr [index];
}



void	ProcessingContextNode::Side::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (int chn_cnt = 0; chn_cnt < _nbr_chn_tot; ++chn_cnt)
	{
		// The 8 first bits are enough.
		crc.process_byte (_buf_arr [chn_cnt]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
