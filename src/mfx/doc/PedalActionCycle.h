/*****************************************************************************

        PedalActionCycle.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PedalActionCycle_HEADER_INCLUDED)
#define mfx_doc_PedalActionCycle_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PedalActionSingleInterface.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class PedalActionCycle
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <PedalActionSingleInterface> ActionSPtr;
	typedef std::vector <ActionSPtr> ActionArray;
	typedef std::vector <ActionArray> ActionCycle;

	               PedalActionCycle ();
	               PedalActionCycle (const PedalActionCycle &other);
	virtual        ~PedalActionCycle () = default;

	PedalActionCycle &
	               operator = (const PedalActionCycle &other);

	bool           is_empty_default () const;
	void           merge_cycle (const PedalActionCycle &other);

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	ActionCycle    _cycle;            // Null pointers and empty members not allowed (but _cycle can be empty). Only "PRESS" cycles are allowed to have more than 1 action array.
	bool           _inherit_flag;     // cycle will merge to the parent cycle, possibly extending it.
	bool           _overridable_flag; // Non-empty cycles will be replaced by a non-empty child



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static ActionCycle
	               duplicate_actions (const ActionCycle &cycle);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PedalActionCycle &other) const = delete;
	bool           operator != (const PedalActionCycle &other) const = delete;

}; // class PedalActionCycle



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PedalActionCycle.hpp"



#endif   // mfx_doc_PedalActionCycle_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
