/*****************************************************************************

        ToolsRouting.cpp
        Author: Laurent de Soras, 2020

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

#include "mfx/doc/Preset.h"
#include "mfx/doc/Routing.h"
#include "mfx/ToolsRouting.h"

#include <algorithm>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Resulting graph contains only connected nodes (at least one connection).
// Node input and output ports (CnxPinList) only contains connected pins.
void	ToolsRouting::build_node_graph (NodeMap &graph, const CnxSet &cnx_set)
{
	graph.clear ();

	for (const auto &cnx : cnx_set)
	{
		const doc::CnxEnd &  cnx_src = cnx.use_src ();
		const doc::CnxEnd &  cnx_dst = cnx.use_dst ();
		const Node     node_src (cnx_src);
		const Node     node_dst (cnx_dst);
		const int      pin_src = cnx_src.get_pin ();
		const int      pin_dst = cnx_dst.get_pin ();

		CnxPinList &   pin_list_src = graph [node_src] [piapi::Dir_OUT];
		CnxPinList &   pin_list_dst = graph [node_dst] [piapi::Dir_IN ];

		if (pin_src >= int (pin_list_src.size ()))
		{
			pin_list_src.resize (pin_src + 1);
		}
		if (pin_dst >= int (pin_list_dst.size ()))
		{
			pin_list_dst.resize (pin_dst + 1);
		}

		pin_list_src [pin_src].insert (cnx);
		pin_list_dst [pin_dst].insert (cnx);
	}
}



bool	ToolsRouting::has_loops (const NodeMap &graph)
{
	SlotIdSet      visit_set;
	SlotIdSet      recur_set;

	for (const auto &node_vt : graph)
	{
		if (has_loops_rec (graph, node_vt, visit_set, recur_set))
		{
			return true;
		}
	}

	return false;
}



// Returns graph.end () if not found
ToolsRouting::NodeMap::const_iterator	ToolsRouting::find_audio_io (const NodeMap &graph) noexcept
{
	const NodeMap::const_iterator it = std::find_if (
		graph.begin (),
		graph.end (),
		[] (const NodeMap::value_type &node_vt)
		{
			return (node_vt.first.get_type () == doc::CnxEnd::Type_IO);
		}
	);

	return it;
}



// Returns graph.end () if not found
ToolsRouting::NodeMap::const_iterator	ToolsRouting::find_return_send (const NodeMap &graph) noexcept
{
	const NodeMap::const_iterator it = std::find_if (
		graph.begin (),
		graph.end (),
		[] (const NodeMap::value_type &node_vt)
		{
			return (node_vt.first.get_type () == doc::CnxEnd::Type_RS);
		}
	);

	return it;
}



// graph must be consistent with prog: it should not contains node that are
// not in prog.
void	ToolsRouting::build_ordered_node_lists (std::vector <int> &audio_list, std::vector <int> &sig_list, const doc::Preset &prog, const NodeMap &graph, const std::set <std::string> &aud_pi_list)
{
#if ! defined (NDEBUG)
	// Consistency check
	for (auto &node_vt : graph)
	{
		const Node &   node = node_vt.first;
		const doc::CnxEnd::Type type = node.get_type ();
		if (type == doc::CnxEnd::Type_NORMAL)
		{
			const int      slot_id = node.get_slot_id ();
			assert (prog._slot_map.find (slot_id) != prog._slot_map.end ());
		}
	}
#endif

	audio_list.clear ();
	sig_list.clear ();

	// First, extracts the signal nodes and fills a list with all other slots.
	std::set <int> unvisit_set;
	for (auto it = prog._slot_map.begin (); it != prog._slot_map.end (); ++it)
	{
		const int      slot_id = it->first;
		if (! prog.is_slot_empty (it))
		{
			const doc::Slot & slot = *(it->second);

			// Is this plug-in a signal one (not audio)?
			if (aud_pi_list.find (slot._pi_model) == aud_pi_list.end ())
			{
				assert (
					   graph.find (Node (doc::CnxEnd::Type_NORMAL, slot_id))
					== graph.end ()
				);
				sig_list.push_back (slot_id);
				continue;
			}
		}

		// Not a signal node
		unvisit_set.insert (slot_id);
	}

	// Starts traversal from the audio output pins
	const NodeMap::const_iterator it_audio_io = find_audio_io (graph);
	if (it_audio_io != graph.end ())
	{
		visit_node_rec (audio_list, unvisit_set, it_audio_io, graph);
	}

	// Same with the send pins
	const NodeMap::const_iterator it_send = find_return_send (graph);
	if (it_send != graph.end ())
	{
		visit_node_rec (audio_list, unvisit_set, it_send, graph);
	}

	// Finds all unvisited audio nodes with disconnected output
	typedef std::pair <int, NodeMap::const_iterator> SlotIdNodeIt;
	std::vector <SlotIdNodeIt> disconnected_out_set;
	for (int slot_id : unvisit_set)
	{
		const Node     node (doc::CnxEnd::Type_NORMAL, slot_id);
		const auto     it = graph.find (node);
		if (   it == graph.end ()
		    || it->second [piapi::Dir_OUT].empty ())
		{
			disconnected_out_set.push_back (std::make_pair (slot_id, it));
		}
	}

	// Starts traversals from these nodes to find all remaining sub-graphes
	// which are not connected to the main graph, including isolated nodes.
	for (const SlotIdNodeIt &p : disconnected_out_set)
	{
		if (p.second == graph.end ())
		{
			audio_list.push_back (p.first);
			unvisit_set.erase (p.first);
		}
		else
		{
			visit_node_rec (audio_list, unvisit_set, p.second, graph);
		}
	}

	assert (unvisit_set.empty ());
	assert (audio_list.size () + sig_list.size () == prog._slot_map.size ());
}



int	ToolsRouting::build_ordered_node_list (std::vector <int> &slot_id_list, bool audio_first_flag, const doc::Preset &prog, const std::vector <std::string> &pi_aud_list)
{
	slot_id_list.clear ();

	const doc::Routing::CnxSet &  cnx_set = prog.use_routing ()._cnx_audio_set;
	ToolsRouting::NodeMap   graph;
	ToolsRouting::build_node_graph (graph, cnx_set);

	std::set <std::string>  pi_aud_set;
	pi_aud_set.insert (pi_aud_list.begin (), pi_aud_list.end ());
	std::vector <int> slot_list_aud;
	std::vector <int> slot_list_sig;
	ToolsRouting::build_ordered_node_lists (
		slot_list_aud,
		slot_list_sig,
		prog,
		graph,
		pi_aud_set
	);

	int            second_list_pos = 0;

	if (audio_first_flag)
	{
		slot_id_list    = slot_list_aud;
		second_list_pos = int (slot_id_list.size ());
		slot_id_list.insert (
			slot_id_list.end (), slot_list_sig.begin (), slot_list_sig.end ()
		);
	}
	else
	{
		slot_id_list    = slot_list_sig;
		second_list_pos = int (slot_id_list.size ());
		slot_id_list.insert (
			slot_id_list.end (), slot_list_aud.begin (), slot_list_aud.end ()
		);
	}

	return second_list_pos;
}



/*
Inserts only before input pin #0 (pin_target in the code below).
If there is nothing connected to the input pin #0, find the first
connected one. If there is none, make a connection on #0 but don't
connect the input of the new node.
Other pins from slot_id_aft are left as they are.
If slot_id_aft < 0, the slot is inserted before the audio output.
*/

void	ToolsRouting::insert_slot_before (CnxSet &cnx_set, int slot_id_new, int slot_id_aft)
{
	assert (slot_id_new >= 0);

	int            pin_target    = 0;
	int            pin_cnx_new_i = 0;
	int            pin_cnx_new_o = 0;

	const doc::CnxEnd::Type type_cur =
		  (slot_id_aft < 0)
		? doc::CnxEnd::Type_IO
		: doc::CnxEnd::Type_NORMAL;

	// Lists input connections for a single pin
	CnxSet         cnx_cur_set_tgt;
	list_preferred_cnx <piapi::Dir_IN> (
		cnx_cur_set_tgt, cnx_set, slot_id_aft, pin_target
	);

	// Replaces these connections with the ones to the new slot
	for (const auto &cnx : cnx_cur_set_tgt)
	{
		const doc::CnxEnd cnx_dst (
			doc::CnxEnd::Type_NORMAL, slot_id_new, pin_cnx_new_i
		);
		const doc::Cnx cnx_new (cnx.use_src (), cnx_dst);

		cnx_set.erase (cnx);
		cnx_set.insert (cnx_new);
	}

	// New in-between connection (new -> cur), if any
	if (pin_target >= 0)
	{
		// Fixes slot_id_aft for the audio output
		if (slot_id_aft < 0)
		{
			slot_id_aft = 0;
		}

		cnx_set.insert (doc::Cnx (
			doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id_new, pin_cnx_new_o),
			doc::CnxEnd (type_cur                , slot_id_aft, pin_target   )
		));
	}
}



// Same as insert_slot_before(), but with everything reverted.
void	ToolsRouting::insert_slot_after (CnxSet &cnx_set, int slot_id_new, int slot_id_bfr)
{
	assert (slot_id_new >= 0);

	int            pin_target    = 0;
	int            pin_cnx_new_i = 0;
	int            pin_cnx_new_o = 0;

	const doc::CnxEnd::Type type_cur =
		  (slot_id_bfr < 0)
		? doc::CnxEnd::Type_IO
		: doc::CnxEnd::Type_NORMAL;

	// Lists input connections for a single pin
	CnxSet         cnx_cur_set_tgt;
	list_preferred_cnx <piapi::Dir_OUT> (
		cnx_cur_set_tgt, cnx_set, slot_id_bfr, pin_target
	);

	// Replaces these connections with the ones to the new slot
	for (const auto &cnx : cnx_cur_set_tgt)
	{
		const doc::CnxEnd cnx_src (
			doc::CnxEnd::Type_NORMAL, slot_id_new, pin_cnx_new_o
		);
		const doc::Cnx cnx_new (cnx_src, cnx.use_dst ());

		cnx_set.erase (cnx);
		cnx_set.insert (cnx_new);
	}

	// New in-between connection (cur -> new), if any
	if (pin_target >= 0)
	{
		// Fixes slot_id_bfr for the audio input
		if (slot_id_bfr < 0)
		{
			slot_id_bfr = 0;
		}

		cnx_set.insert (doc::Cnx (
			doc::CnxEnd (type_cur                , slot_id_bfr, pin_target   ),
			doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id_new, pin_cnx_new_i)
		));
	}
}



/*
Keeps only the connections from/to one pin per port.
The new connections are a product of the deleted one to preserve
the N-in to M-out cardinality.
*/

void	ToolsRouting::disconnect_slot (CnxSet &cnx_set, int slot_id)
{
	int            pin_target_i = 0;
	int            pin_target_o = 0;

	// Lists connections to a single input pin
	CnxSet         cnx_set_i;
	list_preferred_cnx <piapi::Dir_IN> (
		cnx_set_i, cnx_set, slot_id, pin_target_i
	);

	// And from a single output pin
	CnxSet         cnx_set_o;
	list_preferred_cnx <piapi::Dir_OUT> (
		cnx_set_o, cnx_set, slot_id, pin_target_o
	);

	// Delete all connections related to this slot
	CnxSet::iterator it = cnx_set.begin ();
	while (it != cnx_set.end ())
	{
		if (it->has_slot_id (slot_id))
		{
			it = cnx_set.erase (it);
		}
		else
		{
			++ it;
		}
	}

	// Replaces them with N * M connections bypassing the slot
	for (const auto &cnx_i : cnx_set_i)
	{
		for (const auto &cnx_o : cnx_set_o)
		{
			cnx_set.insert (doc::Cnx (
				cnx_i.use_src (),
				cnx_o.use_dst ()
			));
		}
	}
}



/*
The slot is moved at the given position in the audio ordered node list.
For connection choices, the priority is to keep the slot in the same
coverage area.
Then the connections follow the same rules as insert_slot_before().
*/

void	ToolsRouting::move_slot (CnxSet &cnx_set, int slot_id, int pos_aud, const std::vector <int> &slot_list_aud, const NodeMap &graph)
{
	assert (slot_id >= 0);
	assert (pos_aud >= 0);
	assert (pos_aud < int (slot_list_aud.size ()));

	const int      pos_old =
		find_linear_index_audio_graph (slot_list_aud, slot_id);

	// The slot does not move, exit.
	if (pos_old == pos_aud)
	{
		return;
	}

	// Insert before
	const int      pos_ins = (pos_old < pos_aud) ? pos_aud + 1 : pos_aud;

	// Slots before and after the insertion point
	int            slot_id_aft = -1;
	int            slot_id_bef = -1;
	if (pos_ins > 0)
	{
		slot_id_bef = slot_list_aud [pos_ins - 1];
	}
	if (pos_ins < int (slot_list_aud.size ()))
	{
		slot_id_aft = slot_list_aud [pos_ins];
	}

	// If both slots are actually connected, use a regular insertion
	if (   slot_id_bef < 0
	    || slot_id_aft < 0
	    || are_slot_connected (graph, slot_id_bef, slot_id_aft))
	{
		disconnect_slot (cnx_set, slot_id);
		insert_slot_before (cnx_set, slot_id, slot_id_aft);
	}

	// If they aren't connected, check if one of the nodes is from the same
	// subgraph.
	else
	{
		std::set <Node>   node_list_u;
		std::set <Node>   node_list_d;
		find_coverage (node_list_u, node_list_d, graph, slot_id);
		std::set <Node>   node_list;
		node_list.swap (node_list_u);
		for (auto &node : node_list_d)
		{
			node_list.insert (node);
		}

		const Node     node_bef (doc::CnxEnd::Type_NORMAL, slot_id_bef);
		const Node     node_aft (doc::CnxEnd::Type_NORMAL, slot_id_aft);

		// To the up-stream graph
		if (node_list.find (node_bef) != node_list.end ())
		{
			disconnect_slot (cnx_set, slot_id);
			cnx_set.insert (doc::Cnx (
				doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id_bef, 0),
				doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id    , 0)
			));
		}

		// To the down-stream graph
		else if (node_list.find (node_aft) != node_list.end ())
		{
			disconnect_slot (cnx_set, slot_id);
			cnx_set.insert (doc::Cnx (
				doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id    , 0),
				doc::CnxEnd (doc::CnxEnd::Type_NORMAL, slot_id_aft, 0)
			));
		}

		else
		{
			// No relevant existing connection, don't do anything.
		}
	}
}



/*
Finds all up-stream and down-stream slots, for a given slot.
The slot should exist.
Result sets may include the audio I/O and send/return nodes.
*/
void	ToolsRouting::find_coverage (std::set <Node> &node_list_u, std::set <Node> &node_list_d, const NodeMap &graph, int slot_id)
{
	assert (slot_id >= 0);

	node_list_u.clear ();
	node_list_d.clear ();

	NodeMap::const_iterator slot_it =
		graph.find (Node (doc::CnxEnd::Type_NORMAL, slot_id));
	if (slot_it != graph.end ())
	{
		find_coverage_rec <piapi::Dir_IN > (node_list_u, graph, slot_it);
		find_coverage_rec <piapi::Dir_OUT> (node_list_d, graph, slot_it);
	}
}



// Returns -1 if not found
int	ToolsRouting::find_linear_index_audio_graph (const std::vector <int> &slot_list_aud, int slot_id)
{
	assert (slot_id >= 0);

	const int      list_sz    = int (slot_list_aud.size ());
	int            found_pos  = -1;
	for (int pos = 0; pos < list_sz && found_pos < 0; ++pos)
	{
		if (slot_list_aud [pos] == slot_id)
		{
			found_pos = pos;
		}
	}

	return found_pos;
}



// Checks direct connections
bool	ToolsRouting::are_slot_connected (const NodeMap &graph, int slot1_id, int slot2_id)
{
	NodeMap::const_iterator slot_it =
		graph.find (Node (doc::CnxEnd::Type_NORMAL, slot1_id));
	if (slot_it != graph.end ())
	{
		for (int dir_cnt = 0; dir_cnt < piapi::Dir_NBR_ELT; ++dir_cnt)
		{
			const piapi::Dir     dir  = piapi::Dir (dir_cnt);
			const CnxPinList &   side = slot_it->second [dir_cnt];
			for (const auto &pin : side)
			{
				for (const auto &cnx : pin)
				{
					const doc::CnxEnd &  cnx_end = cnx.use_end (dir);
					if (   cnx_end.get_type ()    == doc::CnxEnd::Type_NORMAL
					    && cnx_end.get_slot_id () == slot2_id)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}



bool	ToolsRouting::are_audio_io_connected (const NodeMap &graph)
{
	bool           cnx_flag = false;

	NodeMap::const_iterator aio_it = graph.find (Node (doc::CnxEnd::Type_IO, 0));
	if (aio_it != graph.end ())
	{
		std::set <Node>   visit_set;
		cnx_flag = are_audio_io_connected_rec (visit_set, graph, aio_it);
	}

	return cnx_flag;
}



/*
Returns true if the slot is:
- fed by a single connection on pin 0, and
- has only audio outputs
*/
bool	ToolsRouting::is_slot_last_and_neutral (const NodeMap &graph, int slot_id)
{
	assert (slot_id >= 0);

	bool           ok_flag = true;

	NodeMap::const_iterator slot_it =
		graph.find (Node (doc::CnxEnd::Type_NORMAL, slot_id));
	if (slot_it == graph.end ())
	{
		assert (false);
		ok_flag = false;
	}
	else
	{
		const CnxPinList &   side_i = slot_it->second [piapi::Dir_IN ];
		const CnxPinList &   side_o = slot_it->second [piapi::Dir_OUT];

		// Counts the total number of connections
		size_t         nbr_cnx = 0;
		for (auto &pin : side_i)
		{
			nbr_cnx += pin.size ();
		}
		if (nbr_cnx != 1)
		{
			// Rejected if != 1
			ok_flag = false;
		}
		else if (side_i [0].empty ())
		{
			// Rejected if the only connection is not on pin 0
			ok_flag = false;
		}

		for (size_t pin_cnt = 0
		;	pin_cnt < side_o.size () && ok_flag
		;	++pin_cnt)
		{
			const CnxSet & pin = side_o [pin_cnt];
			for (auto it_cnx = pin.begin ()
			;	it_cnx != pin.end () && ok_flag
			;	++it_cnx)
			{
				if (it_cnx->use_dst ().get_type () != doc::CnxEnd::Type_IO)
				{
					ok_flag = false;
				}
			}
		}
	}

	return ok_flag;
}



ToolsRouting::Node::Node (doc::CnxEnd::Type type, int slot_id) noexcept
:	_type (type)
,	_slot_id (slot_id)
{
	assert (type >= 0);
	assert (type < doc::CnxEnd::Type_NBR_ELT);
	assert (slot_id >= 0);
}



ToolsRouting::Node::Node (const doc::CnxEnd &cnx_end) noexcept
{
	assert (cnx_end.is_valid ());

	_type    = cnx_end.get_type ();
	_slot_id = cnx_end.get_slot_id ();
}



void	ToolsRouting::Node::set (doc::CnxEnd::Type type, int slot_id) noexcept
{
	assert (type >= 0);
	assert (type < doc::CnxEnd::Type_NBR_ELT);
	assert (slot_id >= 0);

	_type    = type;
	_slot_id = slot_id;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
bool	ToolsRouting::has_loops_rec (const NodeMap &graph, const NodeMap::value_type &node_vt, SlotIdSet &visit_set, SlotIdSet &recur_set)
{
	const doc::CnxEnd::Type type = node_vt.first.get_type ();
	const int      slot_id = node_vt.first.get_slot_id ();

	if (   type != doc::CnxEnd::Type_NORMAL
	    || visit_set.find (slot_id) == visit_set.end ())
	{
		if (type == doc::CnxEnd::Type_NORMAL)
		{
			visit_set.insert (slot_id);
			recur_set.insert (slot_id);
		}

		const CnxPinList &   side = node_vt.second [piapi::Dir_IN];
		for (const auto &cnx_list : side)
		{
			for (const auto & cnx : cnx_list)
			{
				const doc::CnxEnd &  cnx_src = cnx.use_src ();
				if (cnx_src.get_type () == doc::CnxEnd::Type_NORMAL)
				{
					const int   src_slot_id = cnx_src.get_slot_id ();
					if (visit_set.find (src_slot_id) == visit_set.end ())
					{
						const NodeMap::value_type &   src_node_vt =
							*graph.find (Node (cnx_src));
						if (has_loops_rec (graph, src_node_vt, visit_set, recur_set))
						{
							return true;
						}
					}
					if (recur_set.find (src_slot_id) != recur_set.end ())
					{
						return true;
					}
				}
			}
		}

		if (type == doc::CnxEnd::Type_NORMAL)
		{
			recur_set.erase (slot_id);
		}
	}

	return false;
}



void	ToolsRouting::visit_node_rec (std::vector <int> &slot_id_list, std::set <int> &unvisit_set, const NodeMap::const_iterator it_node, const NodeMap &graph)
{
	const Node &   node    = it_node->first;
	const int      slot_id = node.get_slot_id ();
	const doc::CnxEnd::Type type = node.get_type ();
	assert (   type != doc::CnxEnd::Type_NORMAL
	        || unvisit_set.find (node.get_slot_id ()) != unvisit_set.end ());

	const CnxPinList &   pin_list = it_node->second [piapi::Dir_IN];
	for (const auto &pin_cnx : pin_list)
	{
		for (const auto &cnx : pin_cnx)
		{
			const doc::CnxEnd &  cnx_src = cnx.use_src ();
			const doc::CnxEnd::Type type_src = cnx_src.get_type ();
			if (type_src == doc::CnxEnd::Type_NORMAL)
			{
				const Node &   node_src    = Node (cnx_src);
				assert (node_src.get_type () == doc::CnxEnd::Type_NORMAL);
				const int      slot_id_src = node_src.get_slot_id ();
				if (unvisit_set.find (slot_id_src) != unvisit_set.end ())
				{
					const NodeMap::const_iterator it_node_src =
						graph.find (node_src);
					assert (it_node_src != graph.end ());
					visit_node_rec (slot_id_list, unvisit_set, it_node_src, graph);
				}
			}
		}
	}

	if (type == doc::CnxEnd::Type_NORMAL)
	{
		slot_id_list.push_back (slot_id);
		unvisit_set.erase (slot_id);
	}
}



/*
Lists input or output connections to a single pin, with a preference for
pin_target.
slot_id < 0: audio output or input
*/

template <piapi::Dir D>
void	ToolsRouting::list_preferred_cnx (CnxSet &result, const CnxSet &cnx_set, int slot_id, int &pin_target)
{
	static const piapi::Dir  DI = piapi::Dir_Inv <D>::_dir;

	const Node     node_cur =
		  (slot_id < 0)
		? Node (doc::CnxEnd::Type_IO, 0)
		: Node (doc::CnxEnd::Type_NORMAL, slot_id);

	// First, lists all connections to the slot and sorts them
	// between the target pin and the other pins.
	CnxSet         cnx_cur_set_oth;
	for (const auto &cnx : cnx_set)
	{
		const doc::CnxEnd &  cnx_end = cnx.use_end <DI> ();
		if (Node (cnx_end) == node_cur)
		{
			if (cnx_end.get_pin () == pin_target)
			{
				result.insert (cnx);
			}
			else
			{
				cnx_cur_set_oth.insert (cnx);
			}
		}
	}

	// Finds a pin. If there is no connected pin, pin_target becomes negative.
	// result collects all the connections to the pin.
	if (result.empty ())
	{
		if (cnx_cur_set_oth.empty ())
		{
			pin_target = -1;
		}
		else
		{
			pin_target = cnx_cur_set_oth.begin ()->use_end <DI> ().get_pin ();
			for (const auto &cnx : cnx_cur_set_oth)
			{
				if (cnx.use_end <DI> ().get_pin () == pin_target)
				{
					result.insert (cnx);
				}
			}
		}
	}
}



template <piapi::Dir D>
void	ToolsRouting::find_coverage_rec (std::set <Node> &visit_set, const NodeMap &graph, NodeMap::const_iterator slot_it)
{
	const Node &   node     = slot_it->first;
	const bool     ins_flag = visit_set.insert (node).second;
	if (ins_flag)
	{
		if (node.get_type () == doc::CnxEnd::Type_NORMAL)
		{
			const CnxPinList &   side = slot_it->second [D];
			for (const CnxSet &pin : side)
			{
				for (const doc::Cnx &cnx : pin)
				{
					const NodeMap::const_iterator it_nxt =
						graph.find (Node (cnx.use_end <D> ()));
					if (it_nxt == graph.end ())
					{
						assert (false);
					}
					else
					{
						find_coverage_rec <D> (visit_set, graph, it_nxt);
					}
				}
			}
		}
	}
}



bool	ToolsRouting::are_audio_io_connected_rec (std::set <Node> &visit_set, const NodeMap &graph, NodeMap::const_iterator slot_it)
{
	const CnxPinList &   side = slot_it->second [piapi::Dir_IN];
	for (const CnxSet &pin : side)
	{
		for (const doc::Cnx &cnx : pin)
		{
			const doc::CnxEnd &  cnx_src = cnx.use_src ();
			if (cnx_src.get_type () == doc::CnxEnd::Type_IO)
			{
				return true;
			}
			else
			{
				const NodeMap::const_iterator it_prev =
					graph.find (Node (cnx.use_src ()));
				assert (it_prev != graph.end ());
				if (are_audio_io_connected_rec (visit_set, graph, it_prev))
				{
					return true;
				}
			}
		}
	}

	return false;
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
