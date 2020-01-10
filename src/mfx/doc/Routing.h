/*****************************************************************************

        Routing.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_Routing_HEADER_INCLUDED)
#define mfx_doc_Routing_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Cnx.h"

#include <set>
#include <vector>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class Routing
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::set <Cnx> CnxSet;

	               Routing ()                        = default;
	               Routing (const Routing &other)    = default;
	virtual        ~Routing ()                       = default;
	Routing &      operator = (const Routing &other) = default;

	bool           has_slots () const;
	bool           is_referencing_slot (int slot_id) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	static void    build_from_audio_chain (CnxSet &cnx_set, const std::vector <int> &chain);

	CnxSet         _cnx_audio_set;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Routing &other) const = delete;
	bool           operator != (const Routing &other) const = delete;

}; // class Routing



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/Routing.hpp"



#endif   // mfx_doc_Routing_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
