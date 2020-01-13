/*****************************************************************************

        CnxEnd.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_CnxEnd_HEADER_INCLUDED)
#define mfx_doc_CnxEnd_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class CnxEnd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_INVALID = -1,

		Type_NORMAL = 0,
		Type_IO,

		Type_NBR_ELT
	};

	explicit       CnxEnd (Type type, int slot_id, int pin);
	               CnxEnd ()                        = default;
	               CnxEnd (const CnxEnd &other)     = default;
	               CnxEnd (CnxEnd &&other)          = default;

	               ~CnxEnd ()                       = default;

	CnxEnd &       operator = (const CnxEnd &other) = default;
	CnxEnd &       operator = (CnxEnd &&other)      = default;

	inline bool    operator == (const CnxEnd &other) const;
	inline bool    operator != (const CnxEnd &other) const;

	void           set (Type type, int slot_id, int pin);
	inline bool    is_valid () const;
	inline Type    get_type () const;
	inline int     get_slot_id () const;
	inline int     get_pin () const;
	inline bool    is_slot_id (int slot_id) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Type           _type    = Type_INVALID;
	int            _slot_id = -1;
	int            _pin     = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class CnxEnd



inline bool    operator < (const CnxEnd &lhs, const CnxEnd &rhs);



}  // namespace doc
}  // namespace mfx



#include "mfx/doc/CnxEnd.hpp"



#endif   // mfx_doc_CnxEnd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
