/*****************************************************************************

        ProcessingContextNode.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Crc32.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/PluginPool.h"
#include "mfx/ProcessingContextNode.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ProcessingContextNode::ProcessingContextNode ()
:	_pi_id (-1)
,	_side_arr ()
,	_sig_buf_arr ()
,	_nbr_sig (0)
,	_aux_param_flag (false)
,	_comp_delay (0)
,	_pin_mult (1)
{
	for (auto & side : _side_arr)
	{
		side._nbr_chn     = 0;
		side._nbr_chn_tot = 0;
		for (auto & buf_index : side._buf_arr)
		{
			buf_index = -1;
		}
	}
}



void	ProcessingContextNode::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (const auto &side : _side_arr)
	{
		side.compute_graph_crc (crc);
	}
}



int &	ProcessingContextNode::Side::use_buf (int pin, int chn)
{
	assert (pin >= 0);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	const int      index = pin * _nbr_chn + chn;
	assert (index < _nbr_chn_tot);

	return _buf_arr [index];
}



const int &	ProcessingContextNode::Side::use_buf (int pin, int chn) const
{
	assert (pin >= 0);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	const int      index = pin * _nbr_chn + chn;
	assert (index < _nbr_chn_tot);

	return _buf_arr [index];
}



void	ProcessingContextNode::Side::compute_graph_crc (fstb::Crc32 &crc) const
{
	for (int chn_cnt = 0; chn_cnt < _nbr_chn_tot; ++chn_cnt)
	{
		// The 8 first bits are enough.
		crc.process_byte (_buf_arr [chn_cnt]);
	}
}



std::string	ProcessingContextNode::dump_as_str (const PluginPool &plugin_pool) const
{
	std::string    out;
	const std::string indent = "  ";

	char           txt_0 [1023+1];

	// Plug-in identifier and category
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"Plug-in id: %d", _pi_id
	); out += indent + txt_0;
	if (_pi_id >= 0)
	{
		const PluginDetails &   details = plugin_pool.use_plugin (_pi_id);
		assert (details._desc_ptr != nullptr);
		const piapi::PluginDescInterface &  pi_desc = *details._desc_ptr;
		const piapi::PluginInfo pi_info { pi_desc.get_info () };
		const std::string pi_categ     = pi_info._unique_id;
		const std::string pi_name_mlab = pi_info._name;
		const std::string pi_name      =
			pi::param::Tools::extract_longest_str (pi_name_mlab.c_str (), '\n');
		out += " (" + pi_categ + ", " + pi_name + ")";
	}
	out += "\n";

	// Audio buffers
	for (int dir = 0; dir < Dir_NBR_ELT; ++dir)
	{
		static const std::array <const char *, Dir_NBR_ELT>   dir_0_arr =
		{{ "In ", "Out" }};

		const Side &   side = _side_arr [dir];
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"%s: chn = %d, tot = %d\n",
			dir_0_arr [dir], side._nbr_chn, side._nbr_chn_tot
		); out += indent + txt_0;
		if (side._nbr_chn > 0)
		{
			out += indent + "  ";
			for (int chn_cnt = 0; chn_cnt < side._nbr_chn_tot; ++chn_cnt)
			{
				if (chn_cnt > 0 && chn_cnt % side._nbr_chn == 0)
				{
					out += " /";
				}
				fstb::snprintf4all (txt_0, sizeof (txt_0),
					" %d",
					side._buf_arr [chn_cnt]
				); out += txt_0;
			}
			out += "\n";
		}
	}

	// Signals
	if (_nbr_sig > 0)
	{
		out += indent + "Signals (buf:port):";
		for (int sig_cnt = 0; sig_cnt < _nbr_sig; ++sig_cnt)
		{
			const SigInfo &   sig_info = _sig_buf_arr [sig_cnt];
			fstb::snprintf4all (txt_0, sizeof (txt_0),
				" %d:%d",
				sig_info._buf_index, sig_info._port_index
			); out += txt_0;
		}
		out += "\n";
	}

	// Auxiliary parameters
	if (_aux_param_flag)
	{
		fstb::snprintf4all (txt_0, sizeof (txt_0),
			"Aux param: delay = %d, pins = %d\n",
			_comp_delay, _pin_mult
		); out += indent + txt_0;
		out += "\n";
	}

	return out;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
