/*****************************************************************************

        PartCapacitor.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartCapacitor_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartCapacitor_HEADER_INCLUDED



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



class PartCapacitor
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartCapacitor (IdNode nid_1, IdNode nid_2, float c);
	virtual        ~PartCapacitor () = default;

	void           set_capacity (float c);



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

	void           update_geq ();

	std::array <IdNode, 2>
	               _nid_arr  = {{ _nid_invalid, _nid_invalid }};
	std::array <int, 2>
	               _node_arr = {{
	               	SimulInterface::_node_invalid,
	               	SimulInterface::_node_invalid
	               }};
	SimulInterface *
	               _sim_ptr = nullptr;
	float          _c       = 1e-9f;    // farad, > 0
	float          _sample_freq = 0;    // Hz, > 0. 0 = not set.

	// State
	float          _geq     = 0;
	float          _ieq     = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartCapacitor ()                               = delete;
	               PartCapacitor (const PartCapacitor &other)     = delete;
	               PartCapacitor (PartCapacitor &&other)          = delete;
	PartCapacitor &
	               operator = (const PartCapacitor &other)        = delete;
	PartCapacitor &
	               operator = (PartCapacitor &&other)             = delete;
	bool           operator == (const PartCapacitor &other) const = delete;
	bool           operator != (const PartCapacitor &other) const = delete;

}; // class PartCapacitor



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartCapacitor.hpp"



#endif   // mfx_dsp_va_mna_PartCapacitor_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
