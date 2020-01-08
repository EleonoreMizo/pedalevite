/*****************************************************************************

        ToolsRouting.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ToolsRouting_HEADER_INCLUDED)
#define mfx_ToolsRouting_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Cnx.h"
#include "mfx/doc/CnxEnd.h"
#include "mfx/piapi/Dir.h"

#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>



namespace mfx
{

namespace doc
{
	class Preset;
	class Routing;
}


class ToolsRouting
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::set <int> SlotIdSet;

	class Node
	{
	public:
		explicit       Node (doc::CnxEnd::Type type, int slot_id);
		explicit       Node (const doc::CnxEnd &cnx_end);
		               Node ()                        = default;
		               Node (const Node &other)       = default;
		virtual        ~Node ()                       = default;
		Node &         operator = (const Node &other) = default;

		inline bool    operator == (const Node &other) const;
		inline bool    operator != (const Node &other) const;

		void           set (doc::CnxEnd::Type type, int slot_id);
		inline bool    is_valid () const;
		inline doc::CnxEnd::Type
		               get_type () const;
		inline int     get_slot_id () const;

	private:
		doc::CnxEnd::Type
		            _type;
		int         _slot_id;
	};

	typedef std::set <doc::Cnx> CnxSet;
	typedef std::vector <CnxSet> CnxPinList;
	typedef std::array <CnxPinList, piapi::Dir_NBR_ELT> NodeCnx;
	typedef std::map <Node, NodeCnx> NodeMap;

	static void    build_node_graph (NodeMap &graph, const CnxSet &cnx_set);
	static bool    has_loops (const NodeMap &graph);
	static NodeMap::const_iterator
	               find_audio_io (const NodeMap &graph);
	static void    build_ordered_node_lists (std::vector <int> &audio_list, std::vector <int> &sig_list, const doc::Preset &prog, const NodeMap &graph, const std::set <std::string> &aud_pi_list);
	static int     build_ordered_node_list (std::vector <int> &slot_id_list, bool audio_first_flag, const doc::Preset &prog, const std::vector <std::string> &pi_aud_list);
	static void    insert_slot_before (CnxSet &cnx_set, int slot_id_new, int slot_id_aft);
	static void    disconnect_slot (CnxSet &cnx_set, int slot_id);
	static void    move_slot (CnxSet &cnx_set, int slot_id, int pos_aud, const std::vector <int> &slot_list_aud, const NodeMap &graph);
	static void    find_coverage (std::set <Node> &node_list_u, std::set <Node> &node_list_d, const NodeMap &graph, int slot_id);
	static int     find_linear_index_audio_graph (const std::vector <int> &slot_list_aud, int slot_id);
	static bool    are_slot_connected (const NodeMap &graph, int slot1_id, int slot2_id);
	static bool    are_audio_io_connected (const NodeMap &graph);
	static bool    is_slot_last_and_neutral (const NodeMap &graph, int slot_id);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static bool    has_loops_rec (const NodeMap &graph, const NodeMap::value_type &node_vt, SlotIdSet &visit_set, SlotIdSet &recur_set);
	static void    visit_node_rec (std::vector <int> &slot_id_list, std::set <int> &unvisit_set, const NodeMap::const_iterator it_node, const NodeMap &graph);
	template <piapi::Dir D>
	static void    list_preferred_cnx (CnxSet &result, const CnxSet &cnx_set, int slot_id, int &pin_target);
	template <piapi::Dir D>
	static void    find_coverage_rec (std::set <Node> &visit_set, const NodeMap &graph, NodeMap::const_iterator slot_it);
	static bool    are_audio_io_connected_rec (std::set <Node> &visit_set, const NodeMap &graph, NodeMap::const_iterator slot_it);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ToolsRouting ()                               = delete;
	               ToolsRouting (const ToolsRouting &other)      = delete;
	virtual        ~ToolsRouting ()                              = delete;
	ToolsRouting & operator = (const ToolsRouting &other)        = delete;
	bool           operator == (const ToolsRouting &other) const = delete;
	bool           operator != (const ToolsRouting &other) const = delete;

}; // class ToolsRouting



inline bool    operator < (const ToolsRouting::Node &lhs, const ToolsRouting::Node &rhs);



}  // namespace mfx



#include "mfx/ToolsRouting.hpp"



#endif   // mfx_ToolsRouting_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
