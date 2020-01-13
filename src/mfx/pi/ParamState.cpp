/*****************************************************************************

        ParamState.cpp
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

#include "mfx/pi/ParamState.h"

#include <cassert>



namespace mfx
{
namespace pi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ParamState::ParamState ()
:	_val_beg_nrm (0)
,	_val_end_nrm (0)
,	_smooth ()
,	_notifier ()
,	_desc_ptr (nullptr)
{
	_smooth.set_inertia_time (0);
	_smooth.force_val (0);
}



void	ParamState::set_sample_freq (double fs)
{
	_smooth.set_sample_freq (fs);
}



void	ParamState::set_ramp_time (double t)
{
	_smooth.set_inertia_time (t);
}



void	ParamState::set_desc (const piapi::ParamDescInterface &desc)
{
	_desc_ptr = &desc;
}



const piapi::ParamDescInterface &	ParamState::use_desc () const
{
	assert (_desc_ptr != nullptr);

	return *_desc_ptr;
}



fstb::util::ObservableInterface &	ParamState::use_notifier ()
{
	return _notifier;
}



fstb::util::NotificationFlagInterface &	ParamState::use_notif_flag ()
{
	return _notifier;
}



void	ParamState::clear_buffers ()
{
	_smooth.clear_buffers ();
	_val_beg_nrm = _smooth.get_target_val ();
	_val_end_nrm = _smooth.get_target_val ();

	_notifier.notify_observers ();
}



void	ParamState::set_val (double val_nrm)
{
	assert (val_nrm >= 0);
	assert (val_nrm <= 1);

	_smooth.set_val (val_nrm);
	_notifier.notify_observers ();
}



void	ParamState::tick (int nbr_spl)
{
	assert (nbr_spl > 0);

	if (_smooth.is_ramping ())
	{
		_val_beg_nrm = _smooth.get_val ();
		_smooth.tick (nbr_spl);
		_val_end_nrm = _smooth.get_val ();
		_notifier.notify_observers ();
	}
	else
	{
		_val_end_nrm = _smooth.get_target_val ();
		if (_val_beg_nrm != _val_end_nrm)
		{
			_val_beg_nrm = _val_end_nrm;
			_notifier.notify_observers ();
		}
	}
}



bool	ParamState::is_ramping () const
{
	return (_val_end_nrm != _val_beg_nrm);
}



double	ParamState::get_val_beg () const
{
	return _val_beg_nrm;
}



double	ParamState::get_val_end () const
{
	return _val_end_nrm;
}



double	ParamState::get_val_tgt () const
{
	return _smooth.get_target_val ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
