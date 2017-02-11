/*****************************************************************************

        NoiseChlorineDesc.cpp
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
#include "mfx/pi/nzcl/Cst.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/nzcl/Param.h"

#include <array>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace nzcl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NoiseChlorineDesc::NoiseChlorineDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Global level
	TplPll *   pll_ptr = new TplPll (
		1e-7, 1,
		"Global level\nGlob level\nGlob lvl\nGLvl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (   1e-7);
	pll_ptr->use_mapper ().add_segment (0.75,  1e-4, true);
	pll_ptr->use_mapper ().add_segment (1    , 1   , true);
	_desc_set.add_glob (Param_LVL, pll_ptr);

	// Notches
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		add_notch (index);
	}
}



ParamDescSet &	NoiseChlorineDesc::use_desc_set ()
{
	return _desc_set;
}



int	NoiseChlorineDesc::get_base_notch (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_notches);

	return Param_BASE_NOTCH + index * ParamNotch_NBR_ELT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	NoiseChlorineDesc::do_get_unique_id () const
{
	return "noisechlorine";
}



std::string	NoiseChlorineDesc::do_get_name () const
{
	return "Noise chlorine\nNoise cl\nNoisCl";
}



void	NoiseChlorineDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	NoiseChlorineDesc::do_prefer_stereo () const
{
	return false;
}



int	NoiseChlorineDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	NoiseChlorineDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NoiseChlorineDesc::add_notch (int index)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	const int      base = get_base_notch (index);

	// Notch N frequency
	param::TplLog *   log_ptr = new param::TplLog (
		20, 20480,
		"Notch %d frequency\nNotch %d freq\nN %d freq\nN%dF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%7.1f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamNotch_FREQ, log_ptr);

	// Notch N selectivity
	log_ptr = new param::TplLog (
		0.125, 64,
		"Notch %d selectivity\nNotch %d Q\nN %d Q\nN%dQ",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.2f"
	);
	_desc_set.add_glob (base + ParamNotch_Q, log_ptr);

	// Notch N level
	TplPll *   pll_ptr = new TplPll (
		0, 16,
		"Notch %d level\nNotch %d lvl\nN %d lvl\nN%dL",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (    0);
	pll_ptr->use_mapper ().add_segment (0.25,   1.0 / 256, false);
	pll_ptr->use_mapper ().add_segment (1    , 16        , true );
	_desc_set.add_glob (base + ParamNotch_LVL, pll_ptr);
}



}  // namespace nzcl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
