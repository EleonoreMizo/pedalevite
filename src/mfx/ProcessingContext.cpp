/*****************************************************************************

        ProcessingContext.cpp
        Author: Laurent de Soras, 2019

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

#include "fstb/Crc32.h"
#include "mfx/ProcessingContext.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProcessingContext::compute_graph_crc (fstb::Crc32 &crc) const
{
	crc.reset ();

	// 8 bits are enough
	crc.process_byte (_nbr_chn_out);

	_interface_ctx.compute_graph_crc (crc);
	PluginContext::compute_graph_crc_mix_i (crc, _interface_mix);

	for (const auto &pi : _context_arr)
	{
		if (pi._mixer_flag)
		{
			pi.compute_graph_crc (crc);
		}
	}
}



void	ProcessingContext::PluginContext::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (auto &c : _pi_model)
	{
		crc.process_byte (c);
	}

	PluginContext::compute_graph_crc_mix_i (crc, _mix_in_arr);

	_node_arr [PiType_MAIN].compute_graph_crc (crc);
	if (_mixer_flag)
	{
		_node_arr [PiType_MIX].compute_graph_crc (crc);
	}
}



void	ProcessingContext::PluginContext::compute_graph_crc_mix_i (fstb::Crc32 &crc, const MixInputArray &mix_in_arr)
{
	if (! mix_in_arr.empty ())
	{
		for (const auto &chn : mix_in_arr)
		{
			for (const int buf : chn)
			{
				// Clipping to 8 bits is OK.
				crc.process_byte (buf);
			}
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
