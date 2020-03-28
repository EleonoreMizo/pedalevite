/*****************************************************************************

        DistoPwm2Desc.cpp
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

#include "mfx/pi/distpwm2/DetectionMethod.h"
#include "mfx/pi/distpwm2/DistoPwm2Desc.h"
#include "mfx/pi/distpwm2/PreFilterType.h"
#include "mfx/pi/distpwm2/PulseType.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/MapSq.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace distpwm2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistoPwm2Desc::DistoPwm2Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "distpwm2";
	_info._name      =
		"Pulse Width Modulation distortion 2\nPWM distortion 2\n"
		"PWM disto 2\nPWM dis2";

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Pre-filter
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Weak\nMild\nStrong", "Pre-filter\nFilter\nFlt", ""
	);
	assert (enu_sptr->get_nbr_elt () == PreFilterType_NBR_ELT);
	_desc_set.add_glob (Param_LPF, enu_sptr);

	// Detection method
	enu_sptr = std::make_shared <param::TplEnum> (
		"0-X\nPeak", "Detection method\nDetection\nDetect\nDet", ""
	);
	assert (enu_sptr->get_nbr_elt () == DetectionMethod_NBR_ELT);
	_desc_set.add_glob (Param_DET, enu_sptr);

	// Threshold
	auto           pll_sptr = std::make_shared <TplPll> (
		1e-6, 1e-4,
		"Detection threshold\nThreshold\nThresh\nThr",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+4.0f"
	);
	pll_sptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (Param_THR, pll_sptr);

	// Density
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Density\nDens",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_DENS, lin_sptr);

	// Sustain level
	pll_sptr = std::make_shared <TplPll> (
		0, 1,
		"Sustain level\nSustain lvl\nSustain\nSust\nSus",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (7, 2);
	_desc_set.add_glob (Param_SUST, pll_sptr);

	// Gate level
	pll_sptr = std::make_shared <TplPll> (
		1e-4, 0.1,
		"Gate level\nGate lvl\nGate\nGat",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+6.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_GATE, pll_sptr);

	init_osc (Param_OSC_STD , "Standard", "Std" , "St");
	init_osc (Param_OSC_OCT , "Double"  , "Dbl" , "Db");
	init_osc (Param_OSC_SUB1, "Sub 1"   , "Sub1", "S1");
	init_osc (Param_OSC_SUB2, "Sub 2"   , "Sub2", "S1");
}



ParamDescSet &	DistoPwm2Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	DistoPwm2Desc::do_get_info () const
{
	return _info;
}



void	DistoPwm2Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DistoPwm2Desc::do_prefer_stereo () const
{
	return false;
}



int	DistoPwm2Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistoPwm2Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DistoPwm2Desc::init_osc (Param base, std::string name_l, std::string name_s, std::string name_t)
{
	assert (base >= 0);
	assert (base < Param_NBR_ELT);
	assert (! name_l.empty ());
	assert (! name_s.empty ());
	assert (! name_t.empty ());

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapSq <false> > TplSq;

	// Pulse type
	const std::string name_pulse =
		  name_l + " pulse type\n"
		+ name_s + " pulse T\n"
		+ name_s + " pulse\n"
		+ name_t + "PT";
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Rect\nRamp\nCycle\nSine\nSaw", name_pulse.c_str (), ""
	);
	assert (enu_sptr->get_nbr_elt () == PulseType_NBR_ELT);
	_desc_set.add_glob (base + ParamOsc_PULSE, enu_sptr);

	// Level
	const std::string name_lvl =
		  name_l + " level\n"
		+ name_s + " level\n"
		+ name_s + " lvl\n"
		+ name_t + "L";
	auto           sq_sptr = std::make_shared <TplSq> (
		0.0, 1.0,
		name_lvl.c_str (),
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (base + ParamOsc_LVL, sq_sptr);

	// Pulse frequency
	const std::string name_pw =
		  name_l + " pulse frequency\n"
		+ name_s + " pulse freq\n"
		+ name_s + " P freq\n"
		+ name_s + " PF\n"
		+ name_t + "PF";
	auto           pll_sptr = std::make_shared <TplPll> (
		20.0, 20.0 * 1024,
		name_pw.c_str (),
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.1f"
	);
	pll_sptr->use_mapper ().gen_log (10);
	_desc_set.add_glob (base + ParamOsc_PF, pll_sptr);
}



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
