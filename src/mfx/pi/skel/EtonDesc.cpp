/*****************************************************************************

        EtonDesc.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/skel/EtonDesc.h"
#include "mfx/pi/skel/Param.h"
#include "mfx/pi/param/TplLog.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace skel
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EtonDesc::EtonDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// XYZ
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.125, 8,
		"Level\nLvl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_XYZ, log_sptr);
}



ParamDescSet &	EtonDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	EtonDesc::do_get_unique_id () const
{
	return "";
}



std::string	EtonDesc::do_get_name () const
{
	return "";
}



void	EtonDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	EtonDesc::do_prefer_stereo () const
{
	return false;
}



int	EtonDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	EtonDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace skel
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
