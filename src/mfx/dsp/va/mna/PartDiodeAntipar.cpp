/*****************************************************************************

        PartDiodeAntipar.cpp
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
#include "fstb/Approx.h"
#include "mfx/dsp/va/mna/PartDiodeAntipar.h"

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



PartDiodeAntipar::PartDiodeAntipar (IdNode nid_1, IdNode nid_2, float is1, float n1, float is2, float n2)
:	_nid_arr {{ nid_1, nid_2 }}
{
	assert (nid_1 >= 0);
	assert (nid_2 >= 0);
	assert (nid_2 != nid_1);
	assert (is1 >= 1e-20f);
	assert (is1 <= 1e-1f);
	assert ( n1 > 0);
	assert (is2 >= 1e-20f);
	assert (is2 <= 1e-1f);
	assert ( n2 > 0);

	_dir_arr [0]._is = +is1;
	_dir_arr [0]._n  =  n1;
	_dir_arr [1]._is = -is2;
	_dir_arr [1]._n  =  n2;
	for (auto &d : _dir_arr)
	{
		d._nvt_inv = d.compute_nvt_inv ();
		d._mul_v   = d.compute_mul_v ();
		d._vcrit   = d.compute_vcrit ();
	}
}



void	PartDiodeAntipar::set_is (int dir, float is)
{
	assert (dir >= 0);
	assert (dir < int (_dir_arr.size ()));
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);

	Direction &    d = _dir_arr [dir];
	d._is    = (dir != 0) ? -is : is;
	d._vcrit = d.compute_vcrit ();
}



void	PartDiodeAntipar::set_n (int dir, float n)
{
	assert (dir >= 0);
	assert (dir < int (_dir_arr.size ()));
	assert (n > 0);

	Direction &    d = _dir_arr [dir];
	d._n       = n;
	d._nvt_inv = d.compute_nvt_inv ();
	d._mul_v   = d.compute_mul_v ();
	d._vcrit   = d.compute_vcrit ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartDiodeAntipar::do_get_info (SimulInterface &sim, PartInfo &info)
{
	_sim_ptr = &sim;

	info._nid_arr.clear ();
	for (auto &nid : _nid_arr)
	{
		info._nid_arr.push_back (nid);
	}
	info._nbr_src_v       = 0;
	info._non_linear_flag = true;
}



void	PartDiodeAntipar::do_prepare (const SimInfo &info)
{
	assert (info._node_idx_arr.size () == _node_arr.size ());
	for (int pos = 0; pos < int (_node_arr.size ()); ++pos)
	{
		_node_arr [pos] = info._node_idx_arr [pos];
	}
}



void	PartDiodeAntipar::do_add_to_matrix ()
{
	const float    v   = _sim_ptr->get_voltage (_node_arr [0], _node_arr [1]);
	const auto &   dir = _dir_arr [(v < 0) ? 1 : 0];
	const float    is  = dir._is;
	const float    mv  = dir._mul_v;
	const float    nvi = dir._nvt_inv;

	const float    e   = is * fstb::Approx::exp2 (v * mv);
	const float    s   = e - is;
	const float    sd  = nvi * e;

	const float    geq = sd;
	const float    ieq = s - sd * v;

	_sim_ptr->add_norton (_node_arr [0], _node_arr [1], geq, ieq);
}



void	PartDiodeAntipar::do_step ()
{
	// Nothing
}



void	PartDiodeAntipar::do_clear_buffers ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	PartDiodeAntipar::Direction::compute_nvt_inv () const
{
	return std::copysign (float (1.0 / (_n * _vt)), _is);
}



float	PartDiodeAntipar::Direction::compute_mul_v () const
{
	return std::copysign (float (1.0 / (_n * _vt * fstb::LN2)), _is);
}



float	PartDiodeAntipar::Direction::compute_vcrit () const
{
	const float    nvt = _n * _vt;
	const float    l2i = nvt / (fabs (_is) * float (fstb::SQRT2));
	const float    l2o = nvt * float (fstb::LN2);

	return l2o * fstb::Approx::log2 (l2i);
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
