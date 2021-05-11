/*****************************************************************************

        VolumeCloneDesc.cpp
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

#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/vclone/ChnMode.h"
#include "mfx/pi/vclone/VolumeCloneDesc.h"
#include "mfx/pi/vclone/Param.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace vclone
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VolumeCloneDesc::VolumeCloneDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "volumeclone";
	_info._name      = "Volume clone\nVolClone\nVClone\nVCl";
	_info._tag_list  = { piapi::Tag::_utility_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Multiply-copy
	auto           sim_sptr = std::make_shared <param::Simple> (
		"Multiply-copy\nMult-copy\nMul-copy\nMulCopy\nMulC\nMC"
	);
	_desc_set.add_glob (Param_MODE, sim_sptr);

	// Strength
	sim_sptr = std::make_shared <param::Simple> (
		"Strength\nStren\nStr"
	);
	_desc_set.add_glob (Param_STRN, sim_sptr);

	// Channel mode
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Linked\nIndep\nLeft\nRight",
		"Channel mode\nChn mode\nChnM\nCM",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == ChnMode_NBR_ELT - 1);
	_desc_set.add_glob (Param_CHN, enu_sptr);

	// Minimum gain
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Minimum gain\nMin gain\nMiGain\nMiG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	double         nat_max = pll_sptr->get_nat_max ();
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.2, nat_max * 0.01, false);
	pll_sptr->use_mapper ().add_segment (1.0, nat_max       , true );
	_desc_set.add_glob (Param_GMIN, pll_sptr);

	// Maximum gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.01, 100,
		"Maximum gain\nMax gain\nMaGain\nMaG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GMAX, log_sptr);

	// Attack time
	pll_sptr = std::make_shared <TplPll> (
		0.1e-3, 1,
		"Attack Time\nAttack\nAtk\nA",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.2, 0.001, true);
	pll_sptr->use_mapper ().add_segment (0.8, 0.100, true);
	pll_sptr->use_mapper ().add_segment (1.0, pll_sptr->get_nat_max (), true);
	_desc_set.add_glob (Param_ATK, pll_sptr);

	// Release time
	pll_sptr = std::make_shared <TplPll> (
		1e-3, 10,
		"Release Time\nRelease\nRls\nR",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.8, 1.000, true);
	pll_sptr->use_mapper ().add_segment (1.0, pll_sptr->get_nat_max (), true);
	_desc_set.add_glob (Param_RLS, pll_sptr);

	// Hold time
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Hold Time\nHold\nH",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.1, 0.010, false);
	pll_sptr->use_mapper ().add_segment (0.9, 1.000, true);
	pll_sptr->use_mapper ().add_segment (1.0, pll_sptr->get_nat_max (), true);
	_desc_set.add_glob (Param_HOLD, pll_sptr);
}



ParamDescSet &	VolumeCloneDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	VolumeCloneDesc::do_get_info () const
{
	return _info;
}



void	VolumeCloneDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 2;
	nbr_o = 1;
	nbr_s = 0;
}



int	VolumeCloneDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	VolumeCloneDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace vclone
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
