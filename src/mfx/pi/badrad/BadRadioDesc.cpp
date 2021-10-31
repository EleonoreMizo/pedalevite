/*****************************************************************************

        BadRadioDesc.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/badrad/BadRadioDesc.h"
#include "mfx/pi/badrad/Param.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMid.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace badrad
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BadRadioDesc::BadRadioDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "badradio";
	_info._name      = "Bad radio reception\nBad radio\nBadRad";
	_info._tag_list  = { piapi::Tag::_distortion_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		1, 100,
		"Gain\nGn",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_sptr);

	// Noisiness
	auto           mid_sptr = std::make_shared <param::TplMid> (
		0, 5, 1,
		"Noisiness\nNoise\nNz",
		"%",
		0,
		"%5.1f"
	);
	mid_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_NOISE, mid_sptr);

	// Hiss
	mid_sptr = std::make_shared <param::TplMid> (
		0, 1, 0.05,
		"Hiss\nHs",
		"%",
		0,
		"%+5.1f"
	);
	mid_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB);
	_desc_set.add_glob (Param_HISS, mid_sptr);

	// Noise bandwidth
	log_sptr = std::make_shared <param::TplLog> (
		640, 20480,
		"Gain\nGn",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_NZBW, log_sptr);

	// Low-cut frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 640,
		"Low-cut frequency\nLow-cut freq\nLow-cut\nLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LC, log_sptr);

	// High-cut frequency
	log_sptr = std::make_shared <param::TplLog> (
		640, 20480,
		"High-cut frequency\nHigh-cut freq\nHigh-cut\nHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HC, log_sptr);
}



ParamDescSet &	BadRadioDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	BadRadioDesc::do_get_info () const
{
	return _info;
}



void	BadRadioDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	BadRadioDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	BadRadioDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace badrad
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
