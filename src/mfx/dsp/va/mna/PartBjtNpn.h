/*****************************************************************************

        PartBjtNpn.h
        Author: Laurent de Soras, 2020

Simple model for a BJT NPN transistor, equivalent to Ebers-Moll.

Reference:
http://qucs.sourceforge.net/tech/node70.html

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartBjtNpn_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartBjtNpn_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
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



class PartBjtNpn
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartBjtNpn (IdNode nid_e, IdNode nid_b, IdNode nid_c, Flt is, Flt beta_f, Flt beta_r);
	virtual        ~PartBjtNpn () = default;

	void           set_is (Flt is);
	void           set_beta_f (Flt beta);
	void           set_beta_r (Flt beta);
	void           set_imax (Flt imax);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PartInterface
	void           do_get_info (SimulInterface &sim, PartInfo &info) final;
	void           do_prepare (const SimInfo &info) final;
	void           do_add_to_matrix (int it_cnt) final;
	void           do_step () final;
	void           do_clear_buffers () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::shared_ptr <PartDiode> DiodeSPtr;
	typedef std::shared_ptr <PartCccs> CccsSPtr;

	void           compute_param ();

	static Flt     compute_vmax (Flt imax, Flt is, Flt vt, Flt n);

	const Flt      _vt      = Flt (0.026); // Thermal voltage, volt

	IdNode         _nid_e   = _nid_invalid;
	IdNode         _nid_b   = _nid_invalid;
	IdNode         _nid_c   = _nid_invalid;
	int            _idx_e   = SimulInterface::_node_invalid;
	int            _idx_b   = SimulInterface::_node_invalid;
	int            _idx_c   = SimulInterface::_node_invalid;

	Flt            _is      = Flt (1e-15);
	Flt            _beta_f  = Flt (200.);
	Flt            _beta_r  = Flt (1.);
	Flt            _imax    = Flt (100.);  // A

	SimulInterface *
	               _sim_ptr = nullptr;

	// Precomputed values
	const Flt      _nvt_inv = 1.f / _vt;
	const Flt      _mul_v   = Flt (1.f / (_vt * fstb::LN2));
	Flt            _bf_inv  = 0;
	Flt            _br_inv  = 0;
	Flt            _ni_o_bf = 0;
	Flt            _ni_o_br = 0;
	Flt            _vbe_max = compute_vmax (_imax, _is, _vt, 1);
	Flt            _vbc_max = _vbe_max;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartBjtNpn ()                               = delete;
	               PartBjtNpn (const PartBjtNpn &other)        = delete;
	               PartBjtNpn (PartBjtNpn &&other)             = delete;
	PartBjtNpn &   operator = (const PartBjtNpn &other)        = delete;
	PartBjtNpn &   operator = (PartBjtNpn &&other)             = delete;
	bool           operator == (const PartBjtNpn &other) const = delete;
	bool           operator != (const PartBjtNpn &other) const = delete;

}; // class PartBjtNpn



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartBjtNpn.hpp"



#endif   // mfx_dsp_va_mna_PartBjtNpn_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
