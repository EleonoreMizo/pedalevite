/*****************************************************************************

        SimulInterface.cpp
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



bool	SimulInterface::is_node_gnd (int node_idx) const
{
	assert (node_idx >= 0);

	return do_is_node_gnd (node_idx);
}



float	SimulInterface::get_voltage (int node_idx) const
{
	assert (node_idx >= 0);

	return do_get_voltage (node_idx);
}



float	SimulInterface::get_voltage (int n1_idx, int n2_idx) const
{
	assert (n1_idx >= 0);
	assert (n2_idx >= 0);

	return do_get_voltage (n1_idx, n2_idx);
}



void	SimulInterface::add_coef_mat (int row, int col, float val)
{
	assert (row >= 0);
	assert (col >= 0);

	do_add_coef_mat (row, col, val);
}



void	SimulInterface::add_coef_vec (int row, float val)
{
	assert (row >= 0);

	do_add_coef_vec (row, val);
}



void	SimulInterface::add_passive (int n1_idx, int n2_idx, float g)
{
	add_coef_mat (n1_idx, n1_idx, +g);
	add_coef_mat (n1_idx, n2_idx, -g);
	add_coef_mat (n2_idx, n1_idx, -g);
	add_coef_mat (n2_idx, n2_idx, +g);
}



void	SimulInterface::add_norton (int n1_idx, int n2_idx, float geq, float ieq)
{
	add_passive (n1_idx, n2_idx, geq);
	add_coef_vec (n1_idx, -ieq);
	add_coef_vec (n2_idx, +ieq);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
