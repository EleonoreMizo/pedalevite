/*****************************************************************************

        Cnx.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_Cnx_HEADER_INCLUDED)
#define mfx_doc_Cnx_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CnxEnd.h"
#include "mfx/piapi/Dir.h"

#include <array>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class Cnx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Cnx (const CnxEnd &src, const CnxEnd &dst);
	               Cnx ()                        = default;
	               Cnx (const Cnx &other)        = default;
	               Cnx (Cnx &&other)             = default;

	               ~Cnx ()                       = default;

	Cnx &          operator = (const Cnx &other) = default;
	Cnx &          operator = (Cnx &&other)      = default;

	inline bool    operator == (const Cnx &other) const;
	inline bool    operator != (const Cnx &other) const;

	inline bool    is_valid () const;

	inline void    set_src (const CnxEnd &ce);
	inline void    set_dst (const CnxEnd &ce);

	inline CnxEnd& use_src ();
	inline CnxEnd& use_dst ();
	inline const CnxEnd &
	               use_src () const;
	inline const CnxEnd &
	               use_dst () const;
	template <piapi::Dir D>
	inline CnxEnd& use_end ();
	template <piapi::Dir D>
	inline const CnxEnd &
	               use_end () const;
	inline CnxEnd& use_end (piapi::Dir dir);
	inline const CnxEnd &
	               use_end (piapi::Dir dir) const;

	inline bool    has_slot_id (int slot_id) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	CnxEnd         _src;
	CnxEnd         _dst;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class Cnx



inline bool    operator < (const Cnx &lhs, const Cnx &rhs);



}  // namespace doc
}  // namespace mfx



#include "mfx/doc/Cnx.hpp"



#endif   // mfx_doc_Cnx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
