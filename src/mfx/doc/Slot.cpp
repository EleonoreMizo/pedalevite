/*****************************************************************************

        Slot.cpp
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

#include "mfx/doc/Slot.h"
#include "mfx/pi/dwm/DryWet.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Slot::Slot ()
:	_pi_model (pi::PluginModel_INVALID)
,	_settings_all ()
,	_settings_mixer ()
,	_label ()
{
	_settings_mixer._force_mono_flag = false;
	_settings_mixer._param_list.resize (pi::dwm::DryWet::Param_NBR_ELT);
	_settings_mixer._param_list [pi::dwm::DryWet::Param_BYPASS] = 0;
	_settings_mixer._param_list [pi::dwm::DryWet::Param_WET   ] = 1;
	_settings_mixer._param_list [pi::dwm::DryWet::Param_GAIN  ] =
		pi::dwm::DryWet::_gain_neutral;
}



bool	Slot::is_empty () const
{
	return (_pi_model == pi::PluginModel_INVALID);
}



PluginSettings &	Slot::use_settings (PiType type)
{
	if (type == PiType_MIX)
	{
		return _settings_mixer;
	}

	auto           it = _settings_all.find (_pi_model);
	assert (it != _settings_all.end ());

	return it->second;
}



const PluginSettings &	Slot::use_settings (PiType type) const
{
	if (type == PiType_MIX)
	{
		return _settings_mixer;
	}

	auto           it = _settings_all.find (_pi_model);
	assert (it != _settings_all.end ());

	return it->second;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
