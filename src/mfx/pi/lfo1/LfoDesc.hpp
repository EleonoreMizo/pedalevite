/*****************************************************************************

        LfoDesc.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_lfo1_LfoDesc_CODEHEADER_INCLUDED)
#define mfx_pi_lfo1_LfoDesc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
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



template <bool SLOW>
LfoDesc <SLOW>::LfoDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	if (SLOW)
	{
		// Period
		param::TplLog *   log_ptr = new param::TplLog (
			0.1, 1000,
			"Period\nPer\nP",
			"s",
			param::HelperDispNum::Preset_FLOAT_STD,
			0,
			"%7.2f"
		);
		log_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
		_desc_set.add_glob (Param_SPEED, log_ptr);
	}
	else
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
	}

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
		"Sine\nTriangle\nSquare\nSaw\nParabola\nBiphase\nN-Phase\nVarislope\nNoise",
		"W\nWf\nWavef\nWaveform",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_WAVEFORM, enu_ptr);
	assert (enu_ptr->get_nat_max () == LfoType_NBR_ELT - 1);

	// Sample and hold
	lin_ptr = new param::TplLin (
		0, 1,
		"SnH\nSplHold\nSample & hold\nSample and hold",
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

	// Chaos amount
	lin_ptr = new param::TplLin (
		0, 1,
		"C\nKos\nChaos\nChaos Amt\nChaos amount",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_CHAOS, lin_ptr);

	// Phase distortion amount
	lin_ptr = new param::TplLin (
		0, 1,
		"PDA\nPhDistA\nPhDistAmt\nPhase dist amt\nPhase dist amount\nPhase distortion amount",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_PH_DIST_AMT, lin_ptr);

	// Phase distortion offset
	lin_ptr = new param::TplLin (
		0, 1,
		"PDO\nPhDistO\nPhDistOfs\nPhase dist ofs\nPhase dist offset\nPhase distortion offset",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_PH_DIST_OFS, lin_ptr);

	// Sign
	enu_ptr = new param::TplEnum (
		"Normal\nInvert",
		"Si\nSig\nSign",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_SIGN, enu_ptr);

	// Polarity
	enu_ptr = new param::TplEnum (
		"Bipolar\nUnipolar",
		"Po\nPol\nPolar\nPolarity",
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

	// Phase set
	lin_ptr = new param::TplLin (
		0, 1,
		"PS\nPhS\nPh set\nPhase set",
		"deg",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_ptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_ptr);
}



template <bool SLOW>
ParamDescSet &	LfoDesc <SLOW>::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <bool SLOW>
void	LfoDesc <SLOW>::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 0;
	nbr_o = 0;
	nbr_s = 1;
}



template <bool SLOW>
bool	LfoDesc <SLOW>::do_prefer_stereo () const
{
	return false;
}



template <bool SLOW>
int	LfoDesc <SLOW>::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



template <bool SLOW>
const piapi::ParamDescInterface &	LfoDesc <SLOW>::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lfo1
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_lfo1_LfoDesc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
