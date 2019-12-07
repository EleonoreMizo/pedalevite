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
	create_routing_chain (doc, plugin_pool);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Router::create_routing_chain (Document &doc, PluginPool &plugin_pool)
{
	ProcessingContext &  ctx = *doc._ctx_sptr;

	// Final number of channels
	int            nbr_chn_cur   = 1;
	int            nbr_chn_final = 1;
	switch (doc._chn_mode)
	{
	case ChnMode_1M_1M:
		nbr_chn_final = 1;
		break;
	case ChnMode_1M_1S:
		nbr_chn_final = 2;
		break;
	case ChnMode_1S_1S:
		nbr_chn_cur   = 2;
		nbr_chn_final = 2;
		break;
	default:
		assert (false);
		break;
	}
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
	for (Slot & slot : doc._slot_list)
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
			slot._ctx_index = int (ctx._context_arr.size ());
			ctx._context_arr.resize (slot._ctx_index + 1);
			ProcessingContext::PluginContext &  pi_ctx = ctx._context_arr.back ();
			pi_ctx._mixer_flag = (pi_id_mix >= 0);
			ProcessingContextNode & ctx_node_main = pi_ctx._node_arr [PiType_MAIN];

			// Main plug-in
			ctx_node_main._pi_id = pi_id_main;

			ctx_node_main._mix_in_arr.clear ();

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

				ctx_node_mix._mix_in_arr.clear ();

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
					ctx_node_main._bypass_buf_arr [chn] = buf_alloc.alloc ();
				}

				// Dry/wet input
				mix_side_i._nbr_chn     = nbr_chn_out;
				mix_side_i._nbr_chn_tot = nbr_chn_out * 2;
				for (int chn = 0; chn < nbr_chn_out; ++chn)
				{
					// 1st pin: main output
					mix_side_i._buf_arr [              chn] = nxt_buf_arr [chn];

					// 2nd pin: main input as default bypass
					const int       chn_in = std::min (chn, nbr_chn_in - 1);
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
					buf_alloc.ret (ctx_node_main._bypass_buf_arr [chn]);
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
		const int      chn_src = std::min (i, nbr_chn_cur - 1);
		audio_o._buf_arr [i] = cur_buf_arr [chn_src];
	}

	for (int chn = 0; chn < nbr_chn_cur; ++chn)
	{
		buf_alloc.ret (cur_buf_arr [chn]);
	}
}



void	Router::create_routing_graph (Document &doc, PluginPool &plugin_pool)
{
	make_graph_from_chain (doc);

	add_aux_plugins (doc, plugin_pool);
	create_graph_context (doc, plugin_pool);
}



// Completes the Document structure to turn the implicit slot chain into
// a true graph.
void	Router::make_graph_from_chain (Document &doc)
{
	doc._cnx_list.clear ();

	const int      nbr_slots = int (doc._slot_list.size ());
	int            slot_src  = CnxEnd::SlotType_IO;
	for (int slot_pos = 0; slot_pos < nbr_slots; ++slot_pos)
	{
		const Slot &   slot = doc._slot_list [slot_pos];
		const int      pi_id_main = slot._component_arr [PiType_MAIN]._pi_id;
		if (pi_id_main >= 0)
		{
			// Connects the plug-in to the source
			Cnx            cnx;
			cnx._src._slot_pos = slot_src;
			cnx._src._pin      = 0;
			cnx._dst._slot_pos = slot_pos;
			cnx._dst._pin      = 0;
			doc._cnx_list.push_back (cnx);

			// The plug-in becomes the new source only if it generates audio
			if (slot._gen_audio_flag)
			{
				slot_src = slot_pos;
			}
		}
	}

	// Last connection to the audio output
	Cnx            cnx;
	cnx._src._slot_pos = slot_src;
	cnx._src._pin      = 0;
	cnx._dst._slot_pos = CnxEnd::SlotType_IO;
	cnx._dst._pin      = 0;
	doc._cnx_list.push_back (cnx);
}



void	Router::add_aux_plugins (Document &doc, PluginPool &plugin_pool)
{
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

	// Computes the plug-in latencies across the graph to evaluate where
	// to insert compensation delay plug-ins
	prepare_graph_for_latency_analysis (doc);
	_lat_algo.run ();

	// Finds multiple connections to single input pins
	count_nbr_cnx_per_input_pin (_cnx_per_pin_in, doc._cnx_list);

	// Inserts required plug-ins: delays for latency compensation
	add_aux_plugins_delays (doc, plugin_pool);

	// Connects the plug-ins to the graph
	_cnx_list = doc._cnx_list;
	connect_delays (doc);
}



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

		int            node_idx_src =
			conv_doc_slot_to_lat_node_index (piapi::Dir_IN , cnx_doc);
		cnx_lat.set_node (piapi::Dir_IN , node_idx_src);

		int            node_idx_dst =
			conv_doc_slot_to_lat_node_index (piapi::Dir_OUT, cnx_doc);
		cnx_lat.set_node (piapi::Dir_OUT, node_idx_dst);

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
}



/*
Maps the document slot indexes to lat::Algo node indexes.
In this order:
- Standard plug-in slots
- Audio inputs
- Audio outputs
*/

int	Router::conv_doc_slot_to_lat_node_index (piapi::Dir dir, const Cnx &cnx)
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
	case CnxEnd::SlotType_MIX:
		// These kinds of slot shouldn't appear here.
		assert (false);
		break;
	default:
		assert (false);
		break;
	}

	return node_index;
}



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



void	Router::create_graph_context (Document &doc, PluginPool &plugin_pool)
{


	/*** To do ****/
	assert (false);


}



void	Router::count_nbr_cnx_per_input_pin (MapCnxPerPin &res_map, const Document::CnxList &graph)
{
	res_map.clear ();

	// Counts all connections
	for (auto &cnx : graph)
	{
		const CnxEnd & pin = cnx._dst;
		auto           p   = res_map.insert (std::make_pair (pin, 0));
		++ p.first->second;
	}

	// Now removes single connections
	MapCnxPerPin::iterator  it = res_map.begin ();
	while (it != res_map.end ())
	{
		if (it->second <= 1)
		{
			it = res_map.erase (it);
		}
		else
		{
			++ it;
		}
	}
}



}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
