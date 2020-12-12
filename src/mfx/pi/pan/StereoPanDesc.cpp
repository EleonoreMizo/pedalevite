/*****************************************************************************

        StereoPanDesc.cpp
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

#include "mfx/pi/pan/StereoPanDesc.h"
#include "mfx/pi/pan/Param.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplPan.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace pan
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



StereoPanDesc::StereoPanDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "stereopan";
	_info._name      = "Stereo panning\nStereo pan\nSt pan\nPan";
	_info._tag_list  = { piapi::Tag::_mixing_0 };
	_info._chn_pref  = piapi::ChnPref::STEREO;

	// Balance
	auto           pan_sptr = std::make_shared <param::TplPan> (
		"Balance\nBal",
		0
	);
	_desc_set.add_glob (Param_POS, pan_sptr);

	// Position, left channel
	pan_sptr = std::make_shared <param::TplPan> (
		"Position, left channel\nPosition, left chn\nPosition, left\nPos left\nPos L\nPL",
		0
	);
	_desc_set.add_glob (Param_POS_L, pan_sptr);

	// Position, right channel
	pan_sptr = std::make_shared <param::TplPan> (
		"Position, right channel\nPosition, right chn\nPosition, right\nPos right\nPos R\nPR",
		0
	);
	_desc_set.add_glob (Param_POS_R, pan_sptr);

	// Panning law
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.5, fstb::SQRT2 * 0.5,
		"Panning law\nPan law\nLaw",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+4.1f"
	);
	_desc_set.add_glob (Param_LAW, log_sptr);
}



ParamDescSet &	StereoPanDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	StereoPanDesc::do_get_info () const
{
	return _info;
}



void	StereoPanDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	StereoPanDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	StereoPanDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace pan
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
