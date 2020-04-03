/*****************************************************************************

        SkoolMoodDesc.cpp
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

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/param/TplPan.h"
#include "mfx/pi/smood/LfoType.h"
#include "mfx/pi/smood/Param.h"
#include "mfx/pi/smood/SkoolMoodDesc.h"
#include "mfx/pi/ParamMapFdbkBipolar.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace smood
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SkoolMoodDesc::SkoolMoodDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "smood";
	_info._name      = "Skool-Mood\nSklMood\nSMood";
	_info._tag_list  = { piapi::Tag::_modulation_0, piapi::Tag::_spatial_0 };

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

	// Width
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1.4,
		"Width\nWd",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_WIDTH, lin_sptr);

	// Depth
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Depth\nDp",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_DEPTH, lin_sptr);

	// Feedback level
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

	// Phase mix
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

	// Stereo difference
   lin_sptr = std::make_shared <param::TplLin> (
		-0.5, 0.5,
		"Stereo difference\nWd",
		"\xC2\xB0", // U+00B0 DEGREE SIGN
		0,
		"%+4.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_STDIF, lin_sptr);

	// Channel crossmix
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Channel crossmix\nChn crossmix\nChn Xmix\nChn X\nCx",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_CHNCROSS, lin_sptr);

	// Stereo panning
	auto           pan_sptr = std::make_shared <param::TplPan> (
		"Stereo panning\nPanning\nPan",
		0
	);
	_desc_set.add_glob (Param_PAN, pan_sptr);

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
	lin_sptr = std::make_shared <param::TplLin> (
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



ParamDescSet &	SkoolMoodDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	SkoolMoodDesc::do_get_info () const
{
	return _info;
}



void	SkoolMoodDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	SkoolMoodDesc::do_prefer_stereo () const
{
	return true;
}



int	SkoolMoodDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SkoolMoodDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace smood
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
