/*****************************************************************************

        Router.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_Router_HEADER_INCLUDED)
#define mfx_cmd_Router_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/lat/Algo.h"
#include "mfx/cmd/CnxEnd.h"
#include "mfx/cmd/Document.h"



namespace mfx
{

class BufAlloc;
class PluginPool;
class ProcessingContext;

namespace cmd
{



class Router
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Router ()  = default;
	virtual        ~Router () = default;

	void           set_process_info (double sample_freq, int max_block_size);
	void           create_routing (Document &doc, PluginPool &plugin_pool);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class CnxInfo
	{
	public:
		int            _delay   = 0;
		int            _cnx_idx = -1;
	};

	class PinOutInfo
	{
	public:
		int            _dst_count = 0; // Number of connections starting from the output pin
	};
	typedef std::vector <PinOutInfo> PinDstInfoNode;

	class NodeInfo
	{
	public:
		bool           _visit_flag  = false;
		int            _ctx_index   = -1; // Set when visited
		PinDstInfoNode _pin_dst_list;
		std::vector <Document::CnxList>   // Per input pin
		               _cnx_src_list;
		int            _nbr_chn     =  1; // For the outputs
	};

	typedef std::vector <NodeInfo> NodeCateg;
	typedef std::array <NodeCateg, CnxEnd::SlotType_NBR_ELT> NodeCategList;

	void           create_routing_chain (Document &doc, PluginPool &plugin_pool);
	void           create_routing_graph (Document &doc, PluginPool &plugin_pool);
	void           make_graph_from_chain (Document &doc);
	void           add_aux_plugins (Document &doc, PluginPool &plugin_pool);
	void           prepare_graph_for_latency_analysis (const Document &doc);
	int            conv_doc_slot_to_lat_node_index (piapi::Dir dir, const Cnx &cnx) const;
	int            conv_io_pin_to_lat_node_index (piapi::Dir dir, int pin) const;
	void           add_aux_plugins_delays (Document &doc, PluginPool &plugin_pool);
	PluginAux &    create_plugin_aux (Document &doc, PluginPool &plugin_pool, Document::PluginAuxList &aux_list, std::string model);
	void           connect_delays (Document &doc);
	void           create_graph_context (Document &doc, PluginPool &plugin_pool);
	void           init_node_categ_list (const Document &doc, NodeCategList &categ_list) const;
	void           allocate_buf_audio_i (Document &doc, BufAlloc &buf_alloc);
	void           allocate_buf_audio_o (Document &doc, BufAlloc &buf_alloc, const NodeCategList &categ_list);
	void           free_buf_audio_i (Document &doc, BufAlloc &buf_alloc);
	void           free_buf_audio_o (Document &doc, BufAlloc &buf_alloc);
	void           visit_node (Document &doc, const PluginPool &plugin_pool, BufAlloc &buf_alloc, NodeCategList &categ_list, CnxEnd::SlotType slot_type, int slot_pos);
	void           check_source_nodes (Document &doc, const PluginPool &plugin_pool, BufAlloc &buf_alloc, NodeCategList &categ_list, NodeInfo &node_info);
	void           collects_mix_source_buffers (ProcessingContext &ctx, BufAlloc &buf_alloc, const NodeCategList &categ_list, const NodeInfo &node_info, ProcessingContextNode::Side &side, int nbr_pins_ctx, int nbr_chn, ProcessingContext::PluginContext::MixInputArray &mix_in_arr) const;
	void           free_source_buffers (ProcessingContext &ctx, BufAlloc &buf_alloc, const NodeCategList &categ_list, const NodeInfo &node_info, ProcessingContextNode::Side &side, int nbr_pins_ctx, int nbr_chn, ProcessingContext::PluginContext::MixInputArray &mix_in_arr);
	const ProcessingContextNode::Side &
	               use_source_side (const NodeCategList &categ_list, const ProcessingContext &ctx, const Cnx &cnx_src) const;
	int            count_nbr_signal_buf (const Document &doc, const NodeCategList &categ_list) const;

	static int     clip_channel (int chn_idx, int nbr_chn);

	// Sampling rate, Hz. > 0. 0 = not known yet
	double         _sample_freq    = 0;

	// Maximum processing length, samples. > 0. 0 = not known yet
	int            _max_block_size = 0;

	lat::Algo      _lat_algo;
	int            _nbr_slots      = 0; // From Document::_slot_list
	int            _nbr_a_src      = 1; // Number of audio input pins
	int            _nbr_a_dst      = 1; // Number of audio output pins

	// Same as Document::_cnx_list, but includes delay plug-ins
	Document::CnxList
	               _cnx_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Router (const Router &other)            = delete;
	Router &       operator = (const Router &other)        = delete;
	bool           operator == (const Router &other) const = delete;
	bool           operator != (const Router &other) const = delete;

}; // class Router



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/Router.hpp"



#endif   // mfx_cmd_Router_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
