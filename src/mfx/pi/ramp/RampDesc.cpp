/*****************************************************************************

        RampDesc.cpp
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

#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/ramp/CurveType.h"
#include "mfx/pi/ramp/RampDesc.h"
#include "mfx/pi/ramp/Param.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace ramp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



RampDesc::RampDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Time
	param::TplLog *   log_ptr = new param::TplLog (
		0.1, 1000,
		"Time\nT",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_TIME, log_ptr);

	// Amplitude
	param::TplLin *   lin_ptr = new param::TplLin (
		-4, 4,
		"A\nAmp\nAmplitude",
		"%",
		0,
		"%+6.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_AMP, lin_ptr);

	// Curve
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Linear\n"
		"Acc 1\nAcc 2\nAcc 3\nAcc 4\nSat 1\nSat 2\nSat 3\nSat 4\n"
		"Fast 1\nFast 2\nSlow 1\nSlow 2",
		"Curve\nC",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_CURVE, enu_ptr);
	assert (enu_ptr->get_nat_max () == CurveType_NBR_ELT - 1);

	// Sample and hold
	lin_ptr = new param::TplLin (
		0, 1,
		"SnH\nSplHold\nSample & hold\nSample and hold",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SNH, lin_ptr);

	// Smoothing
	lin_ptr = new param::TplLin (
		0, 1,
		"Sm\nSmooth\nSmoothing",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SMOOTH, lin_ptr);

	// Direction
	enu_ptr = new param::TplEnum (
		"Normal\nInvert",
		"Direction\nDir\nD",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DIR, enu_ptr);

	// Set position
	lin_ptr = new param::TplLin (
		0, 1,
		"Set position\nSet pos\nPos\nP",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	_desc_set.add_glob (Param_POS, lin_ptr);

	// Initial delay
	TplPll *       pll_ptr = new TplPll (
		0, 1000,
		"Initial delay\nDelay\nD",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	pll_ptr->use_mapper ().set_first_value (     0.0);
	pll_ptr->use_mapper ().add_segment (0.2 ,    1.0, false);
	pll_ptr->use_mapper ().add_segment (0.6 ,   10.0, true);
	pll_ptr->use_mapper ().add_segment (1.0 , 1000.0, true );
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_DELAY, pll_ptr);

	// State
	enu_ptr = new param::TplEnum (
		"Running\nPause",
		"State\nS",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_STATE, enu_ptr);
}



ParamDescSet &	RampDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	RampDesc::do_get_unique_id () const
{
	return "ramp";
}



std::string	RampDesc::do_get_name () const
{
	return "Control ramp\nCtrl ramp\nRamp";
}



void	RampDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 0;
	nbr_o = 0;
	nbr_s = 1;
}



bool	RampDesc::do_prefer_stereo () const
{
	return false;
}



int	RampDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	RampDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ramp
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
