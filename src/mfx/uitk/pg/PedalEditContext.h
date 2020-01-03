/*****************************************************************************

        PedalEditContext.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_PedalEditContext_HEADER_INCLUDED)
#define mfx_uitk_pg_PedalEditContext_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PedalActionGroup.h"
#include "mfx/uitk/pg/PageType.h"
#include "mfx/PedalLoc.h"



namespace mfx
{

class View;

namespace doc
{
	class PedalboardLayout;
}

namespace uitk
{
namespace pg
{



class PedalEditContext
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_INVALID = -1,

		Type_GLOBAL  = 0,
		Type_BANK,
		Type_PRESET,

		Type_NBR_ELT
	};


	               PedalEditContext ()                              = default;
	               PedalEditContext (const PedalEditContext &other) = default;
	virtual        ~PedalEditContext ()                             = default;
	PedalEditContext &
	               operator = (const PedalEditContext &other)       = default;

	const doc::PedalboardLayout &
	               use_layout (const View &view) const;
	PedalLoc       conv_to_loc (const View &view) const;

	Type           _type         = Type_INVALID;
	int            _pedal        = -1;
	doc::ActionTrigger
	               _trigger      = doc::ActionTrigger_INVALID;
	int            _step_index   = -1;
	int            _action_index = -1;
	doc::PedalActionGroup
	               _content;
	PageType       _ret_page     = PageType_PROG_CUR;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PedalEditContext &other) const = delete;
	bool           operator != (const PedalEditContext &other) const = delete;

}; // class PedalEditContext



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/PedalEditContext.hpp"



#endif   // mfx_uitk_pg_PedalEditContext_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
