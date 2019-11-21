/*****************************************************************************

        ProcInfo.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_ProcInfo_HEADER_INCLUDED)
#define mfx_piapi_ProcInfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/BypassState.h"
#include "mfx/piapi/Dir.h"



namespace mfx
{
namespace piapi
{



class EventTs;

class ProcInfo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ProcInfo ()                        = default;
	               ProcInfo (const ProcInfo &other)   = default;
	virtual        ~ProcInfo ()                       = default;

	ProcInfo &     operator = (const ProcInfo &other) = default;

	float * const *
	               _dst_arr   = 0;
	float * const *
	               _byp_arr   = 0;
	const float * const *
	               _src_arr   = 0;
	float * const *
	               _sig_arr   = 0;
	int            _nbr_chn_arr [Dir_NBR_ELT];
	BypassState    _byp_state = BypassState_IGNORE; // On input and output
	int            _nbr_spl   = 0;
	const EventTs * const *
	               _evt_arr   = 0;
	int            _nbr_evt   = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ProcInfo &other) const = delete;
	bool           operator != (const ProcInfo &other) const = delete;

}; // class ProcInfo



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/ProcInfo.hpp"



#endif   // mfx_piapi_ProcInfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
