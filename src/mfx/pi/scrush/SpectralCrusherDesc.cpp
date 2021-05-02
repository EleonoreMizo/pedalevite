/*****************************************************************************

        SpectralCrusherDesc.cpp
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

#include "mfx/pi/scrush/SpectralCrusherDesc.h"
#include "mfx/pi/scrush/Param.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/piapi/Tag.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace scrush
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpectralCrusherDesc::SpectralCrusherDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "specrush";
	_info._name      = "Spectral crusher\nSpectral crush\nSpec crush\nSpeCru";
	_info._tag_list  = { piapi::Tag::_distortion_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Step
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.5, 8,
		"Step\nStp",
		"dB",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%+5.1f"
	);
	log_sptr->use_disp_num ().set_scale (fstb::LOG10_2 * 20);
	_desc_set.add_glob (Param_STEP, log_sptr);

	// Rounding bias
	auto           sim_sptr = std::make_shared <param::Simple> (
		"Rounding bias\nRound bias\nBias\nBia"
	);
	_desc_set.add_glob (Param_BIAS, sim_sptr);

	// Threshold
	log_sptr = std::make_shared <param::TplLog> (
		sqrt (1e-9), sqrt (1e-1),
		"Threshold\nThresh\nThr",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+4.0f"
	);
	_desc_set.add_glob (Param_THR, log_sptr);

	// Quantification shape
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 2,
		"Quantification shape\nQuant shape\nQ shape\nShape\nQSh",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_SHAPE, lin_sptr);

	// Linear difference
	sim_sptr = std::make_shared <param::Simple> ("Linear difference\nLinear dif\nLin dif\nLDif\nLdf");
	_desc_set.add_glob (Param_DIF, sim_sptr);

	// Lowest frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Lowest frequency\nLowest freq\nLow freq\nLow freq\nLF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LB, log_sptr);

	// Highest frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Highest frequency\nHighest freq\nHigh freq\nHi freq\nHF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HB, log_sptr);
}



ParamDescSet &	SpectralCrusherDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	SpectralCrusherDesc::do_get_info () const
{
	return _info;
}



void	SpectralCrusherDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	SpectralCrusherDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SpectralCrusherDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace scrush
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
