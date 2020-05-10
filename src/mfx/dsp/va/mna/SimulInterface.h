/*****************************************************************************

        SimulInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_SimulInterface_HEADER_INCLUDED)
#define mfx_dsp_va_mna_SimulInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/mna/PartInterface.h"



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



class SimulInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _node_invalid = -1;

	               SimulInterface ()                               = default;
	               SimulInterface (const SimulInterface &other)    = default;
	               SimulInterface (SimulInterface &&other)         = default;
	virtual        ~SimulInterface ()                              = default;

	virtual SimulInterface &
	               operator = (const SimulInterface &other)        = default;
	virtual SimulInterface &
	               operator = (SimulInterface &&other)             = default;

	PartInterface::IdNode
	               allocate_node ();
	bool           is_node_gnd (int node_idx) const;
	float          get_voltage (int node_idx) const;
	float          get_voltage (int n1_idx, int n2_idx) const;
	void           add_coef_mat (int row, int col, float val);
	void           add_coef_vec (int row, float val);

	// Convenience functions
	void           add_passive (int n1_idx, int n2_idx, float g);
	void           add_norton (int n1_idx, int n2_idx, float geq, float ieq);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual PartInterface::IdNode
	               do_allocate_node () = 0;
	virtual bool   do_is_node_gnd (int node_idx) const = 0;
	virtual float  do_get_voltage (int node_idx) const = 0;
	virtual float  do_get_voltage (int n1_idx, int n2_idx) const = 0;
	virtual void   do_add_coef_mat (int row, int col, float val) = 0;
	virtual void   do_add_coef_vec (int row, float val) = 0;



}; // class SimulInterface



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/SimulInterface.hpp"



#endif   // mfx_dsp_va_mna_SimulInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
