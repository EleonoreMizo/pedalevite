/*****************************************************************************

        PartInterface.cpp
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

#include "mfx/dsp/va/mna/PartInterface.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartInterface::get_info (SimulInterface &sim, PartInfo &info)
{
	assert (info._nid_arr.empty ());
	assert (info._nbr_src_v == 0);
	assert (! info._non_linear_flag);

	do_get_info (sim, info);

	assert (info._subpart_arr.empty () || info._subpart_arr.front ().get () != nullptr);
	assert (info._subpart_arr.empty () || info._subpart_arr.back ().get ()  != nullptr);
	assert (info._nid_arr.empty () || info._nid_arr.front () >= 0);
	assert (info._nid_arr.empty () || info._nid_arr.back ()  >= 0);
	assert (info._nbr_src_v >= 0);
}



void	PartInterface::prepare (const SimInfo &info)
{
	assert (info._sample_freq > 0);
	assert (info._node_idx_arr.empty () || info._node_idx_arr.front () >= 0);
	assert (info._node_idx_arr.empty () || info._node_idx_arr.back ()  >= 0);

	do_prepare (info);
}



void	PartInterface::add_to_matrix (int it_cnt)
{
	assert (it_cnt >= 0);

	do_add_to_matrix (it_cnt);
}



void	PartInterface::step ()
{
	do_step ();
}



void	PartInterface::clear_buffers ()
{
	do_clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
