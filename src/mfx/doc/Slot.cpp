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

#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/doc/Slot.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Slot::Slot ()
:	_pi_model ()
,	_settings_all ()
,	_settings_mixer ()
,	_label ()
{
	_settings_mixer._force_mono_flag = false;
	_settings_mixer._param_list.resize (pi::dwm::Param_NBR_ELT);
	_settings_mixer._param_list [pi::dwm::Param_BYPASS] = 0;
	_settings_mixer._param_list [pi::dwm::Param_WET   ] = 1;
	_settings_mixer._param_list [pi::dwm::Param_GAIN  ] =
		pi::dwm::DryWetDesc::_gain_neutral;
}



bool	Slot::is_empty () const
{
	return (_pi_model.empty ());
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



void	Slot::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	ser.write (_pi_model);
	ser.write (_label);
	_settings_mixer.ser_write (ser);

	ser.begin_list ();
	for (const auto &sp : _settings_all)
	{
		if (sp.first == _pi_model)
		{
			ser.begin_list ();

			ser.write (sp.first);
			sp.second.ser_write (ser);

			ser.end_list ();
		}
	}
	ser.end_list ();

	ser.end_list ();
}



void	Slot::ser_read (SerRInterface &ser)
{
	ser.begin_list ();

	ser.read (_pi_model);
	ser.read (_label);
	_settings_mixer.ser_read (ser);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_settings_all.clear ();
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		ser.begin_list ();

		std::string    model_id;
		ser.read (model_id);
		_settings_all [model_id].ser_read (ser);

		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
