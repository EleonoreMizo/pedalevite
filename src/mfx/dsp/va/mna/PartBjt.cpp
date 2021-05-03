/*****************************************************************************

        PartBjt.cpp
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
#include "mfx/dsp/va/mna/PartBjt.h"

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



PartBjt::PartBjt (IdNode nid_e, IdNode nid_b, IdNode nid_c, bool pnp_flag, Flt is, Flt beta_f, Flt beta_r) noexcept
:	_nid_e (nid_e)
,	_nid_b (nid_b)
,	_nid_c (nid_c)
,	_pnp_flag (pnp_flag)
,	_is (is)
,	_beta_f (beta_f)
,	_beta_r (beta_r)
,	_alpha_f (compute_alpha (_beta_f))
,	_alpha_r (compute_alpha (_beta_r))
,	_ise (compute_isx (_is, _alpha_f))
,	_isc (compute_isx (_is, _alpha_r))
{
	assert (nid_e >= 0);
	assert (nid_b >= 0);
	assert (nid_c >= 0);
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);
	assert (beta_f > 0);
	assert (beta_r >= 0);
}



void	PartBjt::set_is (Flt is) noexcept
{
	assert (is >= 1e-20f);
	assert (is <= 1e-1f);

	_is  = is;
	_ise = compute_isx (_is, _alpha_f);
	_isc = compute_isx (_is, _alpha_r);
}



void	PartBjt::set_beta_f (Flt beta) noexcept
{
	assert (beta > 0);

	_beta_f  = beta;
	_alpha_f = compute_alpha (_beta_f);
	_ise     = compute_isx (_is, _alpha_f);
	_cs_c_sptr->set_gain (_alpha_f);
}



void	PartBjt::set_beta_r (Flt beta) noexcept
{
	assert (beta >= 0);

	_beta_r  = beta;
	_alpha_r = compute_alpha (_beta_r);
	_isc     = compute_isx (_is, _alpha_r);
	_cs_e_sptr->set_gain (_alpha_r);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PartBjt::do_get_info (SimulInterface &sim, PartInfo &info)
{
	_sim_ptr = &sim;

	_nid_ei = sim.allocate_node ();
	_nid_ci = sim.allocate_node ();

	const IdNode   nid_ep = (_pnp_flag) ? _nid_e : _nid_b;
	const IdNode   nid_en = (_pnp_flag) ? _nid_b : _nid_e;
	const IdNode   nid_cp = (_pnp_flag) ? _nid_c : _nid_b;
	const IdNode   nid_cn = (_pnp_flag) ? _nid_b : _nid_c;

	_d_e_sptr  = std::make_shared <PartDiode> (_nid_ei, nid_en, _ise, 1.f);
	_cs_e_sptr = std::make_shared <PartCccs > (nid_cp, _nid_ci, nid_en, nid_ep, _alpha_r);
	_d_c_sptr  = std::make_shared <PartDiode> (_nid_ci, nid_cn, _isc, 1.f);
	_cs_c_sptr = std::make_shared <PartCccs > (nid_ep, _nid_ei, nid_cn, nid_cp, _alpha_f);

	info._subpart_arr.push_back (_d_e_sptr);
	info._subpart_arr.push_back (_d_c_sptr);
	info._subpart_arr.push_back (_cs_e_sptr);
	info._subpart_arr.push_back (_cs_c_sptr);
}



void	PartBjt::do_prepare (const SimInfo &info)
{
	fstb::unused (info);
}



void	PartBjt::do_add_to_matrix (int it_cnt) noexcept
{
	fstb::unused (it_cnt);

	// Nothing
}



void	PartBjt::do_step () noexcept
{
	// Nothing
}



void	PartBjt::do_clear_buffers () noexcept
{
	// Nothing
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
