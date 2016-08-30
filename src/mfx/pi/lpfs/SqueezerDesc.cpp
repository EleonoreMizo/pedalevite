/*****************************************************************************

        SqueezerDesc.cpp
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

#include "mfx/pi/lpfs/Param.h"
#include "mfx/pi/lpfs/SqueezerDesc.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace lpfs
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SqueezerDesc::SqueezerDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Cutoff Frequency
	param::TplLog *   log_ptr = new param::TplLog (
		20, 20 * 1024,
		"F\nFreq\nFrequency\nCutoff Frequency",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	log_ptr->set_flags (piapi::ParamDescInterface::Flags_AUTOLINK);
	_desc_set.add_glob (Param_FREQ, log_ptr);

	// Resonance
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"R\nReso\nResonance",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_RESO, lin_ptr);

	// Color
	lin_ptr = new param::TplLin (
		0, 1,
		"C\nColor",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_COLOR, lin_ptr);

	// Drive
	log_ptr = new param::TplLog (
		1.0 / 8, 32,
		"D\nDrive",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_DRIVE, log_ptr);

	// Type
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Hard\nSoft\nNone",
		"F\nFold\nFoldback",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_TYPE, enu_ptr);
}



ParamDescSet &	SqueezerDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	SqueezerDesc::do_get_unique_id () const
{
	return "lpfs";
}



std::string	SqueezerDesc::do_get_name () const
{
	return "Filter Squeezer\nSqueezer";
}



void	SqueezerDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	SqueezerDesc::do_prefer_stereo () const
{
	return false;
}



int	SqueezerDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SqueezerDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lpfs
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
