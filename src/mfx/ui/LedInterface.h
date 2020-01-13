/*****************************************************************************

        LedInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_ui_LedInterface_HEADER_INCLUDED)
#define mfx_ui_LedInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace ui
{



class LedInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               LedInterface ()                          = default;
	               LedInterface (const LedInterface &other) = default;
	               LedInterface (LedInterface &&other)      = default;

	virtual        ~LedInterface ()                         = default;

	virtual LedInterface &
	               operator = (const LedInterface &other)   = default;
	virtual LedInterface &
	               operator = (LedInterface &&other)        = default;

	int            get_nbr_led () const;
	void           set_led (int index, float val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_nbr_led () const = 0;
	virtual void   do_set_led (int index, float val) = 0;



}; // class LedInterface



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/LedInterface.hpp"



#endif   // mfx_ui_LedInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
