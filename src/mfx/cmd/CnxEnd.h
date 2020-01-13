/*****************************************************************************

        CnxEnd.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_CnxEnd_HEADER_INCLUDED)
#define mfx_cmd_CnxEnd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace cmd
{



class CnxEnd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum SlotType
	{
		SlotType_INVALID = -1,

		SlotType_NORMAL = 0,
		SlotType_IO,   // Input or output depends on the context
		SlotType_DLY,

		SlotType_NBR_ELT
	};

	inline bool    operator < (const CnxEnd &other) const;
	inline bool    operator == (const CnxEnd &other) const;
	inline bool    operator != (const CnxEnd &other) const;

	inline bool    is_valid () const;
	inline void    invalidate ();

	SlotType       _slot_type = SlotType_INVALID;

	// Slot position within Document::_slot_list, >= 0. -1: not set
	int            _slot_pos  = -1;

	// Pin index, >= 0. -1: not set
	int            _pin       = -1;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



}; // class CnxEnd



}  // namespace cmd
}  // namespace mfx



#include "mfx/cmd/CnxEnd.hpp"



#endif   // mfx_cmd_CnxEnd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
