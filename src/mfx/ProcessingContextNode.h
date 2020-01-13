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



namespace fstb
{
	class Crc32;
}

namespace mfx
{



class ProcessingContextNode
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Side
	{
	public:
		// 2 because the dry/wet/mix plug-in has two sets of input pins, the
		// second one for the default bypass signal.
		typedef std::array <
			int,
			(2 * Cst::_max_nbr_output) * piapi::PluginInterface::_max_nbr_chn
		> BufArray;
		static_assert (Cst::_max_nbr_input <= 2 * Cst::_max_nbr_output, "");

		int &          use_buf (int pin, int chn);
		const int &    use_buf (int pin, int chn) const;

		void           compute_graph_crc (fstb::Crc32 &crc) const;

		// The number of pins in this buffer may be larger than the actual
		// number of pins of the plug-in.
		BufArray       _buf_arr;
		int            _nbr_chn;      // For each input or output pin
		int            _nbr_chn_tot;  // Total number of channels
	};

	class SigInfo
	{
	public:
		int            _buf_index  = Cst::BufSpecial_TRASH;
		int            _port_index = -1;
	};
	typedef std::array <SigInfo, Cst::_max_nbr_sig> SigBufArray;

	typedef std::array <Side, Dir_NBR_ELT> SideArray;

	               ProcessingContextNode ();
	               ProcessingContextNode (const ProcessingContextNode &other) = default;
	ProcessingContextNode &
	               operator = (const ProcessingContextNode &other) = default;

	void           compute_graph_crc (fstb::Crc32 &crc) const;

	int            _pi_id;
	SideArray      _side_arr;
	SigBufArray    _sig_buf_arr;
	int            _nbr_sig;

	// Indicates we have to update the auxiliary parameters when the context
	// is switched.
	// This is for the D/W/M plug-in and possibly for the main plug-in,
	// if it supports it.
	bool           _aux_param_flag;
	int            _comp_delay;      // Compensation delay, in samples. >= 0
	int            _pin_mult;        // Pin multiplier, >= 1. For Dry/Wet mix plug-ins, it's the actual number of output pins.



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
