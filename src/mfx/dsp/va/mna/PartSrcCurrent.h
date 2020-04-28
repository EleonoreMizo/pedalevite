/*****************************************************************************

        PartSrcCurrent.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartSrcCurrent_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartSrcCurrent_HEADER_INCLUDED



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



class PartSrcCurrent
:	public PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PartSrcCurrent (IdNode nid_1, IdNode nid_2, float i);
	virtual        ~PartSrcCurrent () = default;

	void           set_current (float i);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PartInterface
	void           do_get_info (PartInfo &info) const final;
	void           do_prepare (SimulInterface &sim, const SimInfo &info) final;
	void           do_add_to_matrix () final;
	void           do_step () final;
	void           do_clear_buffers () final;



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
	               _sim_ptr = nullptr;
	float          _i       = 0;        // Amperes



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PartSrcCurrent ()                               = delete;
	               PartSrcCurrent (const PartSrcCurrent &other)    = delete;
	               PartSrcCurrent (PartSrcCurrent &&other)         = delete;
	PartSrcCurrent &
	               operator = (const PartSrcCurrent &other)        = delete;
	PartSrcCurrent &
	               operator = (PartSrcCurrent &&other)             = delete;
	bool           operator == (const PartSrcCurrent &other) const = delete;
	bool           operator != (const PartSrcCurrent &other) const = delete;

}; // class PartSrcCurrent



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartSrcCurrent.hpp"



#endif   // mfx_dsp_va_mna_PartSrcCurrent_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
