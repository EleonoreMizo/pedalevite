/*****************************************************************************

        NoiseBleachDesc.cpp
        Author: Laurent de Soras, 2017

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
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/nzbl/Cst.h"
#include "mfx/pi/nzbl/NoiseBleachDesc.h"
#include "mfx/pi/nzbl/Param.h"

#include <array>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace nzbl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NoiseBleachDesc::NoiseBleachDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "noisebleach";
	_info._name      = "Noise bleach\nNoise bl\nNoisBl";

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Global level
	auto           pll_sptr = std::make_shared <TplPll> (
		1e-7, 1,
		"Global level\nGlob level\nGlob lvl\nGLvl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (   1e-7);
	pll_sptr->use_mapper ().add_segment (0.75,  1e-4, true);
	pll_sptr->use_mapper ().add_segment (1    , 1   , true);
	_desc_set.add_glob (Param_LVL, pll_sptr);

	// Bands
	for (int index = 0; index < Cst::_nbr_bands; ++index)
	{
		add_band (index);
	}
}



ParamDescSet &	NoiseBleachDesc::use_desc_set ()
{
	return _desc_set;
}



int	NoiseBleachDesc::get_base_band (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_bands);

	return Param_BASE_BAND + index * ParamBand_NBR_ELT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	NoiseBleachDesc::do_get_info () const
{
	return _info;
}



void	NoiseBleachDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	NoiseBleachDesc::do_prefer_stereo () const
{
	return false;
}



int	NoiseBleachDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	NoiseBleachDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NoiseBleachDesc::add_band (int index)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	const int      base = get_base_band (index);

	static const std::array <const char *, Cst::_nbr_bands> band_range_0_arr =
	{{
		"20-125", "125-250", "250-500", "500-1k",
		"1k-2k", "2k-4k", "4k-8k", "8k-20k"
	}};

	// Band N level
	const std::string range = band_range_0_arr [index];
	const std::string name  =
		"Band " + range + " level\nLevel " + range +"\n"
		"Lvl " + range + "\n" + range + "\nB%dL";
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 16,
		name.c_str (),
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (    0);
	pll_sptr->use_mapper ().add_segment (0.25,   1.0 / 256, false);
	pll_sptr->use_mapper ().add_segment (1    , 16        , true );
	_desc_set.add_glob (base + ParamBand_LVL, pll_sptr);
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
