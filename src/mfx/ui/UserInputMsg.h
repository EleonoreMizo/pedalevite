/*****************************************************************************

        UserInputMsg.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_UserInputMsg_HEADER_INCLUDED)
#define mfx_ui_UserInputMsg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/ui/UserInputType.h"

#include <cstdint>



namespace mfx
{
namespace ui
{



class UserInputMsg
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               UserInputMsg ()                          = default;
	               UserInputMsg (const UserInputMsg &other) = default;
	virtual        ~UserInputMsg ()                         = default;

	UserInputMsg & operator = (const UserInputMsg &other)   = default;

	void           set (int64_t date, UserInputType type, int index, float val);
	int64_t        get_date () const;
	UserInputType  get_type () const;
	int            get_index () const;
	float          get_val  () const;

	bool           is_valid () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int64_t        _date  = INT64_MIN;  // Microseconds, reference depends on the emitter
	UserInputType  _type  = UserInputType_UNDEFINED;
	int            _index = -1;
	float          _val   = -1;         // In range [0 ; 1]



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const UserInputMsg &other) const = delete;
	bool           operator != (const UserInputMsg &other) const = delete;

}; // class UserInputMsg



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputMsg.hpp"



#endif   // mfx_ui_UserInputMsg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
