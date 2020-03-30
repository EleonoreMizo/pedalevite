/*****************************************************************************

        MoogLpfDesc.cpp
        Author: Laurent de Soras, 2020

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

#include "mfx/pi/moog1/FltMode.h"
#include "mfx/pi/moog1/FltVariant.h"
#include "mfx/pi/moog1/MoogLpfDesc.h"
#include "mfx/pi/moog1/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace moog1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MoogLpfDesc::MoogLpfDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "moog1";
	_info._name      = "Moog filter\nMoog flt\nMoogF";
	_info._tag_list  = { piapi::Tag::_eq_filter_0 };

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Mode
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"LP4\nLP2\nBP4\nBP2\nHP4\nHP2\nN4\nN2\nN2X\nPK",
		"Mode\nMo",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == FltMode_NBR_ELT - 1);
	_desc_set.add_glob (Param_MODE, enu_sptr);

	enu_sptr = std::make_shared <param::TplEnum> (
		"Standard\nQuantize\nFlip",
		"Variant\nVar",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == FltVariant_NBR_ELT - 1);
	_desc_set.add_glob (Param_VARIANT, enu_sptr);

	// Cutoff frequency
	auto           log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Cutoff frequency\nCutoff freq\nCutoff\nFreq\nF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_CUTOFF, log_sptr);

	// Resonance
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 1.5,
		"Resonance\nReso\nR",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (     0);
	pll_sptr->use_mapper ().add_segment (0.45,    0.90, false);
	pll_sptr->use_mapper ().add_segment (0.75,    1.05, false);
	pll_sptr->use_mapper ().add_segment (1   ,    1.50, false);
	_desc_set.add_glob (Param_RESO, pll_sptr);

	// DC gain compensation
	auto           sim_sptr = std::make_shared <param::Simple> (
		"DC gain compensation\nGain compensation\nGain comp\nGComp\nGC"
	);
	_desc_set.add_glob (Param_GCOMP, sim_sptr);

	// Side chain FM
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Side chain FM\nSideC FM\nScFM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_SIDEC_ENA, enu_sptr);

	// Side chain FM amplitude
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Side chain FM amplitude\nSide chain FM amp\nSideC FM amp\nScFM amp\nScA",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (     0);
	pll_sptr->use_mapper ().add_segment (0.5,     1, false);
	pll_sptr->use_mapper ().add_segment (1   ,   10, true);
	_desc_set.add_glob (Param_SIDEC_AMP, pll_sptr);

	// Self-FM
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Self-FM\nSelFM\nSlFM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_SELF_ENA, enu_sptr);

	// Self-FM amplitude
	pll_sptr = std::make_shared <TplPll> (
		0, 100,
		"Self-FM amplitude\nSelf-FM amp\nSelFM amp\nSlFM amp\nSlA",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (     0);
	pll_sptr->use_mapper ().add_segment (0.50,    1, false);
	pll_sptr->use_mapper ().add_segment (1   ,  100, true);
	_desc_set.add_glob (Param_SELF_AMP, pll_sptr);

	// Self-FM low-pass filter frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Self-FM low-pass filter frequency\nSelf-FM LPF frequency"
		"\nSelf-FM LPF freq\nSelFM LP freq\nSelFM freq\nSlFm freq\nSlF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_SELF_LPF, log_sptr);
}



ParamDescSet &	MoogLpfDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	MoogLpfDesc::do_get_info () const
{
	return _info;
}



void	MoogLpfDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 2;
	nbr_o = 1;
	nbr_s = 0;
}



bool	MoogLpfDesc::do_prefer_stereo () const
{
	return false;
}



int	MoogLpfDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	MoogLpfDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
