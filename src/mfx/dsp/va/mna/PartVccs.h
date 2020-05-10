/*****************************************************************************

        PartVccs.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartVccs_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartVccs_HEADER_INCLUDED



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



class PartVccs
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartVccs (IdNode nid_i_1, IdNode nid_i_2, IdNode nid_o_1, IdNode nid_o_2, float g);
	virtual        ~PartVccs () = default;

	void           set_transconductance (float g);



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

	std::array <IdNode, 2>     // Input
	               _nid_i_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <IdNode, 2>     // Output
	               _nid_o_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <int, 2>
	               _node_i_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	std::array <int, 2>
	               _node_o_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	SimulInterface *
	               _sim_ptr = nullptr;
	float          _g       = 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartVccs ()                               = delete;
	               PartVccs (const PartVccs &other)          = delete;
	               PartVccs (PartVccs &&other)               = delete;
	PartVccs &     operator = (const PartVccs &other)        = delete;
	PartVccs &     operator = (PartVccs &&other)             = delete;
	bool           operator == (const PartVccs &other) const = delete;
	bool           operator != (const PartVccs &other) const = delete;

}; // class PartVccs



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartVccs.hpp"



#endif   // mfx_dsp_va_mna_PartVccs_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

