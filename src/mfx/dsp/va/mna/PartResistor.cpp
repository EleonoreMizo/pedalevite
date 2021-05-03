/*****************************************************************************

        PartResistor.cpp
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

#include "fstb/def.h"
#include "mfx/dsp/va/mna/PartResistor.h"
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



PartResistor::PartResistor (IdNode nid_1, IdNode nid_2, Flt r) noexcept
:	_nid_arr {{ nid_1, nid_2 }}
,	_r (r)
,	_g (1.f / r)
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_2 != nid_1);
	assert (r > 0);	
}



void	PartResistor::set_resistance (Flt r) noexcept
{
	assert (r > 0);

	_r = r;
	_g = Flt (1. / r);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartResistor::do_get_info (SimulInterface &sim, PartInfo &info)
{
	_sim_ptr = &sim;

	info._nid_arr.clear ();
	for (auto &nid : _nid_arr)
	{
		info._nid_arr.push_back (nid);
	}
	info._nbr_src_v = 0;
}



void	PartResistor::do_prepare (const SimInfo &info)
{
	assert (info._node_idx_arr.size () == _node_arr.size ());
	for (int pos = 0; pos < int (_node_arr.size ()); ++pos)
	{
		_node_arr [pos] = info._node_idx_arr [pos];
	}
}



void	PartResistor::do_add_to_matrix (int it_cnt) noexcept
{
	fstb::unused (it_cnt);

	_sim_ptr->add_coef_mat (_node_arr [0], _node_arr [0], +_g);
	_sim_ptr->add_coef_mat (_node_arr [0], _node_arr [1], -_g);
	_sim_ptr->add_coef_mat (_node_arr [1], _node_arr [0], -_g);
	_sim_ptr->add_coef_mat (_node_arr [1], _node_arr [1], +_g);
}



void	PartResistor::do_step () noexcept
{
	// Nothing
}



void	PartResistor::do_clear_buffers () noexcept
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
