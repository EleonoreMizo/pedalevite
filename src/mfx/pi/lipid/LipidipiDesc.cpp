/*****************************************************************************

        LipidipiDesc.cpp
        Author: Laurent de Soras, 2021

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

#include "mfx/pi/lipid/Cst.h"
#include "mfx/pi/lipid/LipidipiDesc.h"
#include "mfx/pi/lipid/Param.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace lipid
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



LipidipiDesc::LipidipiDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "lipidipi";
	_info._name      = "Lipidipi";
	_info._tag_list  = { piapi::Tag::_modulation_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Fat
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, double (Cst::_max_voice_pairs),
		"Fat",
		"",
		0,
		"%2.1f"
	);
	_desc_set.add_glob (Param_FAT, lin_sptr);

	// Grease
	lin_sptr = std::make_shared <param::TplLin> (
		1, double (Cst::_max_pitch),
		"Grease\nGrs",
		"cent",
		0,
		"%2.2f"
	);
	_desc_set.add_glob (Param_GREASE, lin_sptr);
}



ParamDescSet &	LipidipiDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	LipidipiDesc::do_get_info () const
{
	return _info;
}



void	LipidipiDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	LipidipiDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	LipidipiDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
