/*****************************************************************************

        CompexDesc.cpp
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

#include "mfx/pi/cpx/CompexDesc.h"
#include "mfx/pi/cpx/Param.h"
#include "mfx/pi/param/MapRatio.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace cpx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CompexDesc::CompexDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapRatio> TplRat;

	// Ratio, Low
	// Actually it's the inverse of the displayed ratio (0 = flat)
	TplRat *          rat_ptr = new TplRat (
		1.0 / (1 << 4), 1 << 2,
		"RL\nRLo\nR.Low\nRatio Low\nRatio below threshold",
		"",
		param::HelperDispNum::Preset_RATIO_INV,
		0,
		"%.1f"
	);
	_desc_set.add_glob (Param_RATIO_L, rat_ptr);

	// Ratio, High
	// Actually it's the inverse of the displayed ratio (0 = flat)
	rat_ptr = new TplRat (
		1.0 / (1 << 4), 1 << 2,
		"RH\nRHi\nR.High\nRatio High\nRatio above threshold",
		"",
		param::HelperDispNum::Preset_RATIO_INV,
		0,
		"%.1f"
	);
	_desc_set.add_glob (Param_RATIO_H, rat_ptr);

	// Knee Level
	param::TplLin *   lin_ptr = new param::TplLin (
		-14, +2,	// -84 dB ... +12 dB
		"Th\nThr\nThres\nThreshold",
		"dB",
		0,
		"%+5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB_FROM_LOG2);
	_desc_set.add_glob (Param_KNEE_LVL, lin_ptr);

	// Knee Shape
	lin_ptr = new param::TplLin (
		0, 2,
		"Th\nThr\nThres\nThreshold",
		"dB",
		0,
		"%4.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB_FROM_LOG2);
	_desc_set.add_glob (Param_KNEE_SHAPE, lin_ptr);

	// Attack Time
	param::TplLog *   log_ptr = new param::TplLog (
		0.0001, 0.1,
		"A\nAtk\nAttack\nAttack Time",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_ATTACK, log_ptr);

	// Release Time
	log_ptr = new param::TplLog (
		0.001, 1.0,
		"R\nRls\nRelease\nRelease Time",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%4.1f"
	);
	_desc_set.add_glob (Param_RELEASE, log_ptr);

	// Makeup Gain
	lin_ptr = new param::TplLin (
		-8, +8, // -48 dB ... +48 dB
		"G\nGain\nMakeup gain",
		"dB",
		0,
		"%+5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB_FROM_LOG2);
	_desc_set.add_glob (Param_GAIN, lin_ptr);
}



ParamDescSet &	CompexDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	CompexDesc::do_get_unique_id () const
{
	return "compex";
}



std::string	CompexDesc::do_get_name () const
{
	return "CompEx";
}



void	CompexDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;  // Sidechain not implemented at the moment
	nbr_o = 1;
}



bool	CompexDesc::do_prefer_stereo () const
{
	return false;
}



int	CompexDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	CompexDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
