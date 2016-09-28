/*****************************************************************************

        ProcessingContextNode.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ProcessingContextNode_HEADER_INCLUDED)
#define mfx_ProcessingContextNode_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/PluginInterface.h"
#include "mfx/Cst.h"
#include "mfx/Dir.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{



class ProcessingContextNode
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ProcessingContextNode ();
	               ProcessingContextNode (const ProcessingContextNode &other) = default;
	virtual        ~ProcessingContextNode ()                       = default;
	ProcessingContextNode &
	               operator = (const ProcessingContextNode &other) = default;

	class Side
	{
	public:
		typedef std::array <int, (2 * Cst::_max_nbr_output) * piapi::PluginInterface::_max_nbr_chn> BufArray;
		static_assert (Cst::_max_nbr_input <= 2 * Cst::_max_nbr_output, "");

		BufArray       _buf_arr;
		int            _nbr_chn;      // For each input or output pin
		int            _nbr_chn_tot;  // Total number of channels
	};

	typedef std::array <int, Cst::_max_nbr_output * piapi::PluginInterface::_max_nbr_chn> BypBufArray;
	typedef std::array <int, Cst::_max_nbr_sig> SigBufArray;

	typedef std::array <Side, Dir_NBR_ELT> SideArray;

	int            _pi_id;
	SideArray      _side_arr;
	BypBufArray    _bypass_buf_arr;  // Starts with -1 if bypass should not be generated.
	SigBufArray    _sig_buf_arr;
	int            _nbr_sig;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ProcessingContextNode &other) const = delete;
	bool           operator != (const ProcessingContextNode &other) const = delete;

}; // class ProcessingContextNode



}  // namespace mfx



//#include "mfx/ProcessingContextNode.hpp"



#endif   // mfx_ProcessingContextNode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
