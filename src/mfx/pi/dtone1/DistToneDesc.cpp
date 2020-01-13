/*****************************************************************************

        DistToneDesc.cpp
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

#include "mfx/pi/dtone1/DistToneDesc.h"
#include "mfx/pi/dtone1/Param.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dtone1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistToneDesc::DistToneDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Tone
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Tone",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_TONE, lin_sptr);

	// Mid
	// Internally maps from -0.2 to 1.0, 0.4 being neutral (flat with 50% tone)
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Mid Boost\nMid",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_MID, lin_sptr);

	// Center
	auto           log_sptr = std::make_shared <param::TplLog> (
		300, 1200,
		"Mid Frequency\nMid Freq\nMidF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_CENTER, log_sptr);
}



ParamDescSet &	DistToneDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DistToneDesc::do_get_unique_id () const
{
	return "dtone1";
}



std::string	DistToneDesc::do_get_name () const
{
	return "Distortion Tone Stage\nDistortion Tone\nDist Tone";
}



void	DistToneDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DistToneDesc::do_prefer_stereo () const
{
	return false;
}



int	DistToneDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistToneDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dtone1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
