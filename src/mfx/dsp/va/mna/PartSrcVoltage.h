/*****************************************************************************

        PartSrcVoltage.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartSrcVoltage_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartSrcVoltage_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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



class PartSrcVoltage
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartSrcVoltage (IdNode nid_1, IdNode nid_2, Flt v) noexcept;
	virtual        ~PartSrcVoltage () = default;

	void           set_voltage (Flt v) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PartInterface
	void           do_get_info (SimulInterface &sim, PartInfo &info) final;
	void           do_prepare (const SimInfo &info) final;
	void           do_add_to_matrix (int it_cnt) noexcept final;
	void           do_step () noexcept final;
	void           do_clear_buffers () noexcept final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::array <IdNode, 2>
	               _nid_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <int, 2>
	               _node_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	SimulInterface *
	               _sim_ptr   = nullptr;
	Flt            _v         = 0;      // Volts
	int            _src_v_pos = SimulInterface::_node_invalid;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartSrcVoltage ()                               = delete;
	               PartSrcVoltage (const PartSrcVoltage &other)    = delete;
	               PartSrcVoltage (PartSrcVoltage &&other)         = delete;
	PartSrcVoltage &
	               operator = (const PartSrcVoltage &other)        = delete;
	PartSrcVoltage &
	               operator = (PartSrcVoltage &&other)             = delete;
	bool           operator == (const PartSrcVoltage &other) const = delete;
	bool           operator != (const PartSrcVoltage &other) const = delete;

}; // class PartSrcVoltage



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartSrcVoltage.hpp"



#endif   // mfx_dsp_va_mna_PartSrcVoltage_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
