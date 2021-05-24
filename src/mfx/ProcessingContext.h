/*****************************************************************************

        ProcessingContext.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ProcessingContext_HEADER_INCLUDED)
#define mfx_ProcessingContext_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ControlledParam.h"
#include "mfx/ControlSource.h"
#include "mfx/Cst.h"
#include "mfx/PiType.h"
#include "mfx/ProcessingContextNode.h"

#include <array>
#include <map>
#include <memory>
#include <vector>

#include <climits>



namespace fstb
{
	class Crc32;
}

namespace mfx
{



class ProcessingContext
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class PluginContext
	{
	public:
		typedef std::array <ProcessingContextNode, PiType_NBR_ELT> NodeList;

		typedef std::array <
			int,
			Cst::_max_nbr_output * piapi::PluginInterface::_max_nbr_chn
		> BypBufArray;

		typedef std::vector <int> MixInChn; // Buffers to be mixed in a single input channel. At least 2 elements or none.
		typedef std::vector <MixInChn> MixInputArray; // All the mixed channels for the audio inputs. Size = _node_arr [PiType_MAIN]._side_arr [Dir_IN]._nbr_chn_tot

		void           compute_graph_crc (fstb::Crc32 &crc) const;
		static void    compute_graph_crc_mix_i (fstb::Crc32 &crc, const MixInputArray &mix_in_arr);

		std::string    _pi_model;        // May be empty if the main plug-in is not set (_pi_id < 0)
		NodeList       _node_arr;
		bool           _mixer_flag = false;
		MixInputArray  _mix_in_arr;      // Empty if there is no buffer to mix prior to processing.
		BypBufArray    _bypass_buf_arr = {{ -1 }};  // Starts with -1 if bypass should not be generated. Number of elements: _node_arr [PiType_MAIN]._side_arr [Dir_OUT]._nbr_chn_tot
	};

	typedef std::vector <PluginContext> PluginCtxArray;

	typedef std::map <ParamCoord, std::shared_ptr <ControlledParam> > MapParamCtrl;
	typedef std::multimap <ControlSource, std::shared_ptr <ControlledParam> > MapSourceParam;
	typedef std::multimap <ControlSource, std::shared_ptr <CtrlUnit> > MapSourceUnit;

	class IoDevice
	{
	public:
		ProcessingContextNode        // Initial and final aligned buffers
		               _ctx;
		PluginContext::MixInputArray // For the output pins
		               _mix;
	};

	typedef int RsMask;
	static_assert (sizeof (RsMask) * CHAR_BIT >= Cst::_max_nbr_send, "");

	               ProcessingContext ()  = default;

	void           compute_graph_crc (fstb::Crc32 &crc) const;
	std::string    dump_as_str (const PluginPool &plugin_pool) const;

	IoDevice       _interface;
	IoDevice       _send;

	// Bitmask indicating if send and return pins are used.
	RsMask         _mask_send = 0;
	RsMask         _mask_ret  = 0;

	PluginCtxArray _context_arr;

	MapParamCtrl   _map_param_ctrl;
	MapSourceParam _map_src_param;
	MapSourceUnit  _map_src_unit;

	float          _master_vol  = 1;

	/*** To do:
	Clarify the releation between the number of pins and channels.
	Curently we assume only a single pin.
	***/
	int            _nbr_chn_out = 1;

	bool           _graph_changed_flag = false;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::string    dump_input_mix (const PluginContext::MixInputArray &mia) const;
	std::string    dump_pi_ctx (const PluginContext &pi_ctx, const PluginPool &plugin_pool) const;
	std::string    dump_ctrl (const PluginPool &plugin_pool) const;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ProcessingContext (const ProcessingContext &other)  = delete;
	               ProcessingContext (const ProcessingContext &&other) = delete;
	ProcessingContext &
	               operator = (const ProcessingContext &other)         = delete;
	ProcessingContext &
	               operator = (const ProcessingContext &&other)        = delete;
	bool           operator == (const ProcessingContext &other) const  = delete;
	bool           operator != (const ProcessingContext &other) const  = delete;

}; // class ProcessingContext



}  // namespace mfx



//#include "mfx/ProcessingContext.hpp"



#endif   // mfx_ProcessingContext_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
