/*****************************************************************************

        Preset.cpp
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

#include "mfx/doc/Preset.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/ToolsRouting.h"

#include <utility>

#include <cassert>
#include <climits>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Preset::Preset ()
{
	_routing._cnx_audio_set.insert (Cnx (
		CnxEnd (CnxEnd::Type_IO, 0, 0),
		CnxEnd (CnxEnd::Type_IO, 0, 0)
	));
}



Preset::Preset (const Preset &other)
:	_slot_map (other._slot_map)
,	_name (other._name)
,	_layout (other._layout)
,	_port_map (other._port_map)
,	_prog_switch_mode (other._prog_switch_mode)
,	_routing (other._routing)
{
	duplicate_slot_list ();
}



Preset &	Preset::operator = (const Preset &other)
{
	if (&other != this)
	{
		_slot_map = other._slot_map;
		_name     = other._name;
		_layout   = other._layout;
		_port_map = other._port_map;
		_prog_switch_mode = other._prog_switch_mode;
		_routing  = other._routing;
		duplicate_slot_list ();
	}

	return *this;
}



bool	Preset::is_slot_empty (int slot_id) const
{
	assert (slot_id >= 0);
	const auto     it = _slot_map.find (slot_id);

	return is_slot_empty (it);
}



bool	Preset::is_slot_empty (SlotMap::const_iterator it) const
{
	assert (it != _slot_map.end ());

	return is_slot_empty (*it);
}



bool	Preset::is_slot_empty (SlotMap::value_type vt) const
{
	return (   vt.second.get () == nullptr
	        || vt.second->is_empty ());
}



Slot &	Preset::use_slot (int slot_id)
{
	const auto     it = _slot_map.find (slot_id);
	if (it == _slot_map.end ())
	{
		assert (false);
		throw std::logic_error ("Slot does not exist.");
	}
	if (it->second.get () == nullptr)
	{
		assert (false);
		throw std::logic_error ("Slot is empty.");
	}

	return *(it->second);
}



const Slot &	Preset::use_slot (int slot_id) const
{
	const auto     it = _slot_map.find (slot_id);
	if (it == _slot_map.end ())
	{
		assert (false);
		throw std::logic_error ("Slot does not exist.");
	}
	if (it->second.get () == nullptr)
	{
		assert (false);
		throw std::logic_error ("Slot is empty.");
	}

	return *(it->second);
}



int	Preset::gen_slot_id () const
{
	int            slot_id = 0;

	if (! _slot_map.empty ())
	{
		slot_id = _slot_map.rbegin ()->first;
		do
		{
			static const int  inc = 11;
			if (slot_id >= INT_MAX - (inc - 1))
			{
				slot_id = 0;
			}
			else
			{
				slot_id += inc;
			}
		}
		while (_slot_map.find (slot_id) != _slot_map.end ());
	}

	return slot_id;
}



int	Preset::find_free_port () const
{
	int            port_index = 0;
	auto           it_port    = _port_map.begin ();
	while (it_port != _port_map.end () && port_index == it_port->first)
	{
		++ it_port;
		++ port_index;
	}

	return port_index;
}



bool	Preset::check_routing (const Routing &routing) const
{
	// Checks that all referenced nodes exist
	for (const auto &cnx : routing._cnx_audio_set)
	{
		if (! cnx.is_valid ())
		{
			assert (false);
			return false;
		}

		const CnxEnd & cnx_src = cnx.use_src ();
		if (! check_routing_cnx_audio_end (cnx_src))
		{
			return false;
		}

		const CnxEnd & cnx_dst = cnx.use_dst ();
		if (! check_routing_cnx_audio_end (cnx_dst))
		{
			return false;
		}
	}

	// Check loops
	ToolsRouting::NodeMap   graph;
	ToolsRouting::build_node_graph (graph, routing._cnx_audio_set);
	if (ToolsRouting::has_loops (graph))
	{
		return false;
	}

	return true;
}



void	Preset::set_routing (const Routing &routing)
{
	assert (check_routing (routing));

	_routing = routing;
}



const Routing &	Preset::use_routing () const
{
	return _routing;
}



void	Preset::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_name);
	ser.write (_prog_switch_mode);

	_layout.ser_write (ser);

	ser.begin_list ();
	for (const auto &node : _slot_map)
	{
		ser.begin_list ();
		ser.write (node.first);
		node.second->ser_write (ser);
		ser.end_list ();
	}
	ser.end_list ();

	_routing.ser_write (ser);

	ser.begin_list ();
	for (const auto &node : _port_map)
	{
		ser.begin_list ();
		ser.write (node.first);
		node.second.ser_write (ser);
		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



void	Preset::ser_read (SerRInterface &ser)
{
	const int      doc_ver = ser.get_doc_version ();
	static const int  inc  = 11;

	ser.begin_list ();

	ser.read (_name);

	_prog_switch_mode = ProgSwitchMode::DIRECT;
	if (doc_ver >= 6)
	{
		ser.read (_prog_switch_mode);
	}

	_layout.ser_read (ser);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_slot_map.clear ();
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		int         slot_id = -1;

		if (doc_ver < 3)
		{
			slot_id = cnt * inc;
		}
		else
		{
			ser.begin_list ();

			ser.read (slot_id);
		}

		SlotSPtr    s_sptr { std::make_shared <Slot> () };
		s_sptr->ser_read (ser);

		_slot_map [slot_id] = s_sptr;

		if (doc_ver >= 3)
		{
			ser.end_list ();
		}
	}
	ser.end_list ();

	if (doc_ver >= 3)
	{
		_routing.ser_read (ser);
	}
	else
	{
		_routing = Routing ();
		std::vector <int> chain (nbr_elt);
		for (int cnt = 0; cnt < nbr_elt; ++ cnt)
		{
			chain [cnt] = cnt * inc;
		}
		Routing::build_from_audio_chain (_routing._cnx_audio_set, chain);
	}

	_port_map.clear ();
	if (doc_ver >= 4)
	{
		ser.begin_list (nbr_elt);
		for (int cnt = 0; cnt < nbr_elt; ++cnt)
		{
			ser.begin_list ();

			int            port_index;
			ser.read (port_index);

			SignalPort     port;
			port.ser_read (ser);

			_port_map [port_index] = port;

			ser.end_list ();
		}
		ser.end_list ();
	}

	ser.end_list ();

	assert (check_routing (_routing));

	// With old chain routing, we need to fix the converted routing for all
	// the analysis plug-ins with no output, so the audio chain is not broken.
	if (doc_ver < 10)
	{
		fix_routing_converted_from_chain ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Preset::duplicate_slot_list ()
{
	for (auto & node : _slot_map)
	{
		if (node.second.get () != nullptr)
		{
			node.second = std::make_shared <Slot> (*(node.second));
		}
	}
}



bool	Preset::check_routing_cnx_audio_end (const CnxEnd &cnx_end) const
{
	if (cnx_end.get_type () == CnxEnd::Type_NORMAL)
	{
		const int      slot_id = cnx_end.get_slot_id ();
		if (_slot_map.find (slot_id) == _slot_map.end ())
		{
			assert (false);
			return false;
		}
	}

	return true;
}



void	Preset::fix_routing_converted_from_chain ()
{
	// Finds the slot requiring a fix
	std::set <int> slot_fix_set;
	for (SlotMap::const_iterator it = _slot_map.begin ()
	;	it != _slot_map.end ()
	;	++it)
	{
		if (! is_slot_empty (it))
		{
			const std::string pi_model = it->second->_pi_model;
			if (is_plugin_requiring_routing_fix (pi_model))
			{
				slot_fix_set.insert (it->first);
			}
		}
	}

	// Fix the connections
	if (! slot_fix_set.empty ())
	{
		Routing::CnxSet   cnx_set;
		for (Cnx cnx : _routing._cnx_audio_set)
		{
			bool        retry_flag = false;
			do
			{
				retry_flag = false;
				const CnxEnd & src = cnx.use_src ();
				const CnxEnd::Type   type = src.get_type ();
				const int      slot_id = src.get_slot_id ();
				if (   type == CnxEnd::Type_NORMAL
					 && slot_fix_set.find (slot_id) != slot_fix_set.end ())
				{
					// Connection from a node requiring a fix: uses the upstream
					// source
					const auto  up_it = std::find_if (
						_routing._cnx_audio_set.begin (),
						_routing._cnx_audio_set.end (),
						[slot_id] (const Cnx &up_cnx)
						{
							const CnxEnd &    up_dst = up_cnx.use_dst ();
							return (
								   up_dst.get_type ()    == CnxEnd::Type_NORMAL
								&& up_dst.get_slot_id () == slot_id
							);
						}
					);
					if (up_it == _routing._cnx_audio_set.end ())
					{
						assert (false);
					}
					else
					{
						cnx.use_src () = up_it->use_src ();
						retry_flag     = true;
					}
				}
			}
			while (retry_flag);

			cnx_set.insert (cnx);
		}

		_routing._cnx_audio_set.swap (cnx_set);
	}
}



// These plug-ins have no audio output and are listed in the audio plug-ins.
bool	Preset::is_plugin_requiring_routing_fix (const std::string &pi_model)
{
	return (
		   pi_model == "envf"
		|| pi_model == "osdet"
		|| pi_model == "osdet2"
		|| pi_model == "pidet"
	);
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
