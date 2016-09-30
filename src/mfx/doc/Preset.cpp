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
		auto           it = _slot_map.rbegin ();
		++ it;
		slot_id = it->first;
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



// Returns a list of slot_id beginning with the chain,
// followed by the other slots.
std::vector <int>	Preset::build_ordered_node_list () const
{
	// Set of the nodes we already inserted into the vector
	std::set <int> rem_slot_id;
	for (const auto &node : _slot_map)
	{
		rem_slot_id.insert (node.first);
	}

	// Inserts the main chain
	std::vector <int> slot_id_list = _routing._chain;

	// Removes nodes of the main chain
	for (int rem_id : slot_id_list)
	{
		const auto     it = rem_slot_id.find (rem_id);
		assert (it != rem_slot_id.end ());
		rem_slot_id.erase (it);
	}

	// Adds all the remaining slots
	for (int rem_id : rem_slot_id)
	{
		slot_id_list.push_back (rem_id);
	}

	return slot_id_list;
}



void	Preset::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_name);
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

	ser.end_list ();
}



void	Preset::ser_read (SerRInterface &ser)
{
	const int      doc_ver = ser.get_doc_version ();
	static const int  inc  = 11;

	ser.begin_list ();

	ser.read (_name);
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
