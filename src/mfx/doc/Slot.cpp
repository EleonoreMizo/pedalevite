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



bool	Slot::operator == (const Slot &other) const
{
	return (
		   _pi_model       == other._pi_model
	   && _settings_all   == other._settings_all
		&& _settings_mixer == other._settings_mixer
		&& _label          == other._label
	);
}



bool	Slot::operator != (const Slot &other) const
{
	return ! (*this == other);
}



bool	Slot::is_similar (const Slot &other) const
{
	if (_pi_model != other._pi_model)
	{
		return false;
	}

	for (int type_cnt = 0; type_cnt < PiType_NBR_ELT; ++type_cnt)
	{
		const PiType   type = PiType (type_cnt);
		const doc::PluginSettings &   settings_l =       use_settings (type);
		const doc::PluginSettings &   settings_r = other.use_settings (type);
		if (! settings_l.is_similar (settings_r))
		{
			return false;
		}
	}

	return true;
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



PluginSettings *	Slot::test_and_get_settings (PiType type)
{
	if (type == PiType_MIX)
	{
		return &_settings_mixer;
	}

	auto           it = _settings_all.find (_pi_model);
	if (it != _settings_all.end ())
	{
		return &(it->second);
	}

	return 0;
}



const PluginSettings *	Slot::test_and_get_settings (PiType type) const
{
	if (type == PiType_MIX)
	{
		return &_settings_mixer;
	}

	auto           it = _settings_all.find (_pi_model);
	if (it != _settings_all.end ())
	{
		return &(it->second);
	}

	return 0;
}



bool	Slot::has_ctrl () const
{
	bool           ctrl_flag  = false;

	if (_settings_mixer.has_any_ctrl ())
	{
		ctrl_flag = true;
	}
	else
	{
		auto           it = _settings_all.find (_pi_model);
		if (it != _settings_all.end ())
		{
			ctrl_flag = it->second.has_any_ctrl ();
		}
	}

	return ctrl_flag;
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
	_settings_mixer.ser_read (ser, Cst::_plugin_dwm);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_settings_all.clear ();
	for (int cnt = 0; cnt < nbr_elt; ++cnt)
	{
		ser.begin_list ();

		std::string    model_id;
		ser.read (model_id);
		_settings_all [model_id].ser_read (ser, model_id);

		ser.end_list ();
	}
	ser.end_list ();

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator < (const Slot &lhs, const Slot &rhs)
{
	if (lhs._pi_model < rhs._pi_model ) { return true; }
	else if (lhs._pi_model == rhs._pi_model)
	{
		if (lhs._settings_all < rhs._settings_all ) { return true; }
		else if (lhs._settings_all == rhs._settings_all)
		{
			if (lhs._settings_mixer < rhs._settings_mixer ) { return true; }
			else if (lhs._settings_mixer == rhs._settings_mixer)
			{
				return (lhs._label < rhs._label);
			}
		}
	}

	return false;
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
