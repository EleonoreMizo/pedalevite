/*****************************************************************************

        WahDesc.cpp
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

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/wah1/Param.h"
#include "mfx/pi/wah1/WahDesc.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace wah1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



WahDesc::WahDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "wah1";
	_info._name      = "Wah-wah";
	_info._tag_list  = { piapi::Tag::_eq_filter_0 };

	// Frequency
	auto           log_sptr = std::make_shared <param::TplLog> (
		120, 120 * 32,
		"Frequency\nFreq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	log_sptr->set_flags (piapi::ParamDescInterface::Flags_AUTOLINK);
	_desc_set.add_glob (Param_FREQ, log_sptr);

	// Base Q
	log_sptr = std::make_shared <param::TplLog> (
		1, 32,
		"Selectivity\nQ",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.1f"
	);
	_desc_set.add_glob (Param_Q, log_sptr);
}



ParamDescSet &	WahDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	WahDesc::do_get_info () const
{
	return _info;
}



void	WahDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	WahDesc::do_prefer_stereo () const
{
	return false;
}



int	WahDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	WahDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace wah1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
