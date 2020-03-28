/*****************************************************************************

        ProcessingContext.cpp
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

#include "fstb/Crc32.h"
#include "fstb/fnc.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContext.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProcessingContext::compute_graph_crc (fstb::Crc32 &crc) const
{
	crc.reset ();

	// 8 bits are enough
	crc.process_byte (_nbr_chn_out);

	_interface_ctx.compute_graph_crc (crc);
	PluginContext::compute_graph_crc_mix_i (crc, _interface_mix);

	for (const auto &pi : _context_arr)
	{
		if (pi._mixer_flag)
		{
			pi.compute_graph_crc (crc);
		}
	}
}



void	ProcessingContext::PluginContext::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (auto &c : _pi_model)
	{
		crc.process_byte (c);
	}

	PluginContext::compute_graph_crc_mix_i (crc, _mix_in_arr);

	_node_arr [PiType_MAIN].compute_graph_crc (crc);
	if (_mixer_flag)
	{
		_node_arr [PiType_MIX].compute_graph_crc (crc);
	}
}



void	ProcessingContext::PluginContext::compute_graph_crc_mix_i (fstb::Crc32 &crc, const MixInputArray &mix_in_arr)
{
	if (! mix_in_arr.empty ())
	{
		for (const auto &chn : mix_in_arr)
		{
			for (const int buf : chn)
			{
				// Clipping to 8 bits is OK.
				crc.process_byte (buf);
			}
		}
	}
}



std::string	ProcessingContext::dump_as_str (const PluginPool &plugin_pool) const
{
	std::string    out;
	char           txt_0 [255+1];

	out += "[Begin]\n";

	// General information
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"Master vol  : %.3f (linear)\n"
		"Nbr chn out : %d\n"
		"Graph change: %s\n",
		_master_vol,
		_nbr_chn_out,
		(_graph_changed_flag ? "yes" : "no")
	); out += txt_0;

	// Controllers
	out += dump_ctrl (plugin_pool);

	// Audio in/out
	out += "* Audio interface node:\n";
	out += _interface_ctx.dump_as_str (plugin_pool);
	if (! _interface_mix.empty ())
	{
		out += "Audio intput mix: " + dump_input_mix (_interface_mix);
	}
	out += "\n";

	// Plug-in nodes
	for (int c_cnt = 0; c_cnt < int (_context_arr.size ()); ++c_cnt)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"=== Plug-in %d ============\n", c_cnt
		); out += txt_0;
		out += dump_pi_ctx (_context_arr [c_cnt], plugin_pool) + "\n";
	}

	out += "[End]\n\n";

	return out;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	ProcessingContext::dump_input_mix (const PluginContext::MixInputArray &mia) const
{
	std::string    out;
	char           txt_0 [255+1];

	bool           first_flag = true;
	for (const PluginContext::MixInChn &pin : mia)
	{
		if (! first_flag)
		{
			out += " /";
		}
		for (const int buf_src : pin)
		{
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				" %d", buf_src
			); out += txt_0;
		}
		first_flag = false;
	}
	out += "\n";

	return out;
}



std::string	ProcessingContext::dump_pi_ctx (const PluginContext &pi_ctx, const PluginPool &plugin_pool) const
{
	std::string    out;
	char           txt_0 [255+1];

	// Model
	out += "Model: " + pi_ctx._pi_model;
	if (pi_ctx._mixer_flag)
	{
		out += " + mixer";
	}
	out += "\n";

	// Input mix
	if (! pi_ctx._mix_in_arr.empty ())
	{
		out += "Input mix: " + dump_input_mix (pi_ctx._mix_in_arr);
	}

	// Buffer for the bypass output
	if (   pi_ctx._mixer_flag
	    && pi_ctx._bypass_buf_arr.empty ()
	    && pi_ctx._bypass_buf_arr [0] >= 0)
	{
		out += "Bypass buffers:";
		const ProcessingContextNode::Side & side_o =
			pi_ctx._node_arr [PiType_MAIN]._side_arr [Dir_OUT];
		bool        first_flag = true;
		for (int chn_cnt = 0; chn_cnt < side_o._nbr_chn_tot; ++chn_cnt)
		{
			if (   ! first_flag
			    && side_o._nbr_chn > 0
			    && (chn_cnt % side_o._nbr_chn) == 0)
			{
				out += " /";
			}
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				" %d",
				pi_ctx._bypass_buf_arr [chn_cnt]
			); out += txt_0;
			first_flag = false;
		}
		out += "\n";
	}

	// Nodes
	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		static const std::array <const char *, PiType_NBR_ELT>   type_0_arr =
		{{ "Main", "Mixer" }};

		if (pi_ctx._mixer_flag || type_cnt == PiType_MAIN)
		{
			out += "* ";
			out += type_0_arr [type_cnt];
			out += " node:\n";
			out += pi_ctx._node_arr [type_cnt].dump_as_str (plugin_pool);
		}
	}

	return out;
}



std::string	ProcessingContext::dump_ctrl (const PluginPool &plugin_pool) const
{
	const std::string indent = "  ";

	std::string    out;

	if (! _map_param_ctrl.empty ())
	{
		out += "* Controllers:\n";
		char           txt_0 [255+1];

		for (auto vt_pc : _map_param_ctrl)
		{
			assert (vt_pc.second.get () != nullptr);
			const ParamCoord &   pc = vt_pc.first;
			const ControlledParam & cp = *vt_pc.second;

			// Parameter coordinates
			assert (pc._plugin_id >= 0);
			const PluginPool::PluginDetails &   details =
				plugin_pool.use_plugin (pc._plugin_id);
			assert (details._desc_ptr != nullptr);
			const piapi::PluginDescInterface &  pi_desc = *details._desc_ptr;
			const piapi::PluginInfo pi_info { pi_desc.get_info () };
			const std::string pi_categ     = pi_info._unique_id;
			const piapi::ParamDescInterface &   param_desc =
				pi_desc.get_param_info (piapi::ParamCateg_GLOBAL, pc._param_index);
			const std::string param_name = param_desc.get_name (10);
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				"Id: %4d (%-10s), idx: %3d (%-10s)",
				pc._plugin_id, pi_categ.c_str (),
				pc._param_index, param_name.c_str ()
			); out += indent + txt_0;

			// Controllers
			const ControlledParam::CtrlUnitList & cul = cp.use_unit_list ();
			bool        first_flag = true;
			for (const auto ctrl_sptr : cul)
			{
				const CtrlUnit &  ctrl = *ctrl_sptr;
				if (ctrl._abs_flag)
				{
					assert (first_flag);
					out += " -> ";
				}
				else
				{
					if (first_flag)
					{
						out += "\n";
					}
					out += indent + indent;
				}

				fstb::snprintf4all (txt_0, sizeof (txt_0),
					"Type = %d, index = %d",
					ctrl._source._type, ctrl._source._index
				); out += txt_0;
				if (ctrl.is_src_clipped ())
				{
					out += ", clip";
				}
				if (ctrl._u2b_flag)
				{
					out += ", uni->bi";
				}
				if (! ctrl._notch_list.empty ())
				{
					out += ", notches";
				}
				out += "\n";

				first_flag = false;
			}
		}
	}

	return out;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
