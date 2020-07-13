/*****************************************************************************

        PEqDesc.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_peq_PEqDesc_CODEHEADER_INCLUDED)
#define mfx_pi_peq_PEqDesc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/peq/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace peq
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NB>
PEqDesc <NB>::PEqDesc ()
:	_desc_set (Param_NBR_ELT * _nbr_bands, 0)
,	_info ()
{
	_info._unique_id = "peq";
	if (_nbr_bands == 4)
	{
		_info._name = "Parametric Equalizer\nParametric Eq\nParam Eq\nPEq";
	}
	else
	{
		const std::string s = print_nbr_bands ();
		_info._unique_id += s;
		_info._name       =
			    "Parametric Equalizer " + s
			+ "\nParametric Eq "        + s
			+ "\nParam Eq "             + s
			+ "\nPEq "                  + s;
	}
	_info._tag_list = { piapi::Tag::_eq_parametric_0 };

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	for (int band = 0; band < _nbr_bands; ++band)
	{
		const int      base = compute_param_base (band);

		// Filter Type
		auto           enu_sptr = std::make_shared <param::TplEnum> (
			"Peak\nLow shelf\nHigh Pass\nHigh shelf\nLow Pass",
			"B%dT\nB%d Type\nBand %d Type",
			"",
			band + 1,
			"%s"
		);
		_desc_set.add_glob (base + Param_TYPE, enu_sptr);

		// Frequency
		auto           log_sptr = std::make_shared <param::TplLog> (
			20, 20 * 1024,
			"B%dF\nB%d Freq\nBand %d Freq\nBand %d Frequency",
			"Hz",
			param::HelperDispNum::Preset_FLOAT_STD,
			band + 1,
			"%5.0f"
		);
		log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
		_desc_set.add_glob (base + Param_FREQ, log_sptr);

		// Quality Factor
		{
			const double   val_min =  0.10;
			const double   val_max = 20.0;
			auto           pll_sptr = std::make_shared <TplPll> (
				val_min, val_max,
				"B%dQ\nB%d Q\nBand %d Q",
				"",
				param::HelperDispNum::Preset_FLOAT_STD,
				band + 1,
				"%5.2f"
			);
			pll_sptr->use_mapper ().set_first_value (val_min);
			pll_sptr->use_mapper ().add_segment (0.125, 0.25   , true);
			pll_sptr->use_mapper ().add_segment (0.5  , 1.00   , true);
			pll_sptr->use_mapper ().add_segment (0.875, 5.00   , true);
			pll_sptr->use_mapper ().add_segment (1.0  , val_max, true);
			_desc_set.add_glob (base + Param_Q, pll_sptr);
		}

		// Gain
		{
			const double   val_min = 1.0 / (1 << 8);
			const double   val_max =        1 << 4;
			auto           pll_sptr = std::make_shared <TplPll> (
				val_min, val_max,
				"B%dG\nB%d Gain\nBand %d Gain",
				"dB",
				param::HelperDispNum::Preset_DB,
				band + 1,
				"%+5.1f"
			);
			pll_sptr->use_mapper ().set_first_value (val_min);
			pll_sptr->use_mapper ().add_segment (0.125, 0.25   , true);
			pll_sptr->use_mapper ().add_segment (0.5  , 1.00   , true);
			pll_sptr->use_mapper ().add_segment (0.875, 5.00   , true);
			pll_sptr->use_mapper ().add_segment (1.0  , val_max, true);
			_desc_set.add_glob (base + Param_GAIN, pll_sptr);
		}

		// Bypass
		enu_sptr = std::make_shared <param::TplEnum> (
			"Off\nOn",
			"B%dB\nB%d Byp\nB%d Bypass\nBand %d Bypass",
			"",
			band + 1,
			"%s"
		);
		_desc_set.add_glob (base + Param_BYPASS, enu_sptr);
	}
}



template <int NB>
ParamDescSet &	PEqDesc <NB>::use_desc_set ()
{
	return _desc_set;
}



template <int NB>
constexpr int	PEqDesc <NB>::compute_param_base (int band)
{
	return band * Param_NBR_ELT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NB>
piapi::PluginInfo	PEqDesc <NB>::do_get_info () const
{

	return _info;
}



template <int NB>
void	PEqDesc <NB>::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



template <int NB>
bool	PEqDesc <NB>::do_prefer_stereo () const
{
	return false;
}



template <int NB>
int	PEqDesc <NB>::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



template <int NB>
const piapi::ParamDescInterface &	PEqDesc <NB>::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NB>
std::string	PEqDesc <NB>::print_nbr_bands ()
{
	std::string    s;

	int            n = _nbr_bands;
	while (n > 0)
	{
		const int      i       = n % 10;
		const char     c_0 [2] = { char ('0' + i), '\0' };
		s = c_0 + s;
		n /= 10;
	}

	return s;
}



}  // namespace peq
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_peq_PEqDesc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
