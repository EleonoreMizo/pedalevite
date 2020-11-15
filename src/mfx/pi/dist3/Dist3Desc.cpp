/*****************************************************************************

        Dist3Desc.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/dist3/Dist3Desc.h"
#include "mfx/pi/dist3/Param.h"
#include "mfx/pi/dist3/Shaper.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplEnumEvol.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist3
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Dist3Desc::Dist3Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "dist3";
	_info._name      = "Distortion 3\nDistort 3\nDisto 3\nDist 3\nD3";
	_info._tag_list  = { piapi::Tag::_distortion_0, piapi::Tag::_amp_simulator_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Input high-pass filter cutoff frequency
	auto           log_sptr = std::make_shared <param::TplLog> (
		3, 3000,
		"Input high-pass filter cutoff frequency\nInput HPF frequency"
		"\nHPF cutoff frequency\nHPF frequency\nHPF freq\nHPF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HPF_PRE, log_sptr);

	// Distortion gain
	auto           pll_sptr = std::make_shared <TplPll> (
		1.0 / 64, 1024.0,
		"Distortion gain\nDist gain\nGain",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.2, 1.0                     , true);
	pll_sptr->use_mapper ().add_segment (1.0, pll_sptr->get_nat_max (), true);
	_desc_set.add_glob (Param_GAIN, pll_sptr);

	// Slew rate limiting, input
	pll_sptr = std::make_shared <TplPll> (
		250, 64000,
		"Slew rate limiting, input\nSlew rate limit in"
		"\nSlew rate lim in\nSlew rate in\nSRLI",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_SRL_PRE, pll_sptr);

	// Class
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"A\nB",
		"Class\nCla",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_CLASS, enu_sptr);

	// Bias, static
	auto           sim_sptr = std::make_shared <param::Simple> (
		"Bias, static\nBias stat\nBSta"
	);
	sim_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_BIAS_S, sim_sptr);

	// Bias, dynamic
	sim_sptr = std::make_shared <param::Simple> (
		"Bias, dynamic\nBias dyn\nBDyn"
	);
	sim_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_BIAS_D, sim_sptr);

	// Bias low-pass filter cutoff frequency
	log_sptr = std::make_shared <param::TplLog> (
		1, 1024,
		"Bias low-pass filter cutoff frequency\nBias LPF cutoff frequency"
		"\nBias LPF frequency\nBias LPF freq\nBias LPF\nBLpf",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_BIAS_LPF, log_sptr);

	// Bias source
	enu_sptr = std::make_shared <param::TplEnum> (
		"Direct\nFeedback",
		"Bias source\nBias src\nBSrc",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_BIAS_SRC, enu_sptr);

	// Distortion type
	auto           enue_sptr = std::make_shared <param::TplEnumEvol> (
		2,
		"Arcsinh\nAsym 1\nProg 1\nProg 2\nProg 3\nSudden\nHardclip\nPuncher 1"
		"\nPuncher 2\nPuncher 3\nOvershoot\nBitcrush\nLopsided\nPorridge\nSmartE 1\nSmartE 2"
		"\nAttract\nRandWalk\nSq Root\nBelt\nBad mood\nBounce\nLight 1\nLight 2"
		"\nLight 3\nTanh\nTanh+lin\nBreak\nAsym 2",
		"Distortion type\nDist type\nType",
		"",
		0,
		"%s"
	);
	assert (enue_sptr->get_nat_max () == Shaper::Type_NBR_ELT - 1);
	_desc_set.add_glob (Param_TYPE, enue_sptr);

	// Slew rate limiting, output
	pll_sptr = std::make_shared <TplPll> (
		250, 16000,
		"Slew rate limiting, output\nSlew rate limit, out"
		"\nSlew rate lim, out\nSlew rate out\nSRLO",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (6);
	_desc_set.add_glob (Param_SRL_POST, pll_sptr);

	// PSU overload
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"PSU overload\nPSU ovrld\nPSU ovr\nPsuO",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_PSU_OVRL, enu_sptr);

	// PSU load amount
	pll_sptr = std::make_shared <TplPll> (
		0.1, 10,
		"PSU load amount\nPSU load\nPsuL",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%4.0f"
	);
	pll_sptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (Param_PSU_LOAD, pll_sptr);

	// PSU frequency
	log_sptr = std::make_shared <param::TplLog> (
		55.0 / 2, 55.0 * 2,
		"PSU frequency\nPSU freq\nPsuF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_PSU_FREQ, log_sptr);

	// Low-pass filter cutoff frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Low-pass filter cutoff frequency\nLPF cutoff frequency"
		"\nLPF frequency\nLPF freq\nLPF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LPF_POST, log_sptr);
}



ParamDescSet &	Dist3Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	Dist3Desc::do_get_info () const
{
	return _info;
}



void	Dist3Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	Dist3Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Dist3Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
