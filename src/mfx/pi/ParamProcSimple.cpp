/*****************************************************************************

        ParamProcSimple.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ParamProcSimple::ParamProcSimple (ParamStateSet &state_set)
:	_state_set (state_set)
,	_steady_req_flag (false)
,	_full_reset_flag (false)
,	_nxt_steady_req_flag (false)
,	_nxt_full_reset_flag (false)
{
	// Nothing
}



void	ParamProcSimple::handle_msg (const piapi::ProcInfo &proc)
{
	_steady_req_flag = _nxt_steady_req_flag;
	_full_reset_flag = _nxt_full_reset_flag;

	_nxt_steady_req_flag = false;
	_nxt_full_reset_flag = false;

	const int      nbr_evt = proc._nbr_evt;
	for (int index = 0; index < nbr_evt; ++index)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [index]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
		else if (evt._type == piapi::EventType_RESET)
		{
			_steady_req_flag |= evt._evt._reset._param_ramp_flag;
			_full_reset_flag |= evt._evt._reset._full_state_flag;
		}
	}

	if (_steady_req_flag)
	{
		_state_set.set_steady_state ();
	}
}



bool	ParamProcSimple::is_req_steady_state () const
{
	return _steady_req_flag;
}



bool	ParamProcSimple::is_full_reset () const
{
	return _full_reset_flag;
}



void	ParamProcSimple::req_steady ()
{
	_nxt_steady_req_flag = true;
}



void	ParamProcSimple::req_all ()
{
	_nxt_steady_req_flag = true;
	_nxt_full_reset_flag = true;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
