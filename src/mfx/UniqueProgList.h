/*****************************************************************************

        UniqueProgList.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_UniqueProgList_HEADER_INCLUDED)
#define mfx_UniqueProgList_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{

namespace doc
{
	class Preset;
}


class View;

class UniqueProgList
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class ProgCoord
	{
	public:
		int            _bank;
		int            _prog;
	};
	typedef std::vector <ProgCoord> ProgList;

	               UniqueProgList ()  = default;
	virtual        ~UniqueProgList () = default;

	ProgList       build (const View &view);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           is_prog_eq (const doc::Preset &lhs, const doc::Preset &rhs) const;
	bool           is_slot_eq (const doc::Slot &lhs, const doc::Slot &rhs) const;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UniqueProgList (const UniqueProgList &other)    = delete;
	UniqueProgList &
	               operator = (const UniqueProgList &other)        = delete;
	bool           operator == (const UniqueProgList &other) const = delete;
	bool           operator != (const UniqueProgList &other) const = delete;

}; // class UniqueProgList



}  // namespace mfx



//#include "mfx/UniqueProgList.hpp"



#endif   // mfx_UniqueProgList_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
