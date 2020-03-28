/*****************************************************************************

        HarmTremDesc.cpp
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
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/tremh/HarmTremDesc.h"
#include "mfx/pi/tremh/LfoType.h"
#include "mfx/pi/tremh/Param.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace tremh
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HarmTremDesc::HarmTremDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "harmotremo";
	_info._name      = "Harmonic Tremolo\nHarmo Tremolo\nHarm Trem\nHTrem";

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Speed
	auto           pll_sptr = std::make_shared <TplPll> (
		0.5, 50,
		"Speed\nSpd",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_sptr->use_mapper ().set_first_value (   0.5);
	pll_sptr->use_mapper ().add_segment (0.25,  4  , true);
	pll_sptr->use_mapper ().add_segment (0.75 , 10 , false);
	pll_sptr->use_mapper ().add_segment (1    , 50 , true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_HZ);
	_desc_set.add_glob (Param_SPEED, pll_sptr);

	// Amount
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Amount\nAmt",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (   0);
	pll_sptr->use_mapper ().add_segment (0.25,  0.25, false);
	pll_sptr->use_mapper ().add_segment (0.75,  1.5 , true);
	pll_sptr->use_mapper ().add_segment (1   , 10   , true);
	_desc_set.add_glob (Param_AMT, pll_sptr);

	// Bass
	auto           lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Bass",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_LO, lin_sptr);

	// Treble
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Treb\nTreble",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_HI, lin_sptr);

	// Cutoff frequency
	auto           log_sptr = std::make_shared <param::TplLog> (
		750, 3000,
		"Cut\nCutoff\nCutoff freq\nCutoff frequency",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ, log_sptr);

	// Gain Saturation
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Gain Staturation\nGain Sat\nGSat",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_GSAT, lin_sptr);

	// Bias
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Bias",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_BIAS, lin_sptr);

	// Tone
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"To\nTone",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_TONE, lin_sptr);

	// Stereo
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"St\nStereo",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_STEREO, lin_sptr);

	// Waveform
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Sine\nTriangle\nSquare\nSaw\nParabola\nBiphase\nN-Phase\nVarislope\nNoise",
		"W\nWavef\nWaveform",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == LfoType_NBR_ELT - 1);
	_desc_set.add_glob (Param_WAVEFORM, enu_sptr);

	// Sample and Hold
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"SnH\nSplHold\nSample & Hold\nSample And Hold",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SNH, lin_sptr);

	// Smoothing
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Sm\nSmooth\nSmoothing",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SMOOTH, lin_sptr);

	// Chaos amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"C\nChaos\nChaos amt\nChaos amount",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_CHAOS, lin_sptr);

	// Phase distortion amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PDA\nPhDistA\nPhDistAmt\nPhase dist amt\nPhase dist amount\nPhase distortion amount",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_PH_DIST_AMT, lin_sptr);

	// Phase distortion offset
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PDO\nPhDistO\nPhDistOfs\nPhase dist ofs\nPhase dist offset\nPhase distortion offset",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
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
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_VAR1, lin_sptr);

	// Variation 2
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"V2\nVar 2\nVariation 2",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_VAR2, lin_sptr);

	// Phase set
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"PS\nPh set\nPhase set",
		"deg",
		0,
		"%3.0f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_sptr);
}



ParamDescSet &	HarmTremDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	HarmTremDesc::do_get_info () const
{
	return _info;
}



void	HarmTremDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	HarmTremDesc::do_prefer_stereo () const
{
	return true;
}



int	HarmTremDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	HarmTremDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace tremh
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
