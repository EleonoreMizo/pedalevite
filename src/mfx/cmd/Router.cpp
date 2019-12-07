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
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"

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









}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
