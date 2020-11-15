/*****************************************************************************

        DryWetDesc.cpp
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

#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/dwm/Param.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/Cst.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dwm
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DryWetDesc::DryWetDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = Cst::_plugin_dwm;
	_info._name      = "Dry/Wet mixer\nDry/Wet";
	_info._tag_list  = { piapi::Tag::_utility_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Bypass
	auto           enum_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Bypass",
		""
	);
	_desc_set.add_glob (Param_BYPASS, enum_sptr);

	// Dry/Wet
	auto           maps_sptr =
		std::make_shared <param::TplMapped <param::MapS <false> > > (
			0, 1,
			"Effect mix\nFX mix",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_WET, maps_sptr);

	// Volume
	auto           log_sptr = std::make_shared <param::TplLog> (
		1e-3, 10,
		"Volume\nVol",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_sptr);
}



ParamDescSet &	DryWetDesc::use_desc_set ()
{
	return _desc_set;
}



const float	DryWetDesc::_gain_neutral = 0.75f;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	DryWetDesc::do_get_info () const
{
	return _info;
}



void	DryWetDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 2;
	nbr_o = 1;
	nbr_s = 0;
}



int	DryWetDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DryWetDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dwm
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
