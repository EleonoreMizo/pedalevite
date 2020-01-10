/*****************************************************************************

        Cnx.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_doc_Cnx_CODEHEADER_INCLUDED)
#define mfx_doc_Cnx_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Cnx::operator == (const Cnx &other) const
{
	return (_src == other._src && _dst == other._dst);
}



bool	Cnx::operator != (const Cnx &other) const
{
	return ! (*this == other);
}



bool	Cnx::is_valid () const
{
	return (_src.is_valid () && _dst.is_valid ());
}



void	Cnx::set_src (const CnxEnd &ce)
{
	assert (ce.is_valid ());

	_src = ce;
}



void	Cnx::set_dst (const CnxEnd &ce)
{
	assert (ce.is_valid ());

	_dst = ce;
}



CnxEnd &	Cnx::use_src ()
{
	return _src;
}



CnxEnd &	Cnx::use_dst ()
{
	return _dst;
}



const CnxEnd &	Cnx::use_src () const
{
	return _src;
}



const CnxEnd &	Cnx::use_dst () const
{
	return _dst;
}



template <piapi::Dir D>
CnxEnd &	Cnx::use_end ()
{
	static_assert (D < 0 && D >= piapi::Dir_NBR_ELT, "Wrong D value.");
	return use_src ();
}

template <>
inline CnxEnd &	Cnx::use_end <piapi::Dir_IN> ()
{
	return use_src ();
}

template <>
inline CnxEnd &	Cnx::use_end <piapi::Dir_OUT> ()
{
	return use_dst ();
}



template <piapi::Dir D>
const CnxEnd &	Cnx::use_end () const
{
	static_assert (D < 0 && D >= piapi::Dir_NBR_ELT, "Wrong D value.");
	return use_src ();
}

template <>
inline const CnxEnd &	Cnx::use_end <piapi::Dir_IN> () const
{
	return use_src ();
}

template <>
inline const CnxEnd &	Cnx::use_end <piapi::Dir_OUT> () const
{
	return use_dst ();
}



CnxEnd &	Cnx::use_end (piapi::Dir dir)
{
	return (dir == piapi::Dir_IN) ? use_src () : use_dst ();
}



const CnxEnd &	Cnx::use_end (piapi::Dir dir) const
{
	return (dir == piapi::Dir_IN) ? use_src () : use_dst ();
}



bool	Cnx::has_slot_id (int slot_id) const
{
	return (_src.is_slot_id (slot_id) || _dst.is_slot_id (slot_id));
}



bool	operator < (const Cnx &lhs, const Cnx &rhs)
{
	assert (lhs.is_valid ());
	assert (rhs.is_valid ());

	if (lhs.use_src () < rhs.use_src ())
	{
		return true;
	}
	else if (lhs.use_src () == rhs.use_src ())
	{
		return (lhs.use_dst () < rhs.use_dst ());
	}

	return false;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



#endif   // mfx_doc_Cnx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
