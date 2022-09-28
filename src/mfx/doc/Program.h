/*****************************************************************************

        Program.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_Program_HEADER_INCLUDED)
#define mfx_doc_Program_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PedalboardLayout.h"
#include "mfx/doc/ProgSwitchMode.h"
#include "mfx/doc/Routing.h"
#include "mfx/doc/SignalPort.h"
#include "mfx/doc/Slot.h"

#include <map>
#include <memory>
#include <vector>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class Program
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <Slot> SlotSPtr;
	typedef std::map <int, SlotSPtr> SlotMap; // [slot_id] = SlotSPtr
	typedef std::map <int, SignalPort> PortMap; // [port_index] = SignalPort

	               Program ();
	               Program (const Program &other);
	               Program (Program &&other)    = default;
	               ~Program ()                  = default;

	Program &      operator = (const Program &other);
	Program &      operator = (Program &&other) = default;

	bool           is_slot_empty (int slot_id) const;
	bool           is_slot_empty (SlotMap::const_iterator it) const;
	bool           is_slot_empty (SlotMap::value_type vt) const;
	Slot &         use_slot (int slot_id);
	const Slot &   use_slot (int slot_id) const;
	int            gen_slot_id () const;
	int            find_free_port () const;
	bool           check_routing (const Routing &routing) const;
	void           set_routing (const Routing &routing);
	const Routing& use_routing () const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	SlotMap        _slot_map;
	std::string    _name;
	PedalboardLayout
	               _layout;
	PortMap        _port_map;
	ProgSwitchMode _prog_switch_mode = ProgSwitchMode::DIRECT;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           duplicate_slot_list ();
	bool           check_routing_cnx_audio_end (const CnxEnd &cnx_end) const;
	void           fix_routing_converted_from_chain ();

	static bool    is_plugin_requiring_routing_fix (const std::string &pi_model);

	Routing        _routing;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Program &other) const = delete;
	bool           operator != (const Program &other) const = delete;

}; // class Program



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/Program.hpp"



#endif   // mfx_doc_Program_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
