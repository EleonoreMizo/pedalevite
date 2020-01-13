/*****************************************************************************

        FreqShiftDesc.cpp
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

#include "mfx/pi/freqsh/FreqShiftDesc.h"
#include "mfx/pi/freqsh/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace freqsh
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



FreqShiftDesc::FreqShiftDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Frequency
	auto           pll_sptr = std::make_shared <TplPll> (
		-_max_freq, _max_freq,
		"Frequency\nFreq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%+7.1f"
	);
	pll_sptr->use_mapper ().set_first_value (-_max_freq);
	pll_sptr->use_mapper ().add_segment (0.4, -0.004 * _max_freq, true);
	pll_sptr->use_mapper ().add_segment (0.6,  0.004 * _max_freq, false);
	pll_sptr->use_mapper ().add_segment (1.0,          _max_freq, true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FREQ, pll_sptr);
}



ParamDescSet &	FreqShiftDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	FreqShiftDesc::do_get_unique_id () const
{
	return "freqshift1";
}



std::string	FreqShiftDesc::do_get_name () const
{
	return "Frequency Shifter\nFreq Shift\nFShift";
}



void	FreqShiftDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	FreqShiftDesc::do_prefer_stereo () const
{
	return false;
}



int	FreqShiftDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	FreqShiftDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace freqsh
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
