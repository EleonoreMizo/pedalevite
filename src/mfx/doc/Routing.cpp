/*****************************************************************************

        Routing.cpp
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

#include "mfx/doc/Routing.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <vector>

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Routing::has_slots () const
{
	for (const auto &cnx : _cnx_audio_set)
	{
		if (   cnx.use_src ().get_type () != CnxEnd::Type_IO
		    || cnx.use_dst ().get_type () != CnxEnd::Type_IO)
		{
			return true;
		}
	}

	return false;
}



bool	Routing::is_referencing_slot (int slot_id) const
{
	for (const auto &cnx : _cnx_audio_set)
	{
		const CnxEnd & cnx_src = cnx.use_src ();
		const CnxEnd & cnx_dst = cnx.use_dst ();

		if (   (   cnx_src.get_type ()    == CnxEnd::Type_NORMAL
		        && cnx_src.get_slot_id () == slot_id)
		    || (   cnx_dst.get_type ()    == CnxEnd::Type_NORMAL
		        && cnx_dst.get_slot_id () == slot_id))
		{
			return true;
		}
	}

	return false;
}



void	Routing::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.begin_list ();

	for (const auto &cnx : _cnx_audio_set)
	{
		cnx.ser_write (ser);
	}

	ser.end_list ();

	ser.end_list ();
}



void	Routing::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	int            nbr_elt = 0;
	ser.begin_list (nbr_elt);

	_cnx_audio_set.clear ();

	// The routing is a chain we have to turn into a graph
	if (ser.get_doc_version () < 10)
	{
		std::vector <int> chain (nbr_elt);
		for (int cnt = 0; cnt < nbr_elt; ++cnt)
		{
			ser.read (chain [cnt]);
		}
		build_from_audio_chain (_cnx_audio_set, chain);
		assert (! _cnx_audio_set.empty ());
	}

	// Real graph
	else
	{
		Cnx            cnx;
		for (int cnt = 0; cnt < nbr_elt; ++cnt)
		{
			cnx.ser_read (ser);
			_cnx_audio_set.insert (cnx);
		}
	}

	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Routing::build_from_audio_chain (CnxSet &cnx_set, const std::vector <int> &chain)
{
	CnxEnd         cnx_beg (doc::CnxEnd::Type_IO, 0, 0);

	for (const int slot_id : chain)
	{
		CnxEnd         cnx_end (doc::CnxEnd::Type_NORMAL, slot_id, 0);
		cnx_set.insert (Cnx (cnx_beg, cnx_end));

		cnx_beg = cnx_end;
	}

	CnxEnd         cnx_end (doc::CnxEnd::Type_IO, 0, 0);
	cnx_set.insert (Cnx (cnx_beg, cnx_end));
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
