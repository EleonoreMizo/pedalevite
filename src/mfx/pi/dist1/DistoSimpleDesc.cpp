/*****************************************************************************

        DistoSimpleDesc.cpp
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

#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dist1/Param.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dist1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const double	DistoSimpleDesc::_gain_min = 0.1;
const double	DistoSimpleDesc::_gain_max = 1000;



DistoSimpleDesc::DistoSimpleDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "dist1";
	_info._name      = "Simple distortion\nSimple dist\nDist S";
	_info._tag_list  = { piapi::Tag::_distortion_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		double (_gain_min), double (_gain_max),
		"Distortion Gain\nGain",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_sptr);

	// HPF Cutoff frequency
	log_sptr = std::make_shared <param::TplLog> (
		3, 3000,
		"Input high-pass filter cutoff frequency\nInput HPF Frequency"
		"\nHPF Cutoff Frequency\nHPF Frequency\nHPF Freq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HPF_FREQ, log_sptr);

	// Bias
	auto           lin_sptr = std::make_shared <param::TplLin> (
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
}



ParamDescSet &	DistoSimpleDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	DistoSimpleDesc::do_get_info () const
{
	return _info;
}



void	DistoSimpleDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	DistoSimpleDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistoSimpleDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dist1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
