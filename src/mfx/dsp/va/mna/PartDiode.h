/*****************************************************************************

        PartDiode.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartDiode_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartDiode_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/va/mna/PartInterface.h"
#include "mfx/dsp/va/mna/SimulInterface.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



class PartDiode
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartDiode (IdNode nid_1, IdNode nid_2, Flt is, Flt n);
	virtual        ~PartDiode () = default;

	void           set_is (Flt is);
	void           set_n (Flt n);



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

	Flt            compute_nvt_inv () const;
	Flt            compute_mul_e () const;
	Flt            compute_vcrit () const;

	std::array <IdNode, 2>
	               _nid_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <int, 2>
	               _node_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	SimulInterface *
	               _sim_ptr = nullptr;

	const Flt      _vt      = Flt (0.026);   // Thermal voltage, volt
	Flt            _is      = Flt (0.1e-15); // Inverse saturation current, ampere, > 0
	Flt            _n       = Flt (1);       // Quality factor, > 0

	Flt            _nvt_inv = compute_nvt_inv (); // Precalculed stuff
	Flt            _mul_e   = compute_mul_e (); // Precalculed stuff
	Flt            _vcrit   = compute_vcrit (); // Not used at the moment



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartDiode ()                               = delete;
	               PartDiode (const PartDiode &other)         = delete;
	               PartDiode (PartDiode &&other)              = delete;
	PartDiode &    operator = (const PartDiode &other)        = delete;
	PartDiode &    operator = (PartDiode &&other)             = delete;
	bool           operator == (const PartDiode &other) const = delete;
	bool           operator != (const PartDiode &other) const = delete;

}; // class PartDiode



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartDiode.hpp"



#endif   // mfx_dsp_va_mna_PartDiode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
