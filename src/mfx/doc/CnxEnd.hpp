/*****************************************************************************

        CnxEnd.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_doc_CnxEnd_CODEHEADER_INCLUDED)
#define mfx_doc_CnxEnd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <tuple>

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	CnxEnd::operator == (const CnxEnd &other) const
{
	return (
		   _type    == other._type
		&& _slot_id == other._slot_id
		&& _pin     == other._pin
	);
}



bool	CnxEnd::operator != (const CnxEnd &other) const
{
	return ! (*this == other);
}



bool	CnxEnd::is_valid () const
{
	return (_type >= 0);
}



CnxEnd::Type	CnxEnd::get_type () const
{
	assert (is_valid ());

	return _type;
}



int	CnxEnd::get_slot_id () const
{
	assert (is_valid ());

	return _slot_id;
}



int	CnxEnd::get_pin () const
{
	assert (is_valid ());

	return _pin;
}



bool	CnxEnd::is_slot_id (int slot_id) const
{
	assert (is_valid ());

	return (_type == Type_NORMAL && _slot_id == slot_id);
}



bool  operator < (const CnxEnd &lhs, const CnxEnd &rhs)
{
	assert (lhs.is_valid ());
	assert (rhs.is_valid ());

	const CnxEnd::Type   l_type = lhs.get_type ();
	const CnxEnd::Type   r_type = rhs.get_type ();
	const int            l_slid = lhs.get_slot_id ();
	const int            r_slid = rhs.get_slot_id ();
	const int            l_pin  = lhs.get_pin ();
	const int            r_pin  = rhs.get_pin ();

	return (
		  std::tie (l_type, l_slid, l_pin)
		< std::tie (r_type, r_slid, r_pin)
	);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



#endif   // mfx_doc_CnxEnd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
