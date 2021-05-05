/*****************************************************************************

        SpectralFreezeDesc.cpp
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

#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
#include "mfx/pi/sfreeze/Param.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/piapi/Tag.h"

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace sfreeze
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpectralFreezeDesc::SpectralFreezeDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "specfreeze";
	_info._name      = "Spectral freeze\nSpec freeze\nSpeFrz";
	_info._tag_list  = { piapi::Tag::_spectral_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Freeze
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn", "Freeze\nFrz", "", 0, "%s"
	);
	_desc_set.add_glob (Param_FREEZE, enu_sptr);
}



ParamDescSet &	SpectralFreezeDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	SpectralFreezeDesc::do_get_info () const
{
	return _info;
}



void	SpectralFreezeDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	SpectralFreezeDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SpectralFreezeDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace sfreeze
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
