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

#include "mfx/pi/distpwm2/DistoPwm2Desc.h"
#include "mfx/pi/distpwm2/PreFilterType.h"
#include "mfx/pi/distpwm2/PulseType.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
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
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Pre-filter
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Weak\nMild\nStrong", "Pre-filter\nFilter\nFlt", ""
	);
	assert (enu_ptr->get_nbr_elt () == PreFilterType_NBR_ELT);
	_desc_set.add_glob (Param_LPF, enu_ptr);

	// Threshold
	TplPll *       pll_ptr = new TplPll (
		1e-6, 1e-4,
		"Detection threshold\nThreshold\nThresh\nThr",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+4.0f"
	);
	pll_ptr->use_mapper ().gen_log (4, 0);
	_desc_set.add_glob (Param_THR, pll_ptr);

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



std::string	DistoPwm2Desc::do_get_unique_id () const
{
	return "distpwm2";
}



std::string	DistoPwm2Desc::do_get_name () const
{
	return "Pulse Width Modulation distortion 2\nPWM distortion 2\nPWM disto 2\nPWM dis2";
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

	// Pulse type
	const std::string name_pulse =
		  name_l + " pulse type\n"
		+ name_s + " pulse T\n"
		+ name_s + " pulse\n"
		+ name_t + "PT";
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Rect\nRamp\nCycle\nSine\nSaw", name_pulse.c_str (), ""
	);
	assert (enu_ptr->get_nbr_elt () == PulseType_NBR_ELT);
	_desc_set.add_glob (base + ParamOsc_PULSE, enu_ptr);

	// Level
	const std::string name_lvl =
		  name_l + " level\n"
		+ name_s + " level\n"
		+ name_s + " lvl\n"
		+ name_t + "L";
	param::TplLin *   lin_ptr = new param::TplLin (
		0.0, 1.0,
		name_lvl.c_str (),
		"dB",
		0,
		"%+5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB);
	_desc_set.add_glob (base + ParamOsc_LVL, lin_ptr);

	// Pulse frequency
	const std::string name_pw =
		  name_l + " pulse frequency\n"
		+ name_s + " pulse freq\n"
		+ name_s + " P freq\n"
		+ name_s + " PF\n"
		+ name_t + "PF";
	TplPll *       pll_ptr = new TplPll (
		20.0, 20.0 * 1024,
		name_pw.c_str (),
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.1f"
	);
	pll_ptr->use_mapper ().gen_log (10);
	_desc_set.add_glob (base + ParamOsc_PF, pll_ptr);
}



}  // namespace distpwm2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
