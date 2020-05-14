/*****************************************************************************

        PartDiodeAntipar.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartDiodeAntipar_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartDiodeAntipar_HEADER_INCLUDED



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



class PartDiodeAntipar
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartDiodeAntipar (IdNode nid_1, IdNode nid_2, float is1, float n1, float is2, float n2);
	virtual        ~PartDiodeAntipar () = default;

	void           set_is (int dir, float is);
	void           set_n (int dir, float n);



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

	class Direction
	{
	public:
		float          compute_nvt_inv () const;
		float          compute_mul_v () const;
		float          compute_vcrit () const;

		const float    _vt      = 0.026f;   // Thermal voltage, volt
		float          _is      = 0.1e-15f; // Inverse saturation current, ampere, > 0 for direct, < 0 for inverse
		float          _n       = 1;        // Quality factor, > 0

		// Precalculed stuff
		float          _nvt_inv = compute_nvt_inv (); // Sign depends on the direction
		float          _mul_v   = compute_mul_v ();   // Sign depends on the direction
		float          _vcrit   = compute_vcrit ();   // Not used at the moment
	};

	std::array <IdNode, 2>
	               _nid_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <int, 2>
	               _node_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	SimulInterface *
	               _sim_ptr = nullptr;

	std::array <Direction, 2>
	               _dir_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartDiodeAntipar ()                               = delete;
	               PartDiodeAntipar (const PartDiodeAntipar &other)         = delete;
	               PartDiodeAntipar (PartDiodeAntipar &&other)              = delete;
	PartDiodeAntipar &    operator = (const PartDiodeAntipar &other)        = delete;
	PartDiodeAntipar &    operator = (PartDiodeAntipar &&other)             = delete;
	bool           operator == (const PartDiodeAntipar &other) const = delete;
	bool           operator != (const PartDiodeAntipar &other) const = delete;

}; // class PartDiodeAntipar



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartDiodeAntipar.hpp"



#endif   // mfx_dsp_va_mna_PartDiodeAntipar_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
