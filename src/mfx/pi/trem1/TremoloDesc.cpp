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
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/trem1/Param.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/trem1/Waveform.h"

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
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Period
	TplPll *   pll_ptr = new TplPll (
		0.5, 50,
		"Frequency\nFreq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.3f"
	);
	pll_ptr->use_mapper ().set_first_value (   0.5);
	pll_ptr->use_mapper ().add_segment (0.25,  4  , true);
	pll_ptr->use_mapper ().add_segment (1   , 50  , true);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_HZ);
	_desc_set.add_glob (Param_FREQ, pll_ptr);

	// Amount
	pll_ptr = new TplPll (
		0, 10,
		"Amount\nAmt",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (   0);
	pll_ptr->use_mapper ().add_segment (0.25,  0.25, false);
	pll_ptr->use_mapper ().add_segment (0.75,  1.5 , true);
	pll_ptr->use_mapper ().add_segment (1   , 10   , true);
	_desc_set.add_glob (Param_AMT, pll_ptr);

	// Waveform
	param::TplEnum *  enum_ptr = new param::TplEnum (
		"Sine\nSquare\nTriangle\nSaw Up\nSaw Down",
		"Waveform\nWF",
		""
	);
	assert (enum_ptr->get_nat_max () == Waveform_NBR_ELT - 1);
	_desc_set.add_glob (Param_WF, enum_ptr);

	// Gain Saturation
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"Gain Staturation\nGain Sat\nGSat",
		"%",
		0,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_GSAT, lin_ptr);

	// Bias
	lin_ptr = new param::TplLin (
		-1, 1,
		"Bias",
		"%",
		0,
		"%+6.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (Param_BIAS, lin_ptr);
}



ParamDescSet & TremoloDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	TremoloDesc::do_get_unique_id () const
{
	return "tremolo1";
}



std::string	TremoloDesc::do_get_name () const
{
	return "Tremolo";
}



void	TremoloDesc::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	nbr_i = 1;
	nbr_o = 1;
}



bool	TremoloDesc::do_prefer_stereo () const
{
	return false;
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
