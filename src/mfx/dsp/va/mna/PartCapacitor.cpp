/*****************************************************************************

        PartCapacitor.cpp
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

#include "mfx/dsp/va/mna/PartCapacitor.h"
#include "mfx/dsp/va/mna/SimulInterface.h"

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



PartCapacitor::PartCapacitor (IdNode nid_1, IdNode nid_2, float c)
:	_nid_arr {{ nid_1, nid_2 }}
,	_c (c)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_2 != nid_1);
	assert (c > 0);
}



void	PartCapacitor::set_capacity (float c)
{
	assert (c > 0);

	_c = c;
	if (_sample_freq > 0)
	{
		update_geq ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartCapacitor::do_get_info (SimulInterface &sim, PartInfo &info)
{
	_sim_ptr = &sim;

	info._nid_arr.clear ();
	for (auto &nid : _nid_arr)
	{
		info._nid_arr.push_back (nid);
	}
	info._nbr_src_v = 0;
}



void	PartCapacitor::do_prepare (const SimInfo &info)
{
	_sample_freq = float (info._sample_freq);
	assert (info._node_idx_arr.size () == _node_arr.size ());
	for (int pos = 0; pos < int (_node_arr.size ()); ++pos)
	{
		_node_arr [pos] = info._node_idx_arr [pos];
	}

	update_geq ();
}



void	PartCapacitor::do_add_to_matrix ()
{
	_sim_ptr->add_norton (_node_arr [0], _node_arr [1], _geq, _ieq);
}



void	PartCapacitor::do_step ()
{
	const float    v = _sim_ptr->get_voltage (_node_arr [0], _node_arr [1]);
	_ieq = -2 * _geq * v - _ieq;
}



void	PartCapacitor::do_clear_buffers ()
{
	_ieq = 0;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartCapacitor::update_geq ()
{
	assert (_sample_freq > 0);

	_geq = 2 * _c * _sample_freq;
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
