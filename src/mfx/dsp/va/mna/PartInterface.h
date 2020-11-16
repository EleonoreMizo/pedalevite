/*****************************************************************************

        PartInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_mna_PartInterface_HEADER_INCLUDED)
#define mfx_dsp_va_mna_PartInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/va/mna/Flt.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace va
{
namespace mna
{



class SimulInterface;

class PartInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <PartInterface> SPtr;
	typedef int IdNode;
	static constexpr IdNode _nid_invalid = -1;
	static constexpr IdNode _nid_gnd     = 0;

	class PartInfo
	{
	public:
		// Subparts are probbed directly by the simulator after return from
		// the parent
		std::vector <SPtr>
		               _subpart_arr;
		std::vector <IdNode>
		               _nid_arr;
		int            _nbr_src_v       = 0;
		bool           _non_linear_flag = false;
	};

	class SimInfo
	{
	public:
		double         _sample_freq = 0;
		std::vector <int>
		               _node_idx_arr;
		std::vector <int>
		               _src_v_idx_arr;
	};

	               PartInterface ()                           = default;
	               PartInterface (const PartInterface &other) = default;
	               PartInterface (PartInterface &&other)      = default;
	virtual        ~PartInterface ()                          = default;

	virtual PartInterface &
	               operator = (const PartInterface &other)    = default;
	virtual PartInterface &
	               operator = (PartInterface &&other)         = default;

	void           get_info (SimulInterface &sim, PartInfo &info);
	void           prepare (const SimInfo &info);
	void           add_to_matrix (int it_cnt);
	void           step ();
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_get_info (SimulInterface &sim, PartInfo &info) = 0;
	virtual void   do_prepare (const SimInfo &info) = 0;
	virtual void   do_add_to_matrix (int it_cnt) = 0;
	virtual void   do_step () = 0;
	virtual void   do_clear_buffers () = 0;



}; // class PartInterface



}  // namespace mna
}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/mna/PartInterface.hpp"



#endif   // mfx_dsp_va_mna_PartInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
