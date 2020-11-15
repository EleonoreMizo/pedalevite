/*****************************************************************************

        TremoloDesc.cpp
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

#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/trem1/Param.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/trem1/Waveform.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace trem1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TremoloDesc::TremoloDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "tremolo1";
	_info._name      = "Tremolo";
	_info._tag_list  = { piapi::Tag::_modulation_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Speed
	auto           pll_sptr = std::make_shared <TplPll> (
		0.5, 50,
		"Speed\nSpd",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_sptr->use_mapper ().set_first_value (   0.5);
	pll_sptr->use_mapper ().add_segment (0.25,  4  , true);
	pll_sptr->use_mapper ().add_segment (0.75 , 10 , false);
	pll_sptr->use_mapper ().add_segment (1    , 50 , true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_HZ);
	_desc_set.add_glob (Param_FREQ, pll_sptr);

	// Amount
	pll_sptr = std::make_shared <TplPll> (
		0, 10,
		"Amount\nAmt",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (   0);
	pll_sptr->use_mapper ().add_segment (0.25,  0.25, false);
	pll_sptr->use_mapper ().add_segment (0.75,  1.5 , true);
	pll_sptr->use_mapper ().add_segment (1   , 10   , true);
	_desc_set.add_glob (Param_AMT, pll_sptr);

	// Waveform
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Sine\nSquare\nTriangle\nSaw Up\nSaw Down",
		"Waveform\nWF",
		""
	);
	assert (enu_sptr->get_nat_max () == Waveform_NBR_ELT - 1);
	_desc_set.add_glob (Param_WF, enu_sptr);

	// Gain Saturation
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Gain Staturation\nGain Sat\nGSat",
		"%",
		0,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_GSAT, lin_sptr);

	// Bias
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Bias",
		"%",
		0,
		"%+6.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_BIAS, lin_sptr);
}



ParamDescSet & TremoloDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	TremoloDesc::do_get_info () const
{
	return _info;
}



void	TremoloDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	TremoloDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	TremoloDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace trem1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
