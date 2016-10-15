/*****************************************************************************

        LfoDesc.cpp
        Author: Laurent de Soras, 2016

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

#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/lfo1/LfoDesc.h"
#include "mfx/pi/lfo1/LfoType.h"
#include "mfx/pi/lfo1/Param.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace lfo1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



LfoDesc::LfoDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Speed
	param::TplLog *   log_ptr = new param::TplLog (
		0.01, 100,
		"Sp\nSpd\nSpeed",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_SPEED, log_ptr);

	// Amplitude
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"A\nAmp\nAmplitude",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_AMP, lin_ptr);

	// Waveform
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Sine\nTriangle\nHalf-Sine\nSquare\nSaw\nBiphase\nN-Phase\nVarislope\nNoise",
		"W\nWavef\nWaveform",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_WAVEFORM, enu_ptr);
	assert (enu_ptr->get_nat_max () == LfoType_NBR_ELT - 1);

	// Sample and Hold
	lin_ptr = new param::TplLin (
		0, 1,
		"SnH\nSplHold\nSample & Hold\nSample And Hold",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SNH, lin_ptr);

	// Smoothing
	lin_ptr = new param::TplLin (
		0, 1,
		"Sm\nSmooth\nSmoothing",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_SMOOTH, lin_ptr);

	// Chaos Amount
	lin_ptr = new param::TplLin (
		0, 1,
		"C\nChaos\nChaos Amt\nChaos Amount",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_CHAOS, lin_ptr);

	// Phase Distortion Amount
	lin_ptr = new param::TplLin (
		0, 1,
		"PDA\nPhDistA\nPhDistAmt\nPhase Dist Amt\nPhase Dist Amount\nPhase Distortion Amount",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_PH_DIST_AMT, lin_ptr);

	// Phase Distortion Offset
	lin_ptr = new param::TplLin (
		0, 1,
		"PDO\nPhDistO\nPhDistOfs\nPhase Dist Ofs\nPhase Dist Offset\nPhase Distortion Offset",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_PH_DIST_OFS, lin_ptr);

	// Sign
	enu_ptr = new param::TplEnum (
		"Normal\nInvert",
		"Si\nSign",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_SIGN, enu_ptr);

	// Polarity
	enu_ptr = new param::TplEnum (
		"Bipolar\nUnipolar",
		"Po\nPolar\nPolarity",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_POLARITY, enu_ptr);

	// Variation 1
	lin_ptr = new param::TplLin (
		0, 1,
		"V1\nVar 1\nVariation 1",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_VAR1, lin_ptr);

	// Variation 2
	lin_ptr = new param::TplLin (
		0, 1,
		"V2\nVar 2\nVariation 2",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_VAR2, lin_ptr);

	// Phase Set
	lin_ptr = new param::TplLin (
		0, 1,
		"PS\nPh Set\nPhase Set",
		"deg",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_ptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_ptr);
}



ParamDescSet &	LfoDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	LfoDesc::do_get_unique_id () const
{
	return "lfo1";
}



std::string	LfoDesc::do_get_name () const
{
	return "LFO";
}



void	LfoDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 0;
	nbr_o = 0;
	nbr_s = 1;
}



bool	LfoDesc::do_prefer_stereo () const
{
	return false;
}



int	LfoDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	LfoDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
