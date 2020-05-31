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
#include "mfx/dsp/va/mna/PartSrcVoltage.h"
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



// Do not add anything after this call
void	Simulator::prepare (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = Flt (sample_freq);

	_nid_to_idx_map.clear ();
	_nbr_nodes = 0;
	_nbr_src_v = 0;
	_idx_gnd   = 0;
	_nl_flag   = false;

	std::vector <PartInterface::PartInfo>  part_info_list;
	std::vector <PartInterface::SimInfo>   sim_info_list;
	const int      idx_gnd_tmp = -666999666;

	// First pass to collect information and compute some indexes
	// _part_arr.size () not cached because _part_arr can grow within the loop
	for (int part_cnt = 0; part_cnt < int (_part_arr.size ()); ++part_cnt)
	{
		PartInterface &   part = *_part_arr [part_cnt];

		// Collects information
		PartInterface::PartInfo part_info;
		part.get_info (*this, part_info);

		// Sub-parts
		for (auto &subpart_sptr : part_info._subpart_arr)
		{
			add_part (subpart_sptr);
		}

		_nl_flag |= part_info._non_linear_flag;

		PartInterface::SimInfo  sim_info;
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

		part_info_list.push_back (part_info);
		sim_info_list.push_back (sim_info);
	}

	assert (part_info_list.size () == _part_arr.size ());
	assert (sim_info_list.size () == _part_arr.size ());

	// Matrix size
	_msize   = _nbr_nodes + _nbr_src_v;
	_idx_gnd = _msize;

	_known_voltage_arr.assign (_nbr_nodes, false);

	// Creates the matrices
	_mat_a.resize (_msize, _msize);
	_vec_z.resize (_msize);
	_vec_x.resize (_msize);
	_vec_x_old.resize (_msize);

	// Finalizes indexes and sends information back to the parts
	const int      nbr_parts = int (_part_arr.size ());
	for (int part_cnt = 0; part_cnt < nbr_parts; ++part_cnt)
	{
		PartInterface::SimInfo &   sim_info  = sim_info_list [part_cnt];
		sim_info._sample_freq = sample_freq;
		bool           gnd_flag = false;

		for (auto &idx : sim_info._node_idx_arr)
		{
			if (idx == idx_gnd_tmp)
			{
				idx      = _idx_gnd;
				gnd_flag = true;
			}
		}

		for (auto &idx : sim_info._src_v_idx_arr)
		{
			idx += _nbr_nodes;
		}

		PartInterface &   part = *_part_arr [part_cnt];
		part.prepare (sim_info);

		// Checks if there are nodes with known voltages, too
		if (gnd_flag && dynamic_cast <PartSrcVoltage *> (&part) != 0)
		{
			for (const auto &idx : sim_info._node_idx_arr)
			{
				if (idx != _idx_gnd)
				{
					assert (! _known_voltage_arr [idx]);
					_known_voltage_arr [idx] = true;
				}
			}
		}
	}

	// Starts with a fresh state
	clear_buffers ();
}



void	Simulator::process_sample ()
{
#if defined (mfx_dsp_va_mna_Simulator_STATS)
	_st_nbr_it = 0;
#endif // mfx_dsp_va_mna_Simulator_STATS

	if (! _nl_flag)
	{
		build_matrix (0);
		_decomp.compute (_mat_a);
		_vec_x = _decomp.solve (_vec_z);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
		_st_nbr_it = 1;
#endif // mfx_dsp_va_mna_Simulator_STATS
	}
	else
	{
		// Iterates to solve non-linear stuff
		const Flt      tol_abs   = Flt (1e-6);
		const Flt      tol_rel   = Flt (1e-3);
		bool           cont_flag = false;
		int            nbr_it    = 0;
		do
		{
			_vec_x_old = _vec_x;

			build_matrix (nbr_it);
			_decomp.compute (_mat_a);
			_vec_x = _decomp.solve (_vec_z);

			// Limitation and convergence calculation
			cont_flag = false;
			for (int v_cnt = 0; v_cnt < _nbr_nodes; ++v_cnt)
			{
				if (! _known_voltage_arr [v_cnt])
				{
					Flt            v_cur = _vec_x (v_cnt);
					const Flt      v_old = _vec_x_old (v_cnt);
					assert (std::isfinite (v_cur) && fabs (v_cur) < 1e6f);
					v_cur = fstb::limit (v_cur, v_old - _max_dif, v_old + _max_dif);
					_vec_x (v_cnt) = v_cur;

					const Flt      dif  = v_cur - v_old;
					const Flt      difa = fabs (dif);
					cont_flag |= (difa >= tol_abs && difa >= tol_rel * fabs (v_cur));
				}
			}

			++ nbr_it;
		}
		while (cont_flag && nbr_it < _max_it);
#if defined (mfx_dsp_va_mna_Simulator_STATS)
		_st_nbr_it = nbr_it;
#endif // mfx_dsp_va_mna_Simulator_STATS
	}

	// Integration step
	for (auto &part_sptr : _part_arr)
	{
		part_sptr->step ();
	}

#if defined (mfx_dsp_va_mna_Simulator_STATS)
	++ _st._hist_it [_st_nbr_it];
	++ _st._nbr_spl_proc;
#endif // mfx_dsp_va_mna_Simulator_STATS
}



Flt	Simulator::get_node_voltage (IdNode nid) const
{
	assert (nid >= 0);

	Flt            v = 0;

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



int	Simulator::get_matrix_size () const
{
	return _msize;
}



int	Simulator::get_nbr_nodes () const
{
	return _nbr_nodes;
}



int	Simulator::get_nbr_src_v () const
{
	return _nbr_src_v;
}



std::vector <Flt>	Simulator::get_matrix () const
{
	std::vector <Flt> mat;

	for (int y = 0; y < _msize; ++y)
	{
		for (int x = 0; x < _msize; ++x)
		{
			mat.push_back (_mat_a (y, x));
		}
	}

	return mat;
}



std::vector <Flt>	Simulator::get_vector () const
{
	std::vector <Flt> vec;

	for (int x = 0; x < _msize; ++x)
	{
		vec.push_back (_vec_z (x));
	}

	return vec;
}



#if defined (mfx_dsp_va_mna_Simulator_STATS)

void	Simulator::reset_stats ()
{
	_st._hist_it.fill (0);
	_st._nbr_spl_proc = 0;
}



void	Simulator::get_stats (Stats &stats) const
{
	stats = _st;
}

#endif // mfx_dsp_va_mna_Simulator_STATS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PartInterface::IdNode	Simulator::do_allocate_node ()
{
	const IdNode   nid = _nid_int_cur;
	++ _nid_int_cur;

	return nid;
}



bool	Simulator::do_is_node_gnd (int node_idx) const
{
	return (node_idx == _idx_gnd);
}



Flt	Simulator::do_get_voltage (int node_idx) const
{
	assert (node_idx < _nbr_nodes || is_node_gnd (node_idx));

	Flt            v = 0;
	if (! is_node_gnd (node_idx))
	{
		v = _vec_x (node_idx);
	}

	return v;
}



Flt	Simulator::do_get_voltage (int n1_idx, int n2_idx) const
{
	const Flt      v1 = get_voltage (n1_idx);
	const Flt      v2 = get_voltage (n2_idx);
	const Flt      v  = v1 - v2;

	return v;
}



void	Simulator::do_add_coef_mat (int row, int col, Flt val)
{
	assert (row < _msize || row == _idx_gnd);
	assert (col < _msize || col == _idx_gnd);

	if (row != _idx_gnd && col != _idx_gnd)
	{
		_mat_a (row, col) += val;
	}
}



void	Simulator::do_add_coef_vec (int row, Flt val)
{
	assert (row < _msize || row == _idx_gnd);

	if (row != _idx_gnd)
	{
		_vec_z (row) += val;
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Simulator::build_matrix (int it_cnt)
{
	_mat_a.fill (0);
	_vec_z.fill (0);

	for (auto &part_sptr : _part_arr)
	{
		part_sptr->add_to_matrix (it_cnt);
	}
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
