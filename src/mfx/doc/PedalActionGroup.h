/*****************************************************************************

        PedalActionGroup.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PedalActionGroup_HEADER_INCLUDED)
#define mfx_doc_PedalActionGroup_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/ActionTrigger.h"
#include "mfx/doc/PedalActionCycle.h"

#include <array>



namespace mfx
{
namespace doc
{



class PedalActionGroup
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalActionGroup ()                              = default;
	               PedalActionGroup (const PedalActionGroup &other) = default;
	virtual        ~PedalActionGroup ()                             = default;

	PedalActionGroup &
	               operator = (const PedalActionGroup &other)       = default;

	std::array <PedalActionCycle, ActionTrigger_NBR_ELT>
	               _action_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PedalActionGroup &other) const = delete;
	bool           operator != (const PedalActionGroup &other) const = delete;

}; // class PedalActionGroup



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PedalActionGroup.hpp"



#endif   // mfx_doc_PedalActionGroup_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
