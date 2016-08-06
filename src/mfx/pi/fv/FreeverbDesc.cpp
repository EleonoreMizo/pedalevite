/*****************************************************************************

        FreeverbDesc.cpp
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

#include "mfx/pi/fv/FreeverbDesc.h"
#include "mfx/pi/fv/Param.h"
#include "mfx/pi/param/MapPseudoLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/ParamMapFdbk.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace fv
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FreeverbDesc::FreeverbDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPseudoLog> TplPsl;
	typedef param::TplMapped <ParamMapFdbk> TplFdbk;

	// Room size
	TplFdbk *      fdbk_ptr = new TplFdbk (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Room Size\nSize\nS",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_ROOMSIZE, fdbk_ptr);

	// Damping
	fdbk_ptr = new TplFdbk (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Damping\nDamp\nD",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_DAMP, fdbk_ptr);

	// Wet level
	TplPsl *       pl_ptr = new TplPsl (
		0, 4,
		"Level Wet\nLvl Wet\nWet\nWL",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_WET, pl_ptr);

	// Dry level
	pl_ptr = new TplPsl (
		0, 1,
		"Level Dry\nLvl Dry\nDry\nDL",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_DRY, pl_ptr);

	// Width
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"Width\nW",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_WIDTH, lin_ptr);

	// Mode
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Normal\nFreeze",
		"Mode\nM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_MODE, enu_ptr);

	// Low Cut
	param::TplLog *   log_ptr = new param::TplLog (
		20, 2000,
		"Low Cut\nLow C\nLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LOCUT, log_ptr);

	// High Cut
	log_ptr = new param::TplLog (
		2000, 20000,
		"High Cut\nHigh C\nHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HICUT, log_ptr);
}



ParamDescSet &	FreeverbDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	FreeverbDesc::do_get_unique_id () const
{
	return "freeverb";
}



std::string	FreeverbDesc::do_get_name () const
{
	return "Freeverb";
}



void	FreeverbDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	FreeverbDesc::do_prefer_stereo () const
{
	return true;
}



int	FreeverbDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	FreeverbDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fv
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
