/*****************************************************************************

        DistoPwmDesc.cpp
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

#include "mfx/pi/distpwm/DistoPwmDesc.h"
#include "mfx/pi/distpwm/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace distpwm
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoPwmDesc::DistoPwmDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "distpwm";
	_info._name      = "Pulse Width Modulation distortion\nPWM distortion\nPWM disto";
	_info._tag_list  = { piapi::Tag::_distortion_0, piapi::Tag::_synth_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Pulse Width
	auto           pll_sptr = std::make_shared <TplPll> (
		2.5e-5, 2.5e-3,
		"Pulse Width\nPulse W\nPW",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%6.2f"
	);
	pll_sptr->use_mapper ().set_first_value (2.5e-5);
	pll_sptr->use_mapper ().add_segment (0.5,  2.5e-4, false);
	pll_sptr->use_mapper ().add_segment (1.0,  2.5e-3, true);
	pll_sptr->set_flags (piapi::ParamDescInterface::Flags_AUTOLINK);
	_desc_set.add_glob (Param_PW, pll_sptr);
}



ParamDescSet &	DistoPwmDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	DistoPwmDesc::do_get_info () const
{
	return _info;
}



void	DistoPwmDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	DistoPwmDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistoPwmDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace distpwm
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
