/*****************************************************************************

        MidSideDesc.cpp
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

#include "mfx/pi/ms/MidSideDesc.h"
#include "mfx/pi/ms/Op.h"
#include "mfx/pi/ms/Param.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace ms
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



MidSideDesc::MidSideDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "midside";
	_info._name      = "Mid-Side coder\nMid-Side\nM-S";
	_info._tag_list  = { piapi::Tag::_mixing_0, piapi::Tag::_utility_0 };
	_info._chn_pref  = piapi::ChnPref::STEREO;

	// Operation
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"None\nSt->MS\nMS->St",
		"Operation\nOper\nOp",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == Op_NBR_ELT - 1);
	_desc_set.add_glob (Param_OP, enu_sptr);

	// Channel swap
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Channel swap\nChn swap\nCSw",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_CHN_SWAP, enu_sptr);

	// Left polarity
	enu_sptr = std::make_shared <param::TplEnum> (
		"Normal\nInvert",
		"Left polarity\nLeft pol\nL pol\nLPo",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_POL_L, enu_sptr);

	// Right polarity
	enu_sptr = std::make_shared <param::TplEnum> (
		"Normal\nInvert",
		"Right polarity\nRight pol\nR pol\nRPo",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_POL_R, enu_sptr);
}



ParamDescSet &	MidSideDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	MidSideDesc::do_get_info () const
{
	return _info;
}



void	MidSideDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	MidSideDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	MidSideDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ms
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
