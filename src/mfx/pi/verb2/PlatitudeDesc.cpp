/*****************************************************************************

        PlatitudeDesc.cpp
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
#include "mfx/pi/param/MapSat.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/verb2/Cst.h"
#include "mfx/pi/verb2/PlatitudeDesc.h"
#include "mfx/pi/verb2/Param.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace verb2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PlatitudeDesc::PlatitudeDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "platitude";
	_info._name      = "Platitude Reverb\nPlatitudeVerb\nPlatitude\nPlat\nPV";
	_info._tag_list  = { piapi::Tag::_reverb_0, piapi::Tag::_spatial_0 };
	_info._chn_pref  = piapi::ChnPref::STEREO;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapSat <
		std::ratio <1, 1>,
		std::ratio <1, 2>
	> > TplDecayCoef;

	// Dry level
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 1,
		"Dry level\nDry lvl\nDry",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	double         nat_max = pll_sptr->get_nat_max ();
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.2, nat_max * 0.01, false);
	pll_sptr->use_mapper ().add_segment (1.0, nat_max       , true );
	_desc_set.add_glob (Param_LVL_DRY, pll_sptr);

	// Early reflection level
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Early reflection level\nEarly ref level\nEarly ref lvl\nEarly lvl\nER lvl\nERl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	nat_max = pll_sptr->get_nat_max ();
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.2, nat_max * 0.01, false);
	pll_sptr->use_mapper ().add_segment (1.0, nat_max       , true );
	_desc_set.add_glob (Param_LVL_EARLY, pll_sptr);

	// Diffuse field level
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Diffuse field level\nDiffuse fld level\nDiffuse fld lvl\nDiffuse lvl\nDiffuse\nDif",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	nat_max = pll_sptr->get_nat_max ();
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.2, nat_max * 0.01, false);
	pll_sptr->use_mapper ().add_segment (1.0, nat_max       , true );
	_desc_set.add_glob (Param_LVL_DFS, pll_sptr);

	// Predelay time
	pll_sptr = std::make_shared <TplPll> (
		0, Cst::_max_predelay_time,
		"Predelay time\nPredelay T\nPredelay\nPredly\nPdl",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_PREDELAY, pll_sptr);

	// Number of taps
	auto           int_sptr = std::make_shared <param::TplInt> (
		0, 50,
		"Number of taps\nNbr taps\nER taps\nETp",
		"",
		0,
		"%.0f"
	);
	_desc_set.add_glob (Param_EARLY_NBR, int_sptr);

	// Early reflection duration
	pll_sptr = std::make_shared <TplPll> (
		0.001, Cst::_max_er_duration,
		"Early reflection duration\nEarly ref duration\nEarly ref dur\nER dur\nEDu",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_EARLY_DUR, pll_sptr);

	// Early reflection decay
	pll_sptr = std::make_shared <TplPll> (
		0.01, 1.0,
		"Early reflection decay\nEarly ref decay\nER decay\nER dcy\nEDc",
		"ms",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_EARLY_DCY, pll_sptr);

	// Early reflection stereo depth
	auto           sim_sptr = std::make_shared <param::Simple> (
		"Early reflection stereo depth\nEarly ref stereo\nER stereo\nER st\nESt"
	);
	_desc_set.add_glob (Param_EARLY_STR, sim_sptr);

	// Input low-cut frequency
	auto           log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Input low-cut frequency\nInput LC freq\nIn LC freq\nInLCFrq\nILF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_DFS_INP_LC, log_sptr);

	// Input high-cut frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Input high-cut frequency\nInput HC freq\nIn HC freq\nInHCFrq\nIHF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_DFS_INP_HC, log_sptr);

	// Input diffusion
	sim_sptr = std::make_shared <param::Simple> (
		"Input diffusion\nInput diff\nIn diff\nIDf"
	);
	_desc_set.add_glob (Param_DFS_INP_DIF, sim_sptr);

	// Decay coefficient
	auto           dcy_sptr = std::make_shared <TplDecayCoef> (
		0, 1,
		"Decay coefficient\nDecay coef\nDecay\nDcy",
		"",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_DFS_DECAY, dcy_sptr);

	// Room size
	pll_sptr = std::make_shared <TplPll> (
		4.0/1024, 4,
		"Room size\nSize\nSz",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
#if 1
	pll_sptr->use_mapper ().add_segment (0.75, 1.0, true);
#else
	pll_sptr->use_mapper ().add_segment (0.5 , 1.0, true);
	pll_sptr->use_mapper ().add_segment (0.75, 2.0, false);
#endif
	pll_sptr->use_mapper ().add_segment (1.0 , pll_sptr->get_nat_max (), false);
	_desc_set.add_glob (Param_DFS_SIZE, pll_sptr);

	// Pitch shift
	pll_sptr = std::make_shared <TplPll> (
		-1200, +1200,
		"Pitch shift\nPitch\nPtS",
		"cents",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%+5.0f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.15, -250, false);
	pll_sptr->use_mapper ().add_segment (0.20, -100, false);
	pll_sptr->use_mapper ().add_segment (0.25,  -50, false);
	pll_sptr->use_mapper ().add_segment (0.75,  +50, false);
	pll_sptr->use_mapper ().add_segment (0.80, +100, false);
	pll_sptr->use_mapper ().add_segment (0.85, +250, false);
	pll_sptr->use_mapper ().add_segment (1.0 , pll_sptr->get_nat_max (), false);
	_desc_set.add_glob (Param_DFS_PITCH, pll_sptr);

	// Pitch shift mode
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Single\nMulti",
		"Pitch shift mode\nPitch mode\nPitchM\nPSM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DFS_PMODE, enu_sptr);

	// Tank low-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		2.5, 40960,
		"Tank low-cut frequency\nTank LC freq\nTnk LC freq\nTkLCFrq\nTLF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.10,    40, true);
	pll_sptr->use_mapper ().add_segment (0.90, 10240, true);
	pll_sptr->use_mapper ().add_segment (1.0 , pll_sptr->get_nat_max (), true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_DFS_TNK_LC, pll_sptr);

	// Tank high-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		2.5, 40960,
		"Tank high-cut frequency\nTank HC freq\nTnk HC freq\nTkHCFrq\nTHF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.10,    40, true);
	pll_sptr->use_mapper ().add_segment (0.90, 10240, true);
	pll_sptr->use_mapper ().add_segment (1.0 , pll_sptr->get_nat_max (), true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_DFS_TNK_HC, pll_sptr);

	// Tank diffusion
	sim_sptr = std::make_shared <param::Simple> (
		"Tank diffusion\nTank diff\nTk diff\nTDf"
	);
	_desc_set.add_glob (Param_DFS_TNK_DIF, sim_sptr);

	// Freeze
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Freeze\nFrz",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DFS_FREEZE, enu_sptr);

	// Flush
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Flush\nFlu",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DFS_FLUSH, enu_sptr);
}



ParamDescSet &	PlatitudeDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	PlatitudeDesc::do_get_info () const
{
	return _info;
}



void	PlatitudeDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	PlatitudeDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	PlatitudeDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace verb2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
