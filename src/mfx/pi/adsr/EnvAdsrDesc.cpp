/*****************************************************************************

        EnvAdsrDesc.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/pi/adsr/EnvAdsrDesc.h"
#include "mfx/pi/adsr/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace adsr
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EnvAdsrDesc::EnvAdsrDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Trigger On
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"Trigger on\nTrig on\nOn",
		"%",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_TRIG_ON, lin_ptr);

	// Trigger Off
	lin_ptr = new param::TplLin (
		0, 1,
		"Trigger off\nTrig off\nOff",
		"%",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_TRIG_OFF, lin_ptr);

	// Velocity sensitivity
	lin_ptr = new param::TplLin (
		0, 1,
		"Velocity sensitivity\nVelo sensitivity\nVelo sens\nVel sns\nVelS\nVS",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_VELO_SENS, lin_ptr);

	// Attack time
	TplPll *       pll_ptr = new TplPll (
		1.0 / 1024, 64,
		"Attack time\nAtk time\nAtk T\nAtkT\nAT",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ::Categ_TIME_S);
	pll_ptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_ATK_T, pll_ptr);

	// Decay time
	pll_ptr = new TplPll (
		1.0 / 1024, 64,
		"Decay time\nDcy time\nDcy T\nDcyT\nDT",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ::Categ_TIME_S);
	pll_ptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_DCY_T, pll_ptr);

	// Sustain time
	pll_ptr = new TplPll (
		1.0 / 256, 256,
		"Sustain time\nSus time\nSus T\nSusT\nST",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ::Categ_TIME_S);
	pll_ptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_SUS_T, pll_ptr);

	// Sustain level
	lin_ptr = new param::TplLin (
		-2, 2,
		"Sustain level\nSus level\nSus lvl\nSus L\nSusL\nSL",
		"%",
		0,
		"%+6.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SUS_L, lin_ptr);

	// Release time
	pll_ptr = new TplPll (
		1.0 / 256, 256,
		"Release time\nRls time\nRls T\nRlsT\nRT",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ::Categ_TIME_S);
	pll_ptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_RLS_T, pll_ptr);
}



ParamDescSet &	EnvAdsrDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	EnvAdsrDesc::do_get_unique_id () const
{
	return "adsr";
}



std::string	EnvAdsrDesc::do_get_name () const
{
	return "ADSR";
}



void	EnvAdsrDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 0;
	nbr_o = 0;
	nbr_s = 1;
}



bool	EnvAdsrDesc::do_prefer_stereo () const
{
	return false;
}



int	EnvAdsrDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	EnvAdsrDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace adsr
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
