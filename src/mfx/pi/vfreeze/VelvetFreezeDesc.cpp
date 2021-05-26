/*****************************************************************************

        VelvetFreezeDesc.cpp
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

#include "mfx/pi/vfreeze/Cst.h"
#include "mfx/pi/vfreeze/DMode.h"
#include "mfx/pi/vfreeze/VelvetFreezeDesc.h"
#include "mfx/pi/vfreeze/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace vfreeze
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VelvetFreezeDesc::VelvetFreezeDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "velvetfreeze";
	_info._name      = "Velvet freeze\nVelv freeze\nVelFrz";
	_info._tag_list  = { piapi::Tag::_spectral_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		configure_slot (slot_idx);
	}

	// Crossfade
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, Cst::_nbr_slots,
		"Crossfade\nXFade\nXFd",
		"",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_XFADE, lin_sptr);

	// Crossfade gain
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 2,
		"Crossfade gain\nXFade gain\nXfd gain\nXfG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (10, fstb::SQRT2);
	_desc_set.add_glob (Param_XFGAIN, pll_sptr);

	// Dry mode
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Mix\nCut\nMute",
		"Dry mode\nDMode\nDrM",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == DMode_NBR_ELT - 1);
	_desc_set.add_glob (Param_DMODE, enu_sptr);

	// Grain size
	pll_sptr = std::make_shared <TplPll> (
		Cst::_max_grain_size_ms / 16'000.0, Cst::_max_grain_size_ms / 1000.0,
		"Grain size\nGr size\nGSz",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%3.0f"
	);
	pll_sptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (Param_GSIZE, pll_sptr);

	// Density
	pll_sptr = std::make_shared <TplPll> (
		4, 64,
		"Density\nDens\nDns",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.1f"
	);
	pll_sptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (Param_DENSITY, pll_sptr);
}



ParamDescSet &	VelvetFreezeDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	VelvetFreezeDesc::do_get_info () const
{
	return _info;
}



void	VelvetFreezeDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	VelvetFreezeDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	VelvetFreezeDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	VelvetFreezeDesc::configure_slot (int slot_idx)
{
	assert (slot_idx >= 0);
	assert (slot_idx < Cst::_nbr_slots);

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	const int      base = Param_SLOT_BASE + slot_idx * ParamSlot_NBR_ELT;

	// Freeze
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Slot %d freeze\nS%d freeze\nS%d frz\nS%dF",
		"",
		slot_idx,
		"%s"
	);
	_desc_set.add_glob (base + ParamSlot_FREEZE, enu_sptr);

	// Gain
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 2,
		"Slot %d gain\nS%d gain\nS%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		slot_idx,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (10, fstb::SQRT2);
	_desc_set.add_glob (base + ParamSlot_GAIN, pll_sptr);
}



}  // namespace vfreeze
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
