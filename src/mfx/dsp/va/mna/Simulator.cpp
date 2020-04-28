/*****************************************************************************

        Simulator.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/va/mna/Simulator.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Simulator::add_part (PartSPtr part_sptr)
{
	assert (part_sptr.get () != nullptr);
	assert (std::find (
		_part_arr.begin (), _part_arr.end (), part_sptr
	) == _part_arr.end ());

	_part_arr.push_back (part_sptr);
}



void	Simulator::prepare (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);

	_nid_to_idx_map.clear ();
	_nbr_nodes = 0;
	_nbr_src_v = 0;
	_idx_gnd   = 0;
	_nl_flag   = false;

	const int      nbr_parts = int (_part_arr.size ());
	std::vector <PartInterface::PartInfo>  part_info_list (nbr_parts);
	std::vector <PartInterface::SimInfo>   sim_info_list (nbr_parts);
	const IdNode   idx_gnd_tmp = -999666;

	// First pass to collect information and compute some indexes
	for (int part_cnt = 0; part_cnt < nbr_parts; ++part_cnt)
	{
		PartInterface &   part = *_part_arr [part_cnt];

		// Collects information
		PartInterface::PartInfo &  part_info = part_info_list [part_cnt];
		part.get_info (part_info);

		_nl_flag |= part_info._non_linear_flag;

		PartInterface::SimInfo &   sim_info  = sim_info_list [part_cnt];
		sim_info._node_idx_arr.resize (part_info._nid_arr.size ());
		sim_info._src_v_idx_arr.resize (part_info._nbr_src_v);

		// Assigns matrix indexes
		for (int k = 0; k < int (part_info._nid_arr.size ()); ++k)
		{
			const IdNode   nid = part_info._nid_arr [k];
			int            idx = idx_gnd_tmp;
			if (nid != _nid_gnd)
			{
				auto           it  = _nid_to_idx_map.find (nid);
				if (it != _nid_to_idx_map.end ())
				{
					idx = it->second;
				}
				else
				{
					// Not found? Creates a new node
					idx = _nbr_nodes;
					_nid_to_idx_map [nid] = idx;
					++ _nbr_nodes;
				}
			}
			sim_info._node_idx_arr [k] = idx;
		}

		// Assigns temporary independant voltage source indexes
		for (int k = 0; k < part_info._nbr_src_v; ++k)
		{
			sim_info._src_v_idx_arr [k] = _nbr_src_v;
			++ _nbr_src_v;
		}
	}

	// Matrix size
	_msize   = _nbr_nodes + _nbr_src_v;
	_idx_gnd = _msize;

	// Creates the matrices
	_mat_a.resize (_msize, _msize);
	_vec_z.resize (_msize);
	_vec_x.resize (_msize);
	_vec_x_old.resize (_msize);

	// Finalizes indexes and sends information back to the parts
	for (int part_cnt = 0; part_cnt < nbr_parts; ++part_cnt)
	{
		PartInterface::SimInfo &   sim_info  = sim_info_list [part_cnt];
		sim_info._sample_freq = sample_freq;

		for (auto &idx : sim_info._node_idx_arr)
		{
			if (idx == idx_gnd_tmp)
			{
				idx = _idx_gnd;
			}
		}

		for (auto &idx : sim_info._src_v_idx_arr)
		{
			idx += _nbr_nodes;
		}

		PartInterface &   part = *_part_arr [part_cnt];
		part.prepare (*this, sim_info);
	}

	// Starts with a fresh state
	clear_buffers ();
}



void	Simulator::process_sample ()
{
#if defined (mfx_dsp_va_mna_Simulator_STATS)
	_st_nbr_it = 0;
	_st_nbr_f0 = 0;
	_st_nbr_f1 = 0;
#endif

	if (! _nl_flag)
	{
		build_matrix ();
		_qr.compute (_mat_a);
		_vec_x = _qr.solve (_vec_z);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
		_st_nbr_it = 1;
#endif
	}
	else
	{
		// Iterates to solve non-linear stuff
		const float    tol_abs   = 1e-6f;
		const float    tol_rel   = 1e-3f;
		bool           cont_flag = false;
		int            nbr_it    = 0;
		do
		{
			_vec_x_old = _vec_x;

			build_matrix ();
			_qr.compute (_mat_a);
			_vec_x = _qr.solve (_vec_z);

			// Limitation and convergence calculation
			cont_flag = false;
			for (int v_cnt = 0; v_cnt < _nbr_nodes; ++v_cnt)
			{
				float          v_cur = _vec_x (v_cnt);
				const float    v_old = _vec_x_old (v_cnt);
				v_cur = fstb::limit (v_cur, v_old - _max_dif, v_old + _max_dif);
				_vec_x (v_cnt) = v_cur;

				const float    dif  = v_cur - v_old;
				const float    difa = fabs (dif);
				cont_flag |= (difa >= tol_abs + tol_rel * fabs (v_cur));
			}

			++ nbr_it;
		}
		while (cont_flag && nbr_it < _max_it);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
		_st_nbr_it = nbr_it;
#endif
	}

	// Integration step
	for (auto &part_sptr : _part_arr)
	{
		part_sptr->step ();
	}

#if defined (mfx_dsp_va_mna_Simulator_STATS)
	++ _st._hist_it [_st_nbr_it];
	++ _st._hist_f0 [_st_nbr_f0];
	++ _st._hist_f1 [_st_nbr_f1];
	++ _st._nbr_spl_proc;
#endif
}



float	Simulator::get_node_voltage (IdNode nid) const
{
	assert (nid >= 0);

	float          v = 0;

	if (nid != _nid_gnd)
	{
		const auto     it = _nid_to_idx_map.find (nid);
		assert (it != _nid_to_idx_map.end ());
		const int      idx = it->second;
		v = _vec_x (idx);
	}

	return v;
}



void	Simulator::clear_buffers ()
{
	for (auto &part_sptr : _part_arr)
	{
		part_sptr->clear_buffers ();
	}
	_vec_x.fill (0);
	_vec_x_old.fill (0);
}



#if defined (mfx_dsp_va_mna_Simulator_STATS)

void	Simulator::reset_stats ()
{
	_st._hist_it.fill (0);
	_st._hist_f0.fill (0);
	_st._hist_f1.fill (0);
	_st._nbr_spl_proc = 0;
}



void	Simulator::get_stats (Stats &stats) const
{
	stats = _st;
}

#endif // mfx_dsp_va_mna_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Simulator::do_is_node_gnd (int node_idx) const
{
	return (node_idx == _idx_gnd);
}



float	Simulator::do_get_voltage (int node_idx) const
{
	assert (node_idx < _nbr_nodes || is_node_gnd (node_idx));

	float          v = 0;
	if (! is_node_gnd (node_idx))
	{
		v = _vec_x (node_idx);
	}

	return v;
}



float	Simulator::do_get_voltage (int n1_idx, int n2_idx) const
{
	const float    v1 = get_voltage (n1_idx);
	const float    v2 = get_voltage (n2_idx);
	const float    v  = v1 - v2;

	return v;
}



void	Simulator::do_add_coef_mat (int row, int col, float val)
{
	assert (row < _msize || row == _idx_gnd);
	assert (col < _msize || col == _idx_gnd);

	if (row != _idx_gnd && col != _idx_gnd)
	{
		_mat_a (row, col) += val;
	}
}



void	Simulator::do_add_coef_vec (int row, float val)
{
	assert (row < _msize || row == _idx_gnd);

	if (row != _idx_gnd)
	{
		_vec_z (row) += val;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Simulator::build_matrix ()
{
	_mat_a.fill (0);
	_vec_z.fill (0);

	for (auto &part_sptr : _part_arr)
	{
		part_sptr->add_to_matrix ();
	}
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
