/*****************************************************************************

        SpectralFreezeDesc.cpp
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

#include "fstb/def.h"
#include "mfx/pi/sfreeze/Cst.h"
#include "mfx/pi/sfreeze/DMode.h"
#include "mfx/pi/sfreeze/Param.h"
#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace sfreeze
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpectralFreezeDesc::SpectralFreezeDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "specfreeze";
	_info._name      = "Spectral freeze\nSpec freeze\nSpeFrz";
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

	// Phasing speed
	pll_sptr = std::make_shared <TplPll> (
		0, 20,
		"Phasing speed\nPhasing spd\nPhasing\nPhase\nPha",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.1,  0.01, false);
	pll_sptr->use_mapper ().add_segment (0.5,  1   , true);
	pll_sptr->use_mapper ().add_segment (0.9, 10   , true);
	pll_sptr->use_mapper ().add_segment (1.0, 20   , false);
	_desc_set.add_glob (Param_PHASE, pll_sptr);

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
}



ParamDescSet &	SpectralFreezeDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	SpectralFreezeDesc::do_get_info () const
{
	return _info;
}



void	SpectralFreezeDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	SpectralFreezeDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SpectralFreezeDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SpectralFreezeDesc::configure_slot (int slot_idx)
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



}  // namespace sfreeze
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
