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

class PluginPool;

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

	typedef std::map <CnxEnd, int> MapCnxPerPin;

	class CnxInfo
	{
	public:
		int            _delay   = 0;
		int            _cnx_idx = -1;
	};

	void           create_routing_chain (Document &doc, PluginPool &plugin_pool);
	void           create_routing_graph (Document &doc, PluginPool &plugin_pool);
	void           make_graph_from_chain (Document &doc);
	void           add_aux_plugins (Document &doc, PluginPool &plugin_pool);
	void           prepare_graph_for_latency_analysis (const Document &doc);
	int            conv_doc_slot_to_lat_node_index (piapi::Dir dir, const Cnx &cnx);
	void           add_aux_plugins_delays (Document &doc, PluginPool &plugin_pool);
	PluginAux &    create_plugin_aux (Document &doc, PluginPool &plugin_pool, Document::PluginAuxList &aux_list, std::string model);
	void           connect_delays (Document &doc);
	void           create_graph_context (Document &doc, PluginPool &plugin_pool);

	static void    count_nbr_cnx_per_input_pin (MapCnxPerPin &res_map, const Document::CnxList &graph);

	// Sampling rate, Hz. > 0. 0 = not known yet
	double         _sample_freq    = 0;

	// Maximum processing length, samples. > 0. 0 = not known yet
	int            _max_block_size = 0;

	lat::Algo      _lat_algo;
	int            _nbr_slots      = 0; // From Document::_slot_list
	int            _nbr_a_src      = 1; // Number of audio input pins
	int            _nbr_a_dst      = 1; // Number of audio output pins

	// [input pin] = number of incoming connections. After running the analysis,
	// it only contains pins with multiple connections
	MapCnxPerPin   _cnx_per_pin_in;

	// Same as Document::_cnx_list, but includes delay and mixer plug-ins
	// When a connection requires both a mixer and a delay, the delay is
	// inserted first.
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
