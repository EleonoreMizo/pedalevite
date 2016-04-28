/*****************************************************************************

        TplLog.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_TplLog_HEADER_INCLUDED)
#define mfx_pi_param_TplLog_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/param/MapLog.h"
#include "mfx/pi/param/TplMapped.h"



namespace mfx
{
namespace pi
{
namespace param
{



class TplLog
:	public TplMapped <MapLog>
{

	typedef TplMapped <MapLog> Inherited;

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       TplLog (double val_min, double val_max, const char *name_0, const char *unit_0, int group_index = 0, const char *format_0 = "%5.2f");
	virtual        ~TplLog () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TplLog ()                               = delete;
	               TplLog (const TplLog &other)            = delete;
	TplLog &       operator = (const TplLog &other)        = delete;
	bool           operator == (const TplLog &other) const = delete;
	bool           operator != (const TplLog &other) const = delete;

}; // class TplLog



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/TplLog.hpp"



#endif   // mfx_pi_param_TplLog_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
