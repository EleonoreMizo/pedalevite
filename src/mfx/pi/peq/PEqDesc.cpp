/*****************************************************************************

        PEqDesc.cpp
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

#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/peq/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace peq
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PEqDesc::PEqDesc ()
:	_desc_set (Param_NBR_ELT * _nbr_bands, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	for (int band = 0; band < _nbr_bands; ++band)
	{
		const int      base = compute_param_base (band);

		// Filter Type
		param::TplEnum *  enu_ptr = new param::TplEnum (
			"Peak\nLow shelf\nHigh Pass\nHigh shelf\nLow Pass",
			"B%dT\nB%d Type\nBand %d Type",
			"",
			band + 1,
			"%s"
		);
		_desc_set.add_glob (base + Param_TYPE, enu_ptr);

		// Frequency
		param::TplLog *   log_ptr = new param::TplLog (
			20, 20 * 1024,
			"B%dF\nB%d Freq\nBand %d Freq\nBand %d Frequency",
			"Hz",
			param::HelperDispNum::Preset_FLOAT_STD,
			band + 1,
			"%5.0f"
		);
		log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
		_desc_set.add_glob (base + Param_FREQ, log_ptr);

		// Quality Factor
		{
			const double   val_min =  0.10;
			const double   val_max = 20.0;
			TplPll *       pll_ptr = new TplPll (
				val_min, val_max,
				"B%dQ\nB%d Q\nBand %d Q",
				"",
				param::HelperDispNum::Preset_FLOAT_STD,
				band + 1,
				"%5.2f"
			);
			pll_ptr->use_mapper ().set_first_value (val_min);
			pll_ptr->use_mapper ().add_segment (0.125, 0.25   , true);
			pll_ptr->use_mapper ().add_segment (0.5  , 1.00   , true);
			pll_ptr->use_mapper ().add_segment (0.875, 5.00   , true);
			pll_ptr->use_mapper ().add_segment (1.0  , val_max, true);
			_desc_set.add_glob (base + Param_Q, pll_ptr);
		}

		// Gain
		{
			const double   val_min = 1.0 / (1 << 8);
			const double   val_max =        1 << 4;
			TplPll *       pll_ptr = new TplPll (
				val_min, val_max,
				"B%dG\nB%d Gain\nBand %d Gain",
				"dB",
				param::HelperDispNum::Preset_DB,
				band + 1,
				"%+5.1f"
			);
			pll_ptr->use_mapper ().set_first_value (val_min);
			pll_ptr->use_mapper ().add_segment (0.125, 0.25   , true);
			pll_ptr->use_mapper ().add_segment (0.5  , 1.00   , true);
			pll_ptr->use_mapper ().add_segment (0.875, 5.00   , true);
			pll_ptr->use_mapper ().add_segment (1.0  , val_max, true);
			_desc_set.add_glob (base + Param_GAIN, pll_ptr);
		}

		// Bypass
		enu_ptr = new param::TplEnum (
			"Off\nOn",
			"B%dB\nB%d Byp\nB%d Bypass\nBand %d Bypass",
			"",
			band + 1,
			"%s"
		);
		_desc_set.add_glob (base + Param_BYPASS, enu_ptr);
	}
}



ParamDescSet &	PEqDesc::use_desc_set ()
{
	return _desc_set;
}



int	PEqDesc::compute_param_base (int band)
{
	return band * Param_NBR_ELT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	PEqDesc::do_get_unique_id () const
{
	return "peq";
}



std::string	PEqDesc::do_get_name () const
{
	return "Parametric Equalizer\nParametric Eq\nParam Eq\nPEq";
}



void	PEqDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	PEqDesc::do_prefer_stereo () const
{
	return false;
}



int	PEqDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	PEqDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace peq
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
