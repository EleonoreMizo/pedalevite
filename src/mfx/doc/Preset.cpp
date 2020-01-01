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

#include <cassert>
#include <climits>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Preset::Preset (const Preset &other)
:	_slot_map (other._slot_map)
,	_routing (other._routing)
,	_name (other._name)
,	_layout (other._layout)
,	_port_map (other._port_map)
,	_prog_switch_mode (other._prog_switch_mode)
{
	duplicate_slot_list ();
}



Preset &	Preset::operator = (const Preset &other)
{
	if (&other != this)
	{
		_slot_map = other._slot_map;
		_routing  = other._routing;
		_name     = other._name;
		_layout   = other._layout;
		_port_map = other._port_map;
		_prog_switch_mode = other._prog_switch_mode;
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

	return (   it->second.get () == 0
	        || it->second->is_empty ());
}



Slot &	Preset::use_slot (int slot_id)
{
	const auto     it = _slot_map.find (slot_id);
	if (it == _slot_map.end ())
	{
		assert (false);
		throw std::logic_error ("Slot does not exist.");
	}
	if (it->second.get () == 0)
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
	if (it->second.get () == 0)
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



// Returns a list of slot_id beginning with the chain,
// followed by the other slots.
std::vector <int>	Preset::build_ordered_node_list (bool chain_first_flag) const
{
	// Set of the nodes we already inserted into the vector
	std::set <int> rem_slot_id;
	for (const auto &node : _slot_map)
	{
		rem_slot_id.insert (node.first);
	}

	// Removes nodes of the main chain
	for (int rem_id : _routing._chain)
	{
		const auto     it = rem_slot_id.find (rem_id);
		assert (it != rem_slot_id.end ());
		rem_slot_id.erase (it);
	}

	std::vector <int> slot_id_list;

	// Inserts the main chain (first position case)
	if (chain_first_flag)
	{
		slot_id_list = _routing._chain;
	}

	// Adds all the remaining slots
	for (int rem_id : rem_slot_id)
	{
		slot_id_list.push_back (rem_id);
	}

	// Inserts the main chain (last position case)
	if (! chain_first_flag)
	{
		slot_id_list.insert (
			slot_id_list.end (),
			_routing._chain.begin (),
			_routing._chain.end ()
		);
	}

	return slot_id_list;
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
		int         slot_id;

		if (doc_ver < 3)
		{
			slot_id = cnt * inc;
		}
		else
		{
			ser.begin_list ();

			ser.read (slot_id);
		}

		SlotSPtr    s_sptr = SlotSPtr (new Slot);
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
		for (int cnt = 0; cnt < nbr_elt; ++ cnt)
		{
			_routing._chain.push_back (cnt * inc);
		}
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
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Preset::duplicate_slot_list ()
{
	for (auto & node : _slot_map)
	{
		if (node.second.get () != 0)
		{
			node.second = SlotSPtr (new Slot (*(node.second)));
		}
	}
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
