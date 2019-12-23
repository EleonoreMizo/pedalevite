/*****************************************************************************

        Router.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/BufAlloc.h"
#include "mfx/cmd/Document.h"
#include "mfx/cmd/Router.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/Cst.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"

#include <algorithm>
#include <map>
#include <vector>

#include <cassert>



namespace mfx
{
namespace cmd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Router::set_process_info (double sample_freq, int max_block_size)
{
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_sample_freq    = sample_freq;
	_max_block_size = max_block_size;
}



void	Router::create_routing (Document &doc, PluginPool &plugin_pool)
{
#if 0
	// We still keep the old routing method to be on the safe side
	// if something goes wrong during the development.
	create_routing_chain (doc, plugin_pool);
#else
	create_routing_graph (doc, plugin_pool);
#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Router::create_routing_chain (Document &doc, PluginPool &plugin_pool)
{
	ProcessingContext &  ctx = *doc._ctx_sptr;

	// Final number of channels
	int            nbr_chn_cur   =
		ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_IN );
	const int      nbr_chn_final =
		ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_OUT);
	ctx._nbr_chn_out = nbr_chn_final;

	// Buffers
	BufAlloc       buf_alloc (Cst::BufSpecial_NBR_ELT);

	std::array <int, piapi::PluginInterface::_max_nbr_chn>   cur_buf_arr;
	for (auto &b : cur_buf_arr)
	{
		b = -1;
	}

	// Input
	ProcessingContextNode::Side & audio_i =
		ctx._interface_ctx._side_arr [Dir_IN ];
	audio_i._nbr_chn     = Cst::_nbr_chn_in;
	audio_i._nbr_chn_tot = audio_i._nbr_chn;
	for (int i = 0; i < audio_i._nbr_chn; ++i)
	{
		const int      buf = buf_alloc.alloc ();
		audio_i._buf_arr [i] = buf;
	}
	assert (nbr_chn_cur <= audio_i._nbr_chn);
	for (int i = 0; i < nbr_chn_cur; ++i)
	{
		cur_buf_arr [i] = audio_i._buf_arr [i];
	}

	// Plug-ins
	for (const Slot & slot : doc._slot_list)
	{
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			int            nbr_chn_in      = nbr_chn_cur;
			const piapi::PluginDescInterface &   desc_main =
				*plugin_pool.use_plugin (pi_id_main)._desc_ptr;
			const bool     out_st_flag     = desc_main.prefer_stereo ();
			const bool     final_mono_flag = (nbr_chn_final == 1);
			int            nbr_chn_out     =
				  (out_st_flag && ! (slot._force_mono_flag || final_mono_flag))
				? 2
				: nbr_chn_in;

			const int      latency   = slot._component_arr [PiType_MAIN]._latency;

			const int      pi_id_mix = slot._component_arr [PiType_MIX]._pi_id;

			// Processing context
			ctx._context_arr.resize (ctx._context_arr.size () + 1);
			ProcessingContext::PluginContext &  pi_ctx = ctx._context_arr.back ();
			pi_ctx._mixer_flag = (pi_id_mix >= 0);
			ProcessingContextNode & ctx_node_main = pi_ctx._node_arr [PiType_MAIN];

			// Main plug-in
			ctx_node_main._pi_id = pi_id_main;

			pi_ctx._mix_in_arr.clear ();

			ProcessingContextNode::Side & main_side_i =
				ctx_node_main._side_arr [Dir_IN ];
			ProcessingContextNode::Side & main_side_o =
				ctx_node_main._side_arr [Dir_OUT];
			int            main_nbr_i = 1;
			int            main_nbr_o = 1;
			int            main_nbr_s = 0;

			// Input
			desc_main.get_nbr_io (main_nbr_i, main_nbr_o, main_nbr_s);
			main_side_i._nbr_chn     = (main_nbr_i > 0) ? nbr_chn_in : 0;
			main_side_i._nbr_chn_tot = nbr_chn_in * main_nbr_i;
			for (int chn = 0; chn < main_side_i._nbr_chn_tot; ++chn)
			{
				if (chn < nbr_chn_in)
				{
					main_side_i._buf_arr [chn] = cur_buf_arr [chn];
				}
				else
				{
					main_side_i._buf_arr [chn] = Cst::BufSpecial_SILENCE;
				}
			}

			// Output
			std::array <int, piapi::PluginInterface::_max_nbr_chn>   nxt_buf_arr;
			main_side_o._nbr_chn     = (main_nbr_o > 0) ? nbr_chn_out : 0;
			main_side_o._nbr_chn_tot = nbr_chn_out * main_nbr_o;
			for (int chn = 0; chn < main_side_o._nbr_chn_tot; ++chn)
			{
				if (chn < nbr_chn_out && slot._gen_audio_flag)
				{
					const int      buf = buf_alloc.alloc ();
					nxt_buf_arr [chn]          = buf;
					main_side_o._buf_arr [chn] = buf;
				}
				else
				{
					main_side_o._buf_arr [chn] = Cst::BufSpecial_TRASH;
				}
			}

			// Signals
			ctx_node_main._nbr_sig = main_nbr_s;
			const int      nbr_reg_sig =
				int (slot._component_arr [PiType_MAIN]._sig_port_list.size ());
			for (int sig = 0; sig < main_nbr_s; ++sig)
			{
				ProcessingContextNode::SigInfo & sig_info =
					ctx_node_main._sig_buf_arr [sig];
				sig_info._buf_index  = Cst::BufSpecial_TRASH;
				sig_info._port_index = -1;

				if (sig < nbr_reg_sig)
				{
					const int      port_index =
						slot._component_arr [PiType_MAIN]._sig_port_list [sig];
					if (port_index >= 0)
					{
						sig_info._buf_index  = buf_alloc.alloc ();
						sig_info._port_index = port_index;
					}
				}
			}

			// With dry/wet mixer
			if (pi_id_mix >= 0)
			{
				assert (slot._gen_audio_flag);

				ProcessingContextNode & ctx_node_mix = pi_ctx._node_arr [PiType_MIX];

				ctx_node_mix._aux_param_flag = true;
				ctx_node_mix._comp_delay     = latency;
				ctx_node_mix._pin_mult       = 1;

				ctx_node_mix._pi_id = pi_id_mix;
				ProcessingContextNode::Side & mix_side_i =
					ctx_node_mix._side_arr [Dir_IN ];
				ProcessingContextNode::Side & mix_side_o =
					ctx_node_mix._side_arr [Dir_OUT];

				ctx_node_mix._nbr_sig = 0;

				// Bypass output for the main plug-in
				for (int chn = 0; chn < nbr_chn_out * main_nbr_o; ++chn)
				{
					pi_ctx._bypass_buf_arr [chn] = buf_alloc.alloc ();
				}

				// Dry/wet input
				mix_side_i._nbr_chn     = nbr_chn_out;
				mix_side_i._nbr_chn_tot = nbr_chn_out * 2;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					// 1st pin: main output
					mix_side_i._buf_arr [              chn] = nxt_buf_arr [chn];

					// 2nd pin: main input as default bypass
					const int       chn_in = clip_channel (chn, nbr_chn_in);
					const int       buf    = cur_buf_arr [chn_in];
					mix_side_i._buf_arr [nbr_chn_out + chn] = buf;
				}

				// Dry/wet output
				std::array <int, piapi::PluginInterface::_max_nbr_chn>   mix_buf_arr;
				mix_side_o._nbr_chn     = nbr_chn_out;
				mix_side_o._nbr_chn_tot = nbr_chn_out;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					const int      buf = buf_alloc.alloc ();
					mix_buf_arr [chn]         = buf;
					mix_side_o._buf_arr [chn] = buf;
				}

				// Shift buffers
				for (int chn = 0; chn < nbr_chn_out * main_nbr_o; ++chn)
				{
					buf_alloc.ret (pi_ctx._bypass_buf_arr [chn]);
				}
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					buf_alloc.ret (nxt_buf_arr [chn]);
					nxt_buf_arr [chn] = mix_buf_arr [chn];
				}
			}

			// Output buffers become the next input buffers
			if (slot._gen_audio_flag)
			{
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					if (chn < nbr_chn_in)
					{
						buf_alloc.ret (cur_buf_arr [chn]);
					}
					cur_buf_arr [chn] = nxt_buf_arr [chn];
				}
				nbr_chn_cur = nbr_chn_out;
			}
		}
	}

	// Output
	ProcessingContextNode::Side & audio_o =
		ctx._interface_ctx._side_arr [Dir_OUT];
	audio_o._nbr_chn     = Cst::_nbr_chn_out;
	audio_o._nbr_chn_tot = audio_o._nbr_chn;
	for (int i = 0; i < audio_o._nbr_chn; ++i)
	{
		const int      chn_src = clip_channel (i, nbr_chn_cur);
		audio_o._buf_arr [i] = cur_buf_arr [chn_src];
	}

	for (int chn = 0; chn < nbr_chn_cur; ++chn)
	{
		buf_alloc.ret (cur_buf_arr [chn]);
	}
}



void	Router::create_routing_graph (Document &doc, PluginPool &plugin_pool)
{
	/*** To do: temporary ***/
	make_graph_from_chain (doc);
	/*** To do: temporary ***/

	doc._ctx_sptr->_nbr_chn_out =
		ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_OUT);

	// Finds the number of audio inputs and outputs
	_nbr_a_src = 1;
	_nbr_a_dst = 1;
	switch (doc._chn_mode)
	{
	case ChnMode_1M_1M:
		// Nothing
		break;
	case ChnMode_1M_1S:
		// Nothing
		break;
	case ChnMode_1S_1S:
		// Nothing
		break;
	default:
		assert (false);
		break;
	}

	// We probably need to insert additional delays on specific connections
	// to compensate for the plug-in latencies.
	add_aux_plugins (doc, plugin_pool);

	// Now we can create the roadmap which will be sent to the audio thread.
	create_graph_context (doc, plugin_pool);
}



// Completes the Document structure to turn the implicit slot chain into
// a true graph.
void	Router::make_graph_from_chain (Document &doc)
{
	doc._cnx_list.clear ();

	const int         nbr_slots = int (doc._slot_list.size ());

	// This is the current source for the audio processing
	int               slot_src  = 0;
	CnxEnd::SlotType  slot_type = CnxEnd::SlotType_IO;

	for (int slot_pos = 0; slot_pos < nbr_slots; ++slot_pos)
	{
		const Slot &   slot = doc._slot_list [slot_pos];
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			// Connects the plug-in to the source
			Cnx            cnx;
			cnx._src._slot_type = slot_type;
			cnx._src._slot_pos  = slot_src;
			cnx._src._pin       = 0;
			cnx._dst._slot_type = CnxEnd::SlotType_NORMAL;
			cnx._dst._slot_pos  = slot_pos;
			cnx._dst._pin       = 0;
			doc._cnx_list.push_back (cnx);

			// The plug-in becomes the new source only if it generates audio
			if (slot._gen_audio_flag)
			{
				slot_type = CnxEnd::SlotType_NORMAL;
				slot_src  = slot_pos;
			}
		}
	}

	// Last connection to the audio output
	Cnx            cnx;
	cnx._src._slot_type = slot_type;
	cnx._src._slot_pos  = slot_src;
	cnx._src._pin       = 0;
	cnx._dst._slot_type = CnxEnd::SlotType_IO;
	cnx._dst._slot_pos  = 0;
	cnx._dst._pin       = 0;
	doc._cnx_list.push_back (cnx);
}



void	Router::add_aux_plugins (Document &doc, PluginPool &plugin_pool)
{
	// Computes the plug-in latencies across the graph to evaluate where
	// to insert compensation delay plug-ins
	prepare_graph_for_latency_analysis (doc);
	_lat_algo.run ();

	// Inserts required plug-ins: delays for latency compensation
	add_aux_plugins_delays (doc, plugin_pool);

	// Connects the plug-ins to the graph
	_cnx_list = doc._cnx_list;
	connect_delays (doc);
}



// See conv_doc_slot_to_lat_node_index () for the node order in _lat_algo
void	Router::prepare_graph_for_latency_analysis (const Document &doc)
{
	_lat_algo.reset ();
	const int      nbr_cnx   = int (doc._cnx_list.size ());
	_nbr_slots = int (doc._slot_list.size ());
	const int      nbr_nodes = _nbr_slots + _nbr_a_src + _nbr_a_dst;
	_lat_algo.set_nbr_elt (nbr_nodes, nbr_cnx);

	// Builds the connection list
	for (int cnx_pos = 0; cnx_pos < nbr_cnx; ++cnx_pos)
	{
		const Cnx &    cnx_doc = doc._cnx_list [cnx_pos];
		lat::Cnx &     cnx_lat = _lat_algo.use_cnx (cnx_pos);

		// What is an input for a node is an output for a connection
		int            node_idx_src =
			conv_doc_slot_to_lat_node_index (piapi::Dir_IN , cnx_doc);
		cnx_lat.set_node (piapi::Dir_OUT, node_idx_src);

		int            node_idx_dst =
			conv_doc_slot_to_lat_node_index (piapi::Dir_OUT, cnx_doc);
		cnx_lat.set_node (piapi::Dir_IN , node_idx_dst);

		// Reports the connection on the nodes
		lat::Node &    node_src = _lat_algo.use_node (node_idx_src);
		node_src.add_cnx (piapi::Dir_OUT, cnx_pos);

		lat::Node &    node_dst = _lat_algo.use_node (node_idx_dst);
		node_dst.add_cnx (piapi::Dir_IN , cnx_pos);
	}

	// Builds the node list
	for (int slot_pos = 0; slot_pos < _nbr_slots; ++slot_pos)
	{
		lat::Node &    node = _lat_algo.use_node (slot_pos);
		node.set_nature (lat::Node::Nature_NORMAL);

		const Slot &   slot    = doc._slot_list [slot_pos];
		const int      latency = slot._component_arr [PiType_MAIN]._latency;
		node.set_latency (latency);
	}

	// Source nodes (audio inputs)
	for (int pin_cnt = 0; pin_cnt < _nbr_a_src; ++pin_cnt)
	{
		const int      node_idx =
			conv_io_pin_to_lat_node_index (piapi::Dir_IN , pin_cnt);
		lat::Node &    node = _lat_algo.use_node (node_idx);
		node.set_nature (lat::Node::Nature_SOURCE);
		node.set_latency (0);
		assert (node.is_pure (piapi::Dir_OUT));
	}

	// Destination nodes (audio outputs)
	for (int pin_cnt = 0; pin_cnt < _nbr_a_dst; ++pin_cnt)
	{
		const int      node_idx =
			conv_io_pin_to_lat_node_index (piapi::Dir_OUT, pin_cnt);
		lat::Node &    node = _lat_algo.use_node (node_idx);
		node.set_nature (lat::Node::Nature_SINK);
		node.set_latency (0);
		assert (node.is_pure (piapi::Dir_IN ));
	}
}



/*
Maps the document slot indexes to lat::Algo node indexes.
In this order:
- Standard plug-in slots
- Audio inputs
- Audio outputs
*/

int	Router::conv_doc_slot_to_lat_node_index (piapi::Dir dir, const Cnx &cnx) const
{
	const CnxEnd & end = (dir != piapi::Dir_IN) ? cnx._dst : cnx._src;
	int            node_index = end._slot_pos;

	switch (end._slot_type)
	{
	case CnxEnd::SlotType_NORMAL:
		// Nothing
		break;
	case CnxEnd::SlotType_IO:
		node_index += _nbr_slots;
		if (dir != piapi::Dir_IN)
		{
			node_index += _nbr_a_src;
		}
		break;
	case CnxEnd::SlotType_DLY:
		// These kinds of slot should not appear here.
		// They are not inserted yet.
		assert (false);
		break;
	default:
		assert (false);
		break;
	}

	return node_index;
}



int	Router::conv_io_pin_to_lat_node_index (piapi::Dir dir, int pin) const
{
	int            node_index = _nbr_slots + pin;
	if (dir != piapi::Dir_IN)
	{
		node_index += _nbr_a_src;
	}

	return node_index;
}



// When we add delay plug-ins, we should take care to ensure a continuity
// between the successive configurations. We try to use the same delays
// in the same connections, so minor changes in the graph are glitch-free.
void	Router::add_aux_plugins_delays (Document &doc, PluginPool &plugin_pool)
{
	// Lists the connections requiring a compensation delay.
	std::map <Cnx, CnxInfo>  cnx_map; // [cnx] = info
	const int            nbr_cnx = _lat_algo.get_nbr_cnx ();
	for (int cnx_idx = 0; cnx_idx < nbr_cnx; ++cnx_idx)
	{
		const lat::Cnx &  cnx_lat  = _lat_algo.use_cnx (cnx_idx);
		const int         delay    = cnx_lat.get_comp_delay ();
		if (delay > 0)
		{
			const Cnx &    cnx_doc  = doc._cnx_list [cnx_idx];
			CnxInfo &      cnx_info = cnx_map [cnx_doc];
			cnx_info._delay   = delay;
			cnx_info._cnx_idx = cnx_idx;
		}
	}

	// Checks the existing doc._plugin_dly_list to find delays and their
	// connections. Updates the connection list by erasing fulfilled
	// associations.
	const int      nbr_dly = int (doc._plugin_dly_list.size ());
	for (int dly_idx = 0; dly_idx < nbr_dly; ++dly_idx)
	{
		PluginAux &    plug_dly = doc._plugin_dly_list [dly_idx];
		const auto     it       = cnx_map.find (plug_dly._cnx);
		const CnxInfo& cnx_info = it->second;
		if (it != cnx_map.end ())
		{
			plug_dly._cnx_index  = cnx_info._cnx_idx;

			// Updates the delay
			plug_dly._comp_delay = cnx_info._delay;

			// This connection works, we don't need it anymore.
			cnx_map.erase (it);
		}
		else
		{
			// Not found, this plug-in may be removed or replaced.
			doc._map_model_id [plug_dly._model] [plug_dly._pi_id] = false;
			plug_dly._cnx_index = -1;
			plug_dly._cnx.invalidate ();
			plug_dly._param_list.clear ();
		}
	}

	// Completes the plug-in by reusing the slots scheduled for removal.
	int            scan_pos = 0;
	auto           it       = cnx_map.begin ();
	while (scan_pos < nbr_dly && it != cnx_map.end ())
	{
		PluginAux &    plug_dly = doc._plugin_dly_list [scan_pos];
		if (! plug_dly._cnx.is_valid ())
		{
			assert (plug_dly._pi_id >= 0);
			const CnxInfo& cnx_info = it->second;

			// Reuses this plug-in
			doc._map_model_id [plug_dly._model] [plug_dly._pi_id] = true;
			plug_dly._cnx        = it->first;

			plug_dly._cnx_index  = cnx_info._cnx_idx;

			// Sets the delay
			plug_dly._comp_delay = cnx_info._delay;

			// Next connection
			it = cnx_map.erase (it);
		}
		else
		{
			++ scan_pos;
		}
	}

	// Schedules the unused plug-ins for removal
	while (scan_pos < nbr_dly)
	{
		PluginAux &    plug_dly = doc._plugin_dly_list [scan_pos];
		if (! plug_dly._cnx.is_valid ())
		{
			doc._map_model_id [plug_dly._model] [plug_dly._pi_id] = false;
			plug_dly._cnx_index = -1;
			plug_dly._cnx.invalidate ();
			plug_dly._param_list.clear ();
		}
	}

	// Then add more plug-ins if required
	// From here, nbr_dly is not valid any more.
	while (it != cnx_map.end ())
	{
		const CnxInfo& cnx_info = it->second;

		// Creates a new delay plug-in
		PluginAux &    plug_dly = create_plugin_aux (
			doc, plugin_pool, doc._plugin_dly_list, Cst::_plugin_dly
		);
		plug_dly._cnx        = it->first;
		plug_dly._cnx_index  = cnx_info._cnx_idx;

		// Sets the delay
		plug_dly._comp_delay = cnx_info._delay;

		++ it;
	}
}



// The plug-in is inserted at the back of the list
// It is registred in the Document object as "in use".
PluginAux &	Router::create_plugin_aux (Document &doc, PluginPool &plugin_pool, Document::PluginAuxList &aux_list, std::string model)
{
	// Currently we only support delays as auxiliary plug-ins
	assert (model == Cst::_plugin_dly);

	aux_list.resize (aux_list.size () + 1);
	PluginAux &    plug = aux_list.back ();

	plug._model = model;
	plug._pi_id = plugin_pool.create (plug._model);
	assert (plug._pi_id >= 0);

	if (_sample_freq > 0)
	{
		PluginPool::PluginDetails &   details =
			plugin_pool.use_plugin (plug._pi_id);
		int         latency = 0;
		details._pi_uptr->reset (_sample_freq, _max_block_size, latency);
	}

	doc._map_model_id [plug._model] [plug._pi_id] = true;

	return plug;
}



// Generates additional connections to _cnx_list.
void	Router::connect_delays (Document &doc)
{
	const int      nbr_plug = int (doc._plugin_dly_list.size ());
	for (int plug_idx = 0; plug_idx < nbr_plug; ++plug_idx)
	{
		const auto &   plug_dly = doc._plugin_dly_list [plug_idx];
		if (plug_dly._cnx.is_valid ())
		{
			_cnx_list.resize (_cnx_list.size () + 1);
			Cnx            cnx_ins = _cnx_list.back ();
			Cnx &          cnx_old = _cnx_list [plug_dly._cnx_index];

			cnx_ins._src = cnx_old._src;
			cnx_ins._dst._slot_type = CnxEnd::SlotType_DLY;
			cnx_ins._dst._slot_pos  = plug_idx;
			cnx_ins._dst._pin       = 0;

			cnx_old._src._slot_type = CnxEnd::SlotType_DLY;
			cnx_old._src._slot_pos  = plug_idx;
			cnx_old._src._pin       = 0;
		}
	}
}



/*
Graph course starts from the sink nodes and traces back recursively up to
the source nodes. ProcessingContextNode are created during the down-stream
descent (end of the recursion) so their order correspounds to the processing
order.

Buffers are allocated on request and deallocated when their use is terminated
during the graph traversal. Audio input and output buffers should stay
allocated during the traversal. They are freed only at the end.
Signal buffers stay allocated too.
*/

void	Router::create_graph_context (Document &doc, PluginPool &plugin_pool)
{
	NodeCategList  categ_list;
	init_node_categ_list (doc, categ_list);

	BufAlloc       buf_alloc (Cst::BufSpecial_NBR_ELT);

	// Make sure audio input buffers are allocated
	allocate_buf_audio_i (doc, buf_alloc);

	// First, visit all plug-ins without output connection.
	// These are "analysis" plug-ins not generating any audio data, but
	// producing signals which may be useful for the other plug-ins.
	for (int slot_pos = 0
	;	slot_pos < int (categ_list [CnxEnd::SlotType_NORMAL].size ())
	;	++ slot_pos)
	{
		const NodeInfo &  node_info =
			categ_list [CnxEnd::SlotType_NORMAL] [slot_pos];
		if (node_info._pin_dst_list.empty ())
		{
			visit_node (
				doc, plugin_pool, buf_alloc, categ_list,
				CnxEnd::SlotType_NORMAL, slot_pos
			);
		}
	}

	// Now, starts the traversal from the audio output pins
	for (auto &cnx_pin : categ_list [CnxEnd::SlotType_IO] [0]._cnx_src_list)
	{
		for (auto &cnx : cnx_pin)
		{
			if (cnx._src._slot_type != CnxEnd::SlotType_IO)
			{
				visit_node (
					doc, plugin_pool, buf_alloc, categ_list,
					cnx._src._slot_type, cnx._src._slot_pos
				);
			}
		}
	}

	// Allocates output buffers if necessary, and add a reference for
	// all of them to make sure they are kept allocated
	allocate_buf_audio_o (doc, buf_alloc, categ_list);

	// Deallocates audio input and output buffers (for sanity checks)
	free_buf_audio_i (doc, buf_alloc);
	free_buf_audio_o (doc, buf_alloc);
	assert (   buf_alloc.get_nbr_alloc_buf ()
	        == count_nbr_signal_buf (doc, categ_list));
}



// categ_list is an internal node list, sorted by categories.
// A node is a plug-in, aux or not, or an audio input or output.
// Connections are given by _cnx_list.
void	Router::init_node_categ_list (const Document &doc, NodeCategList &categ_list) const
{
	for (auto &categ : categ_list)
	{
		categ.clear ();
	}

	categ_list [CnxEnd::SlotType_NORMAL].resize (doc._slot_list.size ());
	categ_list [CnxEnd::SlotType_DLY   ].resize (doc._plugin_dly_list.size ());
	categ_list [CnxEnd::SlotType_IO    ].resize (1); // For the audio output node

	// For each category, counts the number of input pins fed by each output pin
	for (auto &cnx : _cnx_list)
	{
		// Does not count the audio input node
		if (cnx._src._slot_type != CnxEnd::SlotType_IO)
		{
			NodeCateg &    categ = categ_list [cnx._src._slot_type];
			assert (cnx._src._slot_pos < int (categ.size ()));
			NodeInfo &     node  = categ [cnx._src._slot_pos];
			if (int (node._pin_dst_list.size ()) <= cnx._src._pin)
			{
				node._pin_dst_list.resize (cnx._src._pin + 1);
			}
			PinOutInfo &   pin   = node._pin_dst_list [cnx._src._pin];

			++ pin._dst_count;
		}

		// Registers the connection on the destination node
		{
			NodeCateg &    categ = categ_list [cnx._dst._slot_type];
			assert (cnx._dst._slot_pos < int (categ.size ()));
			NodeInfo &     node  = categ [cnx._dst._slot_pos];
			if (int (node._cnx_src_list.size ()) <= cnx._dst._pin)
			{
				node._cnx_src_list.resize (cnx._dst._pin + 1);
			}
			Document::CnxList &  pin = node._cnx_src_list [cnx._dst._pin];
			pin.push_back (cnx);
		}
	}
}



// We allocate only buffers we need, given the pin configuration.
// Mixing or channel replication is handled by WorldAudio.
void	Router::allocate_buf_audio_i (Document &doc, BufAlloc &buf_alloc)
{
	ProcessingContext &  ctx = *doc._ctx_sptr;
	ProcessingContextNode::Side & audio_i =
		ctx._interface_ctx._side_arr [Dir_IN ];
	const int      nbr_pins = _nbr_a_src;
	audio_i._nbr_chn     = ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_IN);
	audio_i._nbr_chn_tot = nbr_pins * audio_i._nbr_chn;
	for (int chn_cnt = 0; chn_cnt < audio_i._nbr_chn_tot; ++chn_cnt)
	{
		const int      buf = buf_alloc.alloc ();
		audio_i._buf_arr [chn_cnt] = buf;
	}
}



// We allocate only buffers we need, given the pin configuration.
void	Router::allocate_buf_audio_o (Document &doc, BufAlloc &buf_alloc, const NodeCategList &categ_list)
{
	const NodeInfo &  node_info = categ_list [CnxEnd::SlotType_IO] [0];

	ProcessingContext &  ctx = *doc._ctx_sptr;
	ProcessingContextNode::Side & audio_o =
		ctx._interface_ctx._side_arr [Dir_OUT];
	const int      nbr_pins = _nbr_a_dst;
	audio_o._nbr_chn     = ctx._nbr_chn_out;
	audio_o._nbr_chn_tot = nbr_pins * audio_o._nbr_chn;
	assert (audio_o._nbr_chn_tot <= Cst::_nbr_chn_out);
	assert (node_info._cnx_src_list.size () == nbr_pins);

	// Use the source buffers as audio output buffers or allocates some to mix
	// several inputs
	collects_mix_source_buffers (
		ctx, buf_alloc, categ_list, node_info,
		audio_o, nbr_pins, audio_o._nbr_chn,
		ctx._interface_mix
	);

	// Keeps all the destination buffers allocated
	for (int chn_cnt = 0; chn_cnt < audio_o._nbr_chn_tot; ++chn_cnt)
	{
		buf_alloc.use_more_if_std (audio_o._buf_arr [chn_cnt]);
	}

	// Deallocates the source buffers
	free_source_buffers (
		ctx, buf_alloc, categ_list, node_info,
		audio_o, nbr_pins, audio_o._nbr_chn,
		ctx._interface_mix
	);
}



// Inverse of allocate_buf_audio_i
void	Router::free_buf_audio_i (Document &doc, BufAlloc &buf_alloc)
{
	ProcessingContext &  ctx = *doc._ctx_sptr;
	ProcessingContextNode::Side & audio_i =
		ctx._interface_ctx._side_arr [Dir_IN ];

	for (int chn_cnt = 0; chn_cnt < audio_i._nbr_chn_tot; ++chn_cnt)
	{
		buf_alloc.ret (audio_i._buf_arr [chn_cnt]);
	}
}



// Inverse of allocate_buf_audio_o
void	Router::free_buf_audio_o (Document &doc, BufAlloc &buf_alloc)
{
	ProcessingContext &  ctx = *doc._ctx_sptr;
	ProcessingContextNode::Side & audio_o =
		ctx._interface_ctx._side_arr [Dir_OUT];

	for (int chn_cnt = 0; chn_cnt < audio_o._nbr_chn_tot; ++chn_cnt)
	{
		buf_alloc.ret_if_std (audio_o._buf_arr [chn_cnt]);
	}
}



/*
Recursive traversal of the nodes.


There is nothing to do if the node has already been visited, buffers
are already allocated.
*/
void	Router::visit_node (Document &doc, const PluginPool &plugin_pool, BufAlloc &buf_alloc, NodeCategList &categ_list, CnxEnd::SlotType slot_type, int slot_pos)
{
	assert (   slot_type == CnxEnd::SlotType_NORMAL
	        || slot_type == CnxEnd::SlotType_DLY);

	NodeInfo &     node_info = categ_list [slot_type] [slot_pos];

	// Node not visited yet ?
	if (! node_info._visit_flag)
	{
		// Recursive traversal.
		// This will update the channel count, for the input
		// On the output, node_info._nbr_chn will contain the number of input
		// channels, temporarily. It will be updated later with the number of
		// output channels.
		check_source_nodes (doc, plugin_pool, buf_alloc, categ_list, node_info);

		ProcessingContext &  ctx = *doc._ctx_sptr;

		// Pins that are actually connected in the graph.
		// May be different (< or >) of the plug-in's real number of i/o pins.
		const int      nbr_pins_o = int (node_info._pin_dst_list.size ());
		const int      nbr_pins_i = int (node_info._cnx_src_list.size ());

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Collects some information depending on the plug-in type

		int            pi_id_main = -1;
		int            pi_id_mix  = -1;
		int            latency    =  0;
		int            nbr_chn_i  = node_info._nbr_chn;
		int            nbr_chn_o  = nbr_chn_i;
		int            main_nbr_i = 1;
		int            main_nbr_o = 1;
		int            main_nbr_s = 0;
#if ! defined (NDEBUG)
		bool           gen_audio_flag = false;
#endif
		if (slot_type == CnxEnd::SlotType_DLY)
		{
			// Delay plug-in
			assert (nbr_pins_i == nbr_pins_o);
			PluginAux &    dly = doc._plugin_dly_list [slot_pos];
			pi_id_main = dly._pi_id;
			latency    = dly._comp_delay;
			dly._comp_delay = 0;
#if ! defined (NDEBUG)
			gen_audio_flag  = true;
#endif
		}
		else
		{
			// Standard plug-in with its mixer
			assert (slot_type == CnxEnd::SlotType_NORMAL);
			const Slot &   slot = doc._slot_list [slot_pos];
			pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
			if (pi_id_main >= 0)
			{
#if ! defined (NDEBUG)
				gen_audio_flag = slot._gen_audio_flag;
#endif

				const piapi::PluginDescInterface &   desc_main =
					*plugin_pool.use_plugin (pi_id_main)._desc_ptr;
				const bool     out_st_flag = desc_main.prefer_stereo ();
				desc_main.get_nbr_io (main_nbr_i, main_nbr_o, main_nbr_s);

				pi_id_mix  = slot._component_arr [PiType_MIX ]._pi_id;
				latency    = slot._component_arr [PiType_MAIN]._latency;
				nbr_chn_o  = nbr_chn_i;
				if (out_st_flag && ! slot._force_mono_flag)
				{
					// At this time the number of channels can be only 1 or 2.
					// So the number of channels for the audio output will
					// perfectly limit the number of chan for the plug-in output:
					// Either we stay in mono up to the end, either we go directly
					// stereo if the audio output is stereo.
					nbr_chn_o = ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_OUT);
				}
			}
		}

		// Now node_info._nbr_chn contains the number of output channels.
		node_info._nbr_chn = nbr_chn_o;

		// Number of pins used for the buffer allocations, and more specifically
		// for the size of the buffer lists.
		const int   nbr_pins_ctx_i = std::max (nbr_pins_i, main_nbr_i);
		const int   nbr_pins_ctx_o = std::max (nbr_pins_o, main_nbr_o);

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Creates a processing context node

		node_info._ctx_index = int (ctx._context_arr.size ());
		ctx._context_arr.resize (node_info._ctx_index + 1);
		ProcessingContext::PluginContext &  pi_ctx = ctx._context_arr.back ();
		pi_ctx._mixer_flag = (pi_id_mix >= 0);

		ProcessingContextNode & ctx_node_main = pi_ctx._node_arr [PiType_MAIN];
		ctx_node_main._pi_id = pi_id_main;
		pi_ctx._mix_in_arr.clear (); // Default: no mix

		ProcessingContextNode::Side & main_side_i =
			ctx_node_main._side_arr [Dir_IN ];
		ProcessingContextNode::Side & main_side_o =
			ctx_node_main._side_arr [Dir_OUT];

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Allocates buffers

		// Inputs
		main_side_i._nbr_chn     = (main_nbr_i > 0) ? nbr_chn_i : 0;
		main_side_i._nbr_chn_tot = nbr_chn_i * nbr_pins_ctx_i;

		collects_mix_source_buffers (
			ctx, buf_alloc, categ_list, node_info,
			main_side_i, nbr_pins_ctx_i, nbr_chn_i,
			pi_ctx._mix_in_arr
		);

		// Outputs
		main_side_o._nbr_chn     = (main_nbr_o > 0) ? nbr_chn_o : 0;
		main_side_o._nbr_chn_tot = nbr_chn_o * nbr_pins_ctx_o;

		for (int pin_cnt = 0; pin_cnt < nbr_pins_ctx_o; ++pin_cnt)
		{
			const int      chn_ofs = pin_cnt * nbr_chn_o;

			// Counts the number of destination nodes for this pin
			int            nbr_dst = 0;
			if (pi_ctx._mixer_flag)
			{
				nbr_dst = 1;
			}
			else if (pin_cnt < nbr_pins_o)
			{
				nbr_dst = node_info._pin_dst_list [pin_cnt]._dst_count;
			}

			// Allocates buffers for this pin, depending on various situations.
			// In case of allocation, we reference them nbr_times. They will be
			// released each time a destination node is visited during the input
			// buffer allocation (even if the node is not really using them).
			for (int chn_cnt = 0; chn_cnt < nbr_chn_o; ++chn_cnt)
			{
				int            buf = Cst::BufSpecial_TRASH;
				if (nbr_dst > 0)
				{
					// No plug-in: acts like a bypass on the first input pin by
					// reporting the buffers on the outputs (no copy needed)
					if (pi_id_main < 0)
					{
						if (main_side_i._nbr_chn == 0)
						{
							buf = Cst::BufSpecial_SILENCE;
						}
						else
						{
							const int      chn_idx =
								clip_channel (chn_cnt, main_side_i._nbr_chn);
							buf = main_side_i._buf_arr [chn_idx];
						}
						buf_alloc.use_more_if_std (buf, nbr_dst);
					}

					// Ensures first that the pin exists on the plug-in side.
					else if (pin_cnt < main_nbr_o)
					{
						buf = buf_alloc.alloc (nbr_dst);
					}

					// The pin does not exist on the plug-in but it content may be
					// used by the destination, so we use a silent buffer here.
					else
					{
						buf = Cst::BufSpecial_SILENCE;
					}
				}
				main_side_o._buf_arr [chn_ofs + chn_cnt] = buf;
			}
		} // for pin_cnt

		// Signals
		ctx_node_main._nbr_sig = main_nbr_s;
		if (slot_type == CnxEnd::SlotType_NORMAL && pi_id_main >= 0)
		{
			const Slot &   slot = doc._slot_list [slot_pos];
			const int      nbr_reg_sig =
				int (slot._component_arr [PiType_MAIN]._sig_port_list.size ());
			for (int sig = 0; sig < main_nbr_s; ++sig)
			{
				ProcessingContextNode::SigInfo & sig_info =
					ctx_node_main._sig_buf_arr [sig];

				// Default: signal is not used and goes to trash
				sig_info._buf_index  = Cst::BufSpecial_TRASH;
				sig_info._port_index = -1;

				// OK, we may be actually using it.
				if (sig < nbr_reg_sig)
				{
					const int      port_index =
						slot._component_arr [PiType_MAIN]._sig_port_list [sig];
					if (port_index >= 0)
					{
						// We don't know where the signals are used, so we keep them
						// allocated during the whole graph traversal. This could be
						// improved, but this is not necessary now.
						sig_info._buf_index  = buf_alloc.alloc ();
						sig_info._port_index = port_index;
					}
				}
			}
		}

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Dry/wet mixer

		if (pi_id_mix >= 0)
		{
#if ! defined (NDEBUG)
			assert (gen_audio_flag);
#endif
			ProcessingContextNode & ctx_node_mix = pi_ctx._node_arr [PiType_MIX];

			// The D/W mixer may need additional configuration and processing.
			ctx_node_mix._aux_param_flag = (latency > 0 || nbr_pins_o > 1);
			ctx_node_mix._comp_delay     = latency;
			ctx_node_mix._pin_mult       = nbr_pins_o;

			ctx_node_mix._pi_id = pi_id_mix;
			ProcessingContextNode::Side & mix_side_i =
				ctx_node_mix._side_arr [Dir_IN ];
			ProcessingContextNode::Side & mix_side_o =
				ctx_node_mix._side_arr [Dir_OUT];

			ctx_node_mix._nbr_sig = 0;

			// The main plug-in may generate an additional bypassed output.
			// Allocates channels for this operation.
			for (int chn = 0; chn < main_side_o._nbr_chn_tot; ++chn)
			{
				pi_ctx._bypass_buf_arr [chn] = buf_alloc.alloc ();
			}

			// Dry/wet mixer input
			// *2 because the number of input pins of the D/W mixer is the double
			// of the output plug-in pins (wet and dry for each pin)
			mix_side_i._nbr_chn     =                      nbr_chn_o;
			mix_side_i._nbr_chn_tot = nbr_pins_ctx_o * 2 * nbr_chn_o;
			for (int pin_cnt = 0; pin_cnt < nbr_pins_ctx_o; ++pin_cnt)
			{
				const int   ofs_chn_i = pin_cnt * nbr_chn_i;
				const int   ofs_chn_o = pin_cnt * nbr_chn_o;

				// Input pins are interleaved: wet0, dry0, wet1, dry1...
				const int   ofs_chn_w = pin_cnt * 2 * nbr_chn_o;
				const int   ofs_chn_d = ofs_chn_w + nbr_chn_o;

				for (int chn_cnt = 0; chn_cnt < nbr_chn_o; ++chn_cnt)
				{
					// 1st pin of the pair: wet, main output
					mix_side_i._buf_arr [ofs_chn_w + chn_cnt] =
						main_side_o._buf_arr [ofs_chn_o + chn_cnt];

					// 2nd pin of the pair: dry, main input as default.
					// The RT thread will use the bypass buffers instead if they
					// were filled by the main plug-in.
					// The number of pins may be different for the input and the
					// output, so we need silence buffers if we run short on input
					// pins.
					int               buf = Cst::BufSpecial_SILENCE;
					if (pin_cnt < nbr_pins_ctx_i)
					{
						const int    chn_idx = clip_channel (chn_cnt, nbr_chn_i);
						buf = main_side_i._buf_arr [ofs_chn_i + chn_idx];
					}
					mix_side_i._buf_arr [ofs_chn_d + chn_cnt] = buf;
				}
			}

			// Dry/wet mixer output
			mix_side_o._nbr_chn     =                  nbr_chn_o;
			mix_side_o._nbr_chn_tot = nbr_pins_ctx_o * nbr_chn_o;
			for (int pin_cnt = 0; pin_cnt < nbr_pins_ctx_o; ++pin_cnt)
			{
				const int      ofs_chn = pin_cnt * nbr_chn_o;
				int            nbr_dst = 0;
				if (pin_cnt < nbr_pins_o)
				{
					nbr_dst = node_info._pin_dst_list [pin_cnt]._dst_count;
				}

				for (int chn_cnt = 0; chn_cnt < nbr_chn_o; ++chn_cnt)
				{
					int            buf = Cst::BufSpecial_TRASH;
					if (nbr_dst > 0)
					{
						buf = buf_alloc.alloc (nbr_dst);
					}
					mix_side_o._buf_arr [ofs_chn + chn_cnt] = buf;
				}
			}
		}

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// At this point, the plug-in is being processed

		if (slot_type == CnxEnd::SlotType_DLY)
		{
			assert (latency > 0);
			ctx_node_main._aux_param_flag = true;
			ctx_node_main._comp_delay     = latency;
			ctx_node_main._pin_mult       = nbr_pins_ctx_i;
		}

		// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
		// Frees buffers

		// Input buffers for the main plug-in
		free_source_buffers (
			ctx, buf_alloc, categ_list, node_info,
			main_side_i, nbr_pins_ctx_i, nbr_chn_i,
			pi_ctx._mix_in_arr
		);

		// Output buffers for the main plug-in in case of dry/wet mixer.
		// The real output buffers are the d/w mixer ones.
		if (pi_id_mix >= 0)
		{
			for (int chn = 0; chn < main_side_o._nbr_chn_tot; ++chn)
			{
				// Output of the main plug-in
				buf_alloc.ret (main_side_o._buf_arr [chn]);

				// Bypass output for the main plug-in
				buf_alloc.ret (pi_ctx._bypass_buf_arr [chn]);
			}
		}

		// Done.
		node_info._visit_flag = true;
	}
}



void	Router::check_source_nodes (Document &doc, const PluginPool &plugin_pool, BufAlloc &buf_alloc, NodeCategList &categ_list, NodeInfo &node_info)
{
	int         nbr_chn = 1;

	// Visits all source nodes
	for (auto &cnx_src_pin : node_info._cnx_src_list)
	{
		for (auto &cnx_src : cnx_src_pin)
		{
			const NodeInfo &  node_src =
				categ_list [cnx_src._src._slot_type] [cnx_src._src._slot_pos];
			int            nbr_chn_src = 1;

			// Source is an audio input
			if (cnx_src._src._slot_type == CnxEnd::SlotType_IO)
			{
				nbr_chn_src = ChnMode_get_nbr_chn (doc._chn_mode, piapi::Dir_IN);
			}

			// Source is a plug-in: process it
			else
			{
				visit_node (
					doc, plugin_pool, buf_alloc, categ_list,
					cnx_src._src._slot_type, cnx_src._src._slot_pos
				);
				nbr_chn_src = node_src._nbr_chn;
			}

			nbr_chn = std::max (nbr_chn, nbr_chn_src);
		}
	}

	node_info._nbr_chn = nbr_chn;
}



void	Router::collects_mix_source_buffers (ProcessingContext &ctx, BufAlloc &buf_alloc, const NodeCategList &categ_list, const NodeInfo &node_info, ProcessingContextNode::Side &side, int nbr_pins_ctx, int nbr_chn, ProcessingContext::PluginContext::MixInputArray &mix_in_arr) const
{
	bool           mix_flag = false;
	const int      nbr_pins_cnx = int (node_info._cnx_src_list.size ());
	mix_in_arr.resize (side._nbr_chn_tot);
	for (int pin_cnt = 0; pin_cnt < nbr_pins_ctx; ++pin_cnt)
	{
		const int      chn_ofs = pin_cnt * nbr_chn;

		// Default: silence
		int            nbr_real_src = 0;
		for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
		{
			side._buf_arr [chn_ofs + chn_cnt] = Cst::BufSpecial_SILENCE;
		}

		// Pins in the graph
		if (pin_cnt < nbr_pins_cnx)
		{
			const Document::CnxList &  pin_src_arr =
				node_info._cnx_src_list [pin_cnt];
			const int      nbr_src = int (pin_src_arr.size ());

			// Scans each source and add their buffers to the mixing lists
			for (int src_cnt = 0; src_cnt < nbr_src; ++src_cnt)
			{
				const Cnx &       cnx_src  = pin_src_arr [src_cnt];
				const ProcessingContextNode::Side & src_side_o =
					use_source_side (categ_list, ctx, cnx_src);

				const int      nbr_chn_src = src_side_o._nbr_chn;

				if (nbr_chn_src > 0)
				{
					// Collects the input channels
					for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
					{
						ProcessingContext::PluginContext::MixInChn & mix_in =
							mix_in_arr [chn_ofs + chn_cnt];

						const int      chn_idx = clip_channel (chn_cnt, nbr_chn_src);
						const int      buf     =
							src_side_o.use_buf (cnx_src._src._pin, chn_idx);
						mix_in.push_back (buf);
						nbr_real_src = std::max (nbr_real_src, int (mix_in.size ()));
					}
				}
			} // for src_cnt

			// Input buffers for the plug-in
			for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
			{
				int            buf = Cst::BufSpecial_SILENCE;
				if (nbr_real_src > 1)
				{
					// Allocates new buffers for the mix output
					buf = buf_alloc.alloc ();
					mix_flag = true;
				}
				else if (nbr_real_src == 1)
				{
					// Only one source, moves the mixing list content to the
					// ProcessingContextNode::Side
					ProcessingContext::PluginContext::MixInChn & mix_in =
						mix_in_arr [chn_ofs + chn_cnt];
					assert (mix_in.size () == 1);
					buf = mix_in [0];
					mix_in.clear ();
				}
				side._buf_arr [chn_ofs + chn_cnt] = buf;
			} // for chn_cnt
		} // if pin_cnt < nbr_pins_cnx
	} // for pin_cnt

	// Removes the mixing data if we found out there is nothing to mix
	if (! mix_flag)
	{
		mix_in_arr.clear ();
	}
}



void	Router::free_source_buffers (ProcessingContext &ctx, BufAlloc &buf_alloc, const NodeCategList &categ_list, const NodeInfo &node_info, ProcessingContextNode::Side &side, int nbr_pins_ctx, int nbr_chn, ProcessingContext::PluginContext::MixInputArray &mix_in_arr)
{
	const bool     mix_flag = ! mix_in_arr.empty ();

	for (int pin_cnt = 0; pin_cnt < nbr_pins_ctx; ++pin_cnt)
	{
		const int      chn_ofs = pin_cnt * nbr_chn;
		if (mix_flag && ! mix_in_arr [chn_ofs].empty ())
		{
			// Releases the main inputs
			for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
			{
				const int      buf = side._buf_arr [chn_ofs + chn_cnt];
				buf_alloc.ret_if_std (buf);
			}
		}

		// Releases the mixed output pins from the source nodes
		const Document::CnxList &  cnx_list =
			node_info._cnx_src_list [pin_cnt];
		for (auto &cnx_src : cnx_list)
		{
			if (cnx_src._src._slot_type != CnxEnd::SlotType_IO)
			{
				const ProcessingContextNode::Side & src_side_o =
					use_source_side (categ_list, ctx, cnx_src);

				const int      nbr_chn_src = src_side_o._nbr_chn;
				const int      chn_ofs_src = cnx_src._src._pin * nbr_chn_src;
				for (int chn_cnt = 0; chn_cnt < nbr_chn_src; ++chn_cnt)
				{
					const int      buf =
						src_side_o._buf_arr [chn_ofs_src + chn_cnt];
					buf_alloc.ret_if_std (buf);
				}
			}
		}
	}
}



const ProcessingContextNode::Side &	Router::use_source_side (const NodeCategList &categ_list, const ProcessingContext &ctx, const Cnx &cnx_src) const
{
	const NodeInfo &  node_src =
		categ_list [cnx_src._src._slot_type] [cnx_src._src._slot_pos];
	const ProcessingContextNode::Side * src_side_o_ptr = 0;

	if (cnx_src._src._slot_type == CnxEnd::SlotType_IO)
	{
		src_side_o_ptr = &ctx._interface_ctx._side_arr [Dir_IN];
	}

	else
	{
		assert (node_src._ctx_index >= 0);
		const ProcessingContext::PluginContext & pi_ctx_src =
			ctx._context_arr [node_src._ctx_index];
		const ProcessingContextNode & ctx_src =
				(pi_ctx_src._mixer_flag)
			? pi_ctx_src._node_arr [PiType_MIX ]
			: pi_ctx_src._node_arr [PiType_MAIN];
		src_side_o_ptr = &ctx_src._side_arr [Dir_OUT];
	}

	return *src_side_o_ptr;
}



int	Router::count_nbr_signal_buf (const Document &doc, const NodeCategList &categ_list) const
{
	int            nbr_buf = 0;

	for (int slot_pos = 0
	;	slot_pos < int (categ_list [CnxEnd::SlotType_NORMAL].size ())
	;	++ slot_pos)
	{
		const Slot &   slot       = doc._slot_list [slot_pos];
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			const int      nbr_reg_sig =
				int (slot._component_arr [PiType_MAIN]._sig_port_list.size ());
			nbr_buf += nbr_reg_sig;
		}
	}

	return nbr_buf;
}



int	Router::clip_channel (int chn_idx, int nbr_chn)
{
	return std::min (chn_idx, nbr_chn - 1);
}



}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
