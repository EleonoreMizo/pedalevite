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
{
	// Bypass
	param::TplEnum *  enum_ptr = new param::TplEnum (
		"Off\nOn",
		"Bypass",
		""
	);
	_desc_set.add_glob (Param_BYPASS, enum_ptr);

	// Dry/Wet
	param::TplMapped <param::MapS <false> > * maps_ptr =
		new param::TplMapped <param::MapS <false> > (
			0, 1,
			"Effect Mix\nFX Mix",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_WET, maps_ptr);

	// Volume
	param::TplLog *   log_ptr = new param::TplLog (
		1e-3, 10,
		"Volume\nVol",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_ptr);
}



ParamDescSet &	DryWetDesc::use_desc_set ()
{
	return _desc_set;
}



const float	DryWetDesc::_gain_neutral = 0.75f;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DryWetDesc::do_get_unique_id () const
{
	return Cst::_plugin_mix;
}



std::string	DryWetDesc::do_get_name () const
{
	return "Dry/Wet mixer\nDry/Wet";
}



void	DryWetDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 2;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DryWetDesc::do_prefer_stereo () const
{
	return false;
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
