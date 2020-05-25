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



PartDiodeAntipar::PartDiodeAntipar (IdNode nid_1, IdNode nid_2, Flt is1, Flt n1, Flt is2, Flt n2)
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



void	PartDiodeAntipar::set_is (int dir, Flt is)
{
	assert (dir >= 0);
	assert (dir < int (_dir_arr.size ()));
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);

	Direction &    d = _dir_arr [dir];
	d._is    = (dir != 0) ? -is : is;
	d._vcrit = d.compute_vcrit ();
}



void	PartDiodeAntipar::set_n (int dir, Flt n)
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
	Flt            v   = _sim_ptr->get_voltage (_node_arr [0], _node_arr [1]);
	const auto &   dir = _dir_arr [(v < 0) ? 1 : 0];
	const Flt      is  = dir._is;
	const Flt      mv  = dir._mul_v;
	const Flt      nvi = dir._nvt_inv;

#if 1
	const Flt      e   = is * fstb::Approx::exp2 (float (v * mv));
#else
	const Flt      e   = is * Flt (exp (v / std::copysign (dir._n * dir._vt, is)));
#endif
	const Flt      s   = e - is;
	Flt            sd  = nvi * e;

	const Flt      geq = sd;
	const Flt      ieq = s - sd * v;

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



Flt	PartDiodeAntipar::Direction::compute_nvt_inv () const
{
	return std::copysign (Flt (1. / (_n * _vt)), _is);
}



Flt	PartDiodeAntipar::Direction::compute_mul_v () const
{
	return std::copysign (Flt (1. / (_n * _vt * fstb::LN2)), _is);
}



// (3.55) from http://qucs.sourceforge.net/tech/node16.html
Flt	PartDiodeAntipar::Direction::compute_vcrit () const
{
	const Flt      nvt = _n * _vt;
	const Flt      l2i = nvt / (fabs (_is) * Flt (fstb::SQRT2));
	const Flt      l2o = nvt * Flt (fstb::LN2);

	return l2o * fstb::Approx::log2 (float (l2i));
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
