/*****************************************************************************

        IIFixDesc.cpp
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

#include "mfx/pi/iifix/IIFixDesc.h"
#include "mfx/pi/iifix/Param.h"
#include "mfx/pi/param/TplLog.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace iifix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



IIFixDesc::IIFixDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Center
	param::TplLog *   log_ptr = new param::TplLog (
		2000, 16000,
		"Cutoff Frequency\nCutoff Freq\nCutoff",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ, log_ptr);

	// Level
	log_ptr = new param::TplLog (
		1, 4,
		"Level",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_LEVEL, log_ptr);
}



ParamDescSet &	IIFixDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	IIFixDesc::do_get_unique_id () const
{
	return "iifix";
}



std::string	IIFixDesc::do_get_name () const
{
	return "Input Impedance Fix\nImpedance Fix\nImp Fix";
}



void	IIFixDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	IIFixDesc::do_prefer_stereo () const
{
	return false;
}



int	IIFixDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	IIFixDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iifix
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
