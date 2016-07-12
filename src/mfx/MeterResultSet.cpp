/*****************************************************************************

        MeterResultSet.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/MeterResultSet.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	MeterResultSet::Side::check_signal_clipping ()
{
	return _clip_flag.exchange (false);
}



MeterResultSet::MeterResultSet ()
:	_audio_io ()
,	_dsp_use ()
,	_dsp_overload_flag ()
{
	reset ();
}



void	MeterResultSet::reset ()
{
	for (auto &s : _audio_io)
	{
		for (auto &c : s._chn_arr)
		{
			c._peak = 0;
			c._rms  = 0;
		}
		s._clip_flag.store (false);
	}
	_dsp_use._peak = 0;
	_dsp_use._rms  = 0;
	_dsp_overload_flag.store (false);
}



bool	MeterResultSet::check_signal_clipping ()
{
	bool           clip_flag = false;
	for (auto &s : _audio_io)
	{
		if (s.check_signal_clipping ())
		{
			clip_flag = true;
		}
	}

	return clip_flag;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
