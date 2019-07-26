/*****************************************************************************

        DistApfDesc.cpp
        Author: Laurent de Soras, 2019

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

#include "mfx/pi/distapf/DistApfDesc.h"
#include "mfx/pi/distapf/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/param/TplEnum.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace distapf
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DistApfDesc::DistApfDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Gain
	TplPll *       pll_ptr = new TplPll (
		1, 1024,
		"Gain\nG",
		"db",
		param::HelperDispNum::Preset_DB,
		0,
		"%5.1f"
	);
	pll_ptr->use_mapper ().gen_log (10);
	_desc_set.add_glob (Param_GAIN, pll_ptr);

	// Slew rate limiting
	pll_ptr = new TplPll (
		250, 16000,
		"Slew rate limiting\nSlew rate limit\nSlew rate lim\nSlew rate\nSRL",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (6);
	_desc_set.add_glob (Param_SRL, pll_ptr);

	// Oversampling rate
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"\xC3\x97" "1\n" "\xC3\x97" "4", // U+00D7 multiplication sign (UTF-8 C3 97)
		"Oversampling rate\nOversampling\nOversamp\nOvrspl\nOvrs\nOS",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_OVRS, enu_ptr);
}



ParamDescSet &	DistApfDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DistApfDesc::do_get_unique_id () const
{
	return "distapf";
}



std::string	DistApfDesc::do_get_name () const
{
	return "All-pass filter distortion\nAll-pass distortion\nAll-pass dist\nAPF dist";
}



void	DistApfDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DistApfDesc::do_prefer_stereo () const
{
	return false;
}



int	DistApfDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DistApfDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace distapf
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
