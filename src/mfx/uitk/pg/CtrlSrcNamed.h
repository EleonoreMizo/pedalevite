/*****************************************************************************

        CtrlSrcNamed.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_CtrlSrcNamed_HEADER_INCLUDED)
#define mfx_uitk_pg_CtrlSrcNamed_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ControlSource.h"

#include <string>



namespace mfx
{
namespace uitk
{
namespace pg
{



class CtrlSrcNamed
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						CtrlSrcNamed (ControllerType type, int index, const char *name_0);
	               CtrlSrcNamed ()                          = default;
	               CtrlSrcNamed (const CtrlSrcNamed &other) = default;
	virtual        ~CtrlSrcNamed ()                         = default;

	CtrlSrcNamed & operator = (const CtrlSrcNamed &other)   = default;

	ControlSource  _src;
	std::string    _name;   // '\n'-separated multilabel string



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CtrlSrcNamed &other) const = delete;
	bool           operator != (const CtrlSrcNamed &other) const = delete;

}; // class CtrlSrcNamed



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/CtrlSrcNamed.hpp"



#endif   // mfx_uitk_pg_CtrlSrcNamed_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
