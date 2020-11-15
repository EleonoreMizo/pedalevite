/*****************************************************************************

        Phaser2Desc.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/phase2/LfoType.h"
#include "mfx/pi/phase2/Param.h"
#include "mfx/pi/phase2/Phaser2Desc.h"
#include "mfx/pi/ParamMapFdbkBipolar.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace phase2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Phaser2Desc::Phaser2Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "phaser2";
	_info._name      = "Phaser AP";
	_info._tag_list  = { piapi::Tag::_modulation_0 };
	_info._chn_pref  = piapi::ChnPref::STEREO;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapS <false> > TplMaps;
	typedef param::TplMapped <ParamMapFdbkBipolar> TplFdbk;

	// Speed
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.01, 100,
		"Sp\nSpd\nSpeed",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_SPEED, log_sptr);

	// Mix
	auto           maps_sptr = std::make_shared <TplMaps> (
		0, 1,
		"Phase Mix\nPh Mix\nMix\nM",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_MIX, maps_sptr);

	// Feedback
	auto           fbi_sptr = std::make_shared <TplFdbk> (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Feedback level\nFdbk level\nFeedback\nFdbk\nF",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%+6.1f"
	);
	_desc_set.add_glob (Param_FEEDBACK, fbi_sptr);

	// Number of stages
	auto           int_sptr = std::make_shared <param::TplInt> (
		1, 20,
		"Number of stages\nStages\nStg",
		"",
		0,
		"%.0f"
	);
	int_sptr->use_disp_num ().set_scale (2);
	_desc_set.add_glob (Param_NBR_STAGES, int_sptr);

	// Minimum frequency
	auto           pll_sptr = std::make_shared <TplPll> (
		20, 20480,
		"Minimum frequency\nMin freq\nMinF\nMF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	pll_sptr->use_mapper ().gen_log (10);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ_MIN, pll_sptr);

	// Maximum frequency
	pll_sptr = std::make_shared <TplPll> (
		20, 20480,
		"Maximum frequency\nMax freq\nMaxF\nMF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	pll_sptr->use_mapper ().gen_log (10);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ_MAX, pll_sptr);

	// Feedback source
	int_sptr = std::make_shared <param::TplInt> (
		1, 40,
		"Feedback source\nFdbk source\nFdbk src\nFSrc\nFS",
		"",
		0,
		"%.0f"
	);
	_desc_set.add_glob (Param_FDBK_POS, int_sptr);

	// Waveform
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Sine\nTriangle\nSquare\nSaw\nParabola\nBiphase\nN-Phase\nVarislope"
		"\nNoise",
		"W\nWavef\nWaveform",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == LfoType_NBR_ELT - 1);
	_desc_set.add_glob (Param_WAVEFORM, enu_sptr);

	// Sample and hold
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"SnH\nSplHold\nSample & hold\nSample and hold",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_SNH, lin_sptr);

	// Smoothing
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Sm\nSmooth\nSmoothing",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_SMOOTH, lin_sptr);

	// Chaos amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"C\nChaos\nChaos amt\nChaos amount",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_CHAOS, lin_sptr);

	// Phase distortion amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PDA\nPhDistA\nPhDistAmt\nPhase dist amt\nPhase dist amount"
		"\nPhase distortion amount",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_PH_DIST_AMT, lin_sptr);

	// Phase distortion offset
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PDO\nPhDistO\nPhDistOfs\nPhase dist ofs\nPhase dist offset"
		"\nPhase distortion offset",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_PH_DIST_OFS, lin_sptr);

	// Sign
	enu_sptr = std::make_shared <param::TplEnum> (
		"Normal\nInvert",
		"Si\nSign",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_SIGN, enu_sptr);

	// Polarity
	enu_sptr = std::make_shared <param::TplEnum> (
		"Bipolar\nUnipolar",
		"Po\nPolar\nPolarity",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_POLARITY, enu_sptr);

	// Variation 1
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"V1\nVar 1\nVariation 1",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_VAR1, lin_sptr);

	// Variation 2
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"V2\nVar 2\nVariation 2",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_VAR2, lin_sptr);

	// Phase set
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PS\nPh set\nPhase set",
		"\xC2\xB0", // U+00B0 DEGREE SIGN
		0,
		"%3.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_sptr);
}



ParamDescSet &	Phaser2Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	Phaser2Desc::do_get_info () const
{
	return _info;
}



void	Phaser2Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	Phaser2Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Phaser2Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace phase2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
