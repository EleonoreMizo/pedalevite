/*****************************************************************************

        PartBjt.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartBjt_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartBjt_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/mna/PartCccs.h"
#include "mfx/dsp/va/mna/PartDiode.h"
#include "mfx/dsp/va/mna/PartInterface.h"
#include "mfx/dsp/va/mna/SimulInterface.h"

#include <array>
#include <memory>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



class PartBjt
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartBjt (IdNode nid_e, IdNode nid_b, IdNode nid_c, bool pnp_flag, Flt is, Flt beta_f, Flt beta_r);
	virtual        ~PartBjt () = default;

	void           set_is (Flt is);
	void           set_beta_f (Flt beta);
	void           set_beta_r (Flt beta);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PartInterface
	void           do_get_info (SimulInterface &sim, PartInfo &info) final;
	void           do_prepare (const SimInfo &info) final;
	void           do_add_to_matrix () final;
	void           do_step () final;
	void           do_clear_buffers () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::shared_ptr <PartDiode> DiodeSPtr;
	typedef std::shared_ptr <PartCccs> CccsSPtr;

	static Flt     compute_alpha (Flt beta) { return beta / (1.f + beta); }
	static Flt     compute_isx (Flt is, Flt alpha) { return is / alpha; }

	IdNode         _nid_e    = _nid_invalid;
	IdNode         _nid_b    = _nid_invalid;
	IdNode         _nid_c    = _nid_invalid;
	IdNode         _nid_ei   = _nid_invalid; // Intermediate nodes to get the
	IdNode         _nid_ci   = _nid_invalid; // current flowing through the diodes

	bool           _pnp_flag = false;
	Flt            _is       = Flt (1e-15);
	Flt            _beta_f   = Flt (200.);
	Flt            _beta_r   = Flt (1.);

	SimulInterface *
	               _sim_ptr  = nullptr;

	DiodeSPtr      _d_e_sptr;
	CccsSPtr       _cs_e_sptr;
	DiodeSPtr      _d_c_sptr;
	CccsSPtr       _cs_c_sptr;

	PartInfo       _info_d_e;
	PartInfo       _info_d_c;
	PartInfo       _info_cs_e;
	PartInfo       _info_cs_c;

	// Precomputed values
	Flt            _alpha_f  = compute_alpha (_beta_f);
	Flt            _alpha_r  = compute_alpha (_beta_r);
	Flt            _ise      = compute_isx (_is, _alpha_f);
	Flt            _isc      = compute_isx (_is, _alpha_r);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartBjt ()                               = delete;
	               PartBjt (const PartBjt &other)           = delete;
	               PartBjt (PartBjt &&other)                = delete;
	PartBjt &      operator = (const PartBjt &other)        = delete;
	PartBjt &      operator = (PartBjt &&other)             = delete;
	bool           operator == (const PartBjt &other) const = delete;
	bool           operator != (const PartBjt &other) const = delete;

}; // class PartBjt



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartBjt.hpp"



#endif   // mfx_dsp_va_mna_PartBjt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
