/*****************************************************************************

        PartSrcVoltage.cpp
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

#include "mfx/dsp/va/mna/PartSrcVoltage.h"
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



PartSrcVoltage::PartSrcVoltage (IdNode nid_1, IdNode nid_2, float v)
:	_nid_arr {{ nid_1, nid_2 }}
,	_v (v)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_2 != nid_1);
}



void	PartSrcVoltage::set_voltage (float v)
{
	_v = v;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartSrcVoltage::do_get_info (PartInfo &info) const
{
	info._nid_arr.clear ();
	for (auto &nid : _nid_arr)
	{
		info._nid_arr.push_back (nid);
	}
	info._nbr_src_v = 1;
}



void	PartSrcVoltage::do_prepare (SimulInterface &sim, const SimInfo &info)
{
	_sim_ptr = &sim;

	assert (info._node_idx_arr.size () == _node_arr.size ());
	for (int pos = 0; pos < int (_node_arr.size ()); ++pos)
	{
		_node_arr [pos] = info._node_idx_arr [pos];
	}
	_src_v_pos = info._src_v_idx_arr [0];
}



void	PartSrcVoltage::do_add_to_matrix ()
{
	_sim_ptr->add_coef_mat (_node_arr [0], _src_v_pos, +1);
	_sim_ptr->add_coef_mat (_src_v_pos, _node_arr [0], +1);
	_sim_ptr->add_coef_mat (_node_arr [1], _src_v_pos, -1);
	_sim_ptr->add_coef_mat (_src_v_pos, _node_arr [1], -1);

	_sim_ptr->add_coef_vec (_src_v_pos, _v);
}



void	PartSrcVoltage::do_step ()
{
	// Nothing
}



void	PartSrcVoltage::do_clear_buffers ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
