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

#include <cassert>
#include <climits>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



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



bool	Preset::check_new_routing (const Routing &routing) const
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
	assert (check_new_routing (routing));

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

	assert (check_new_routing (_routing));
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



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
