/*****************************************************************************

        BigMuff1Desc.cpp
        Author: Laurent de Soras, 2020

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

#include "mfx/pi/bmp1/BigMuff1Desc.h"
#include "mfx/pi/bmp1/Ovrspl.h"
#include "mfx/pi/bmp1/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace bmp1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BigMuff1Desc::BigMuff1Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "bmp1";
	_info._name      = "Big Muff Pi\nBig Muff\nBMP";
	_info._tag_list  = { piapi::Tag::_distortion_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Sustain
	auto           pll_sptr = std::make_shared <TplPll> (
		1e3 / (150e3 + 1e3), 1,
		"Sustain\nSust\nSt",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_SUS, pll_sptr);

	// Tone
	auto           sim_sptr = std::make_shared <param::Simple> ("Tone\nTn");
	_desc_set.add_glob (Param_TONE, sim_sptr);

	// Volume
	pll_sptr = std::make_shared <TplPll> (
		0, 1,
		"Volume\nVol\nVl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_VOL, pll_sptr);

	// Pre-gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.1, 10.0,
		"Pre-gain\nGain\nPG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_PREGAIN, log_sptr);

	// Oversampling rate
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		// U+00D7 multiplication sign (UTF-8 C3 97)
		"\xC3\x97" "1\n" "\xC3\x97" "2\n" "\xC3\x97" "4",
		"Oversampling rate\nOversampling\nOversamp\nOvrspl\nOvrs\nOS",
		"",
		0,
		"%s"
	);
	assert (int (enu_sptr->get_nat_max ()) == Ovrspl_NBR_ELT - 1);
	_desc_set.add_glob (Param_OVRSPL, enu_sptr);
}



ParamDescSet &	BigMuff1Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	BigMuff1Desc::do_get_info () const
{
	return _info;
}



void	BigMuff1Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	BigMuff1Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	BigMuff1Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace bmp1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
