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

#include <map>
#include <set>
#include <vector>

#include "mfx/doc/Slot.h"



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

	               UniqueProgList ()                            = default;
	               UniqueProgList (const UniqueProgList &other) = default;
	               UniqueProgList (UniqueProgList &&other)      = default;

	               ~UniqueProgList ()                           = default;

	UniqueProgList &
	               operator = (const UniqueProgList &other)     = default;
	UniqueProgList &
	               operator = (UniqueProgList &&other)          = default;

	ProgList       build (const View &view);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::set <int> SlotIdSet;
	typedef std::array <SlotIdSet, 2> SlotIdSetPair;
	typedef std::map <doc::Slot, SlotIdSet> SlotMap;
	typedef std::set <SlotIdSetPair> SlotSetPairSet;

	bool           is_prog_eq (const doc::Preset &lhs, const doc::Preset &rhs) const;
	void           build_slot_map (SlotMap &slot_map, const doc::Preset &prog) const;
	bool           merge_slot_maps (SlotSetPairSet &slot_set_pair_set, const SlotMap &slot_map_1, const SlotMap &slot_map_2) const;
	bool           check_connections (const SlotSetPairSet &slot_set_pair_set, const doc::Preset &lhs, const doc::Preset &rhs) const;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const UniqueProgList &other) const = delete;
	bool           operator != (const UniqueProgList &other) const = delete;

}; // class UniqueProgList



}  // namespace mfx



//#include "mfx/UniqueProgList.hpp"



#endif   // mfx_UniqueProgList_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
