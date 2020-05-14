/*****************************************************************************

        PartBjtNpn.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "mfx/dsp/va/mna/PartBjtNpn.h"

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



PartBjtNpn::PartBjtNpn (IdNode nid_e, IdNode nid_b, IdNode nid_c, float is, float beta_f, float beta_r)
:	_nid_e (nid_e)
,	_nid_b (nid_b)
,	_nid_c (nid_c)
,	_is (is)
,	_beta_f (beta_f)
,	_beta_r (beta_r)
{
	assert (nid_e >= 0);
	assert (nid_b >= 0);
	assert (nid_c >= 0);
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);
	assert (beta_f > 0);
	assert (beta_r >= 0);

	compute_param ();
}



void	PartBjtNpn::set_is (float is)
{
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);

	_is  = is;
}



void	PartBjtNpn::set_beta_f (float beta)
{
	assert (beta > 0);

	_beta_f  = beta;
	compute_param ();
}



void	PartBjtNpn::set_beta_r (float beta)
{
	assert (beta >= 0);

	_beta_r  = beta;
	compute_param ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartBjtNpn::do_get_info (SimulInterface &sim, PartInfo &info)
{
	_sim_ptr = &sim;

	info._nid_arr.clear ();
	info._nid_arr.push_back (_nid_e);
	info._nid_arr.push_back (_nid_b);
	info._nid_arr.push_back (_nid_c);
	info._nbr_src_v       = 0;
	info._non_linear_flag = true;
}



void	PartBjtNpn::do_prepare (const SimInfo &info)
{
	assert (info._node_idx_arr.size () == 3);
	_idx_e = info._node_idx_arr [0];
	_idx_b = info._node_idx_arr [1];
	_idx_c = info._node_idx_arr [2];
}



void	PartBjtNpn::do_add_to_matrix ()
{
	// Retrieves the voltages
	const float    ve   = _sim_ptr->get_voltage (_idx_e);
	const float    vb   = _sim_ptr->get_voltage (_idx_b);
	const float    vc   = _sim_ptr->get_voltage (_idx_c);
	const float    vbe  = vb - ve;
	const float    vbc  = vb - vc;
	const float    vce  = vc - ve;

	// Computes the exp parts of the currents
	const float    vbem = vbe * _mul_v;
	const float    vbcm = vbc * _mul_v;
	const float    pbe  =
		(vbem <= -127.f) ? 0.f : _is * fstb::Approx::exp2 (vbem);
	const float    pbc  =
		(vbcm <= -127.f) ? 0.f : _is * fstb::Approx::exp2 (vbcm);

	// Currents
	const float    i_f  = pbe - _is;       // 10.84
	const float    i_r  = pbc - _is;       // 10.89
	const float    i_t  = i_f - i_r;       // 10.95 w/ QB = 1.
	const float    ibei = i_f * _bf_inv;   // 10.85
	const float    ibci = i_r * _br_inv;   // 10.90
	const float    ibe  = ibei;            // 10.87
	const float    ibc  = ibci;            // 10.92

	// Current derivatives
	const float    gbei = _ni_o_bf * pbe;  // 10.85
	const float    gbci = _ni_o_br * pbc;  // 10.90
	const float    gpi  = gbei;            // 10.88
	const float    gmu  = gbci;            // 10.93
	const float    gif  = gbei * _beta_f;  // 10.100
	const float    gir  = gbci * _beta_r;  // 10.101
	const float    gmf  = +gif;            // 10.98
	const float    gmr  = -gir;            // 10.99

	// Linear companinon circuits
	const float    ibeeq = ibe - gpi * vbe;               // 10.107
	const float    ibceq = ibc - gmu * vbc;               // 10.108
	const float    iceeq = i_t - gmf * vbe + gmr * vbc;   // 10.109

	_sim_ptr->add_coef_mat (_idx_b, _idx_b, +gpi +gmu          );
	_sim_ptr->add_coef_mat (_idx_b, _idx_c,      -gmu          );
	_sim_ptr->add_coef_mat (_idx_b, _idx_e, -gpi               );
	_sim_ptr->add_coef_mat (_idx_c, _idx_b,      -gmu +gmf -gmr);
	_sim_ptr->add_coef_mat (_idx_c, _idx_c,      +gmu      +gmr);
	_sim_ptr->add_coef_mat (_idx_c, _idx_e,           -gmf     );
	_sim_ptr->add_coef_mat (_idx_e, _idx_b, -gpi      -gmf +gmr);
	_sim_ptr->add_coef_mat (_idx_e, _idx_c,                -gmr);
	_sim_ptr->add_coef_mat (_idx_e, _idx_e, +gpi      +gmf     );

	_sim_ptr->add_coef_vec (_idx_b, -ibeeq - ibceq        );
	_sim_ptr->add_coef_vec (_idx_c,        + ibceq - iceeq);
	_sim_ptr->add_coef_vec (_idx_e, +ibeeq         + iceeq);
}



void	PartBjtNpn::do_step ()
{
	// Nothing
}



void	PartBjtNpn::do_clear_buffers ()
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartBjtNpn::compute_param ()
{
	_bf_inv  = 1.f / _beta_f;
	_br_inv  = 1.f / _beta_r;
	_ni_o_bf = _nvt_inv * _bf_inv;
	_ni_o_br = _nvt_inv * _br_inv;
}



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
