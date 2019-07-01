/*****************************************************************************

        OnsetDetect2Desc.cpp
        Author: Laurent de Soras, 2018

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

#include "mfx/pi/osdet2/OnsetDetect2Desc.h"
#include "mfx/pi/osdet2/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace osdet2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OnsetDetect2Desc::OnsetDetect2Desc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Velocity clipping
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Off\nOn",
		"Velocity clipping",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_VELO_CLIP, enu_ptr);

	// High-pass filter cutoff frequency
	TplPll *       pll_ptr = new TplPll (
		40, 4000,
		"High-pass filter cutoff frequency\nHigh-pass filter frequency"
		"\nHigh-pass frequency\nHigh-pass freq\nHP freq\nHP",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_HP, pll_ptr);

	// Low-pass filter 1 cutoff frequency
	pll_ptr = new TplPll (
		10, 100,
		"Low-pass filter 1 cutoff frequency\nLow-pass filter 1 frequency"
		"\nLow-pass 1 frequency\nLow-pass 1 freq\nLP1 freq\nLP1",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.1f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LP1, pll_ptr);

	// Low-pass filter 2 cutoff frequency
	pll_ptr = new TplPll (
		10, 100,
		"Low-pass filter 2 cutoff frequency\nLow-pass filter 2 frequency"
		"\nLow-pass 2 frequency\nLow-pass 2 freq\nLP1 freq\nLP2",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.1f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LP2, pll_ptr);

	// a
	param::TplInt* int_ptr = new param::TplInt (32, 159, "a", "spl", 0, "%3.0f");
	_desc_set.add_glob (Param_A, int_ptr);

	// b
	int_ptr = new param::TplInt (0, 15, "b", "spl", 0, "%3.0f");
	_desc_set.add_glob (Param_B, int_ptr);

	// Beta
	pll_ptr = new TplPll (
		1.0/256, 1,
		"Beta",
		"",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_BETA, pll_ptr);

	// Lambda
	param::Simple *   sim_ptr = new param::Simple ("Lambda\nLmbd");
	_desc_set.add_glob (Param_LAMBDA, sim_ptr);

	// Alpha
	sim_ptr = new param::Simple ("Alpha\nAlph");
	_desc_set.add_glob (Param_ALPHA, sim_ptr);

	// Release threshold
	pll_ptr = new TplPll (
		1e-4, 1e-1,
		"Release threshold\nRelease thr\nRls thr\nRT",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (6);
	_desc_set.add_glob (Param_RLS_THR, pll_ptr);

	// Release ratio
	pll_ptr = new TplPll (
		1, 8,
		"Release ratio\nRls ratio\nRls R\nRR",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%3.0f"
	);
	pll_ptr->use_mapper ().gen_log (6);
	_desc_set.add_glob (Param_RLS_RATIO, pll_ptr);

}



ParamDescSet &	OnsetDetect2Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	OnsetDetect2Desc::do_get_unique_id () const
{
	return "osdet2";
}



std::string	OnsetDetect2Desc::do_get_name () const
{
	return "Onset detector II\nOnset det II\nOnset2\nOSD2";
}



void	OnsetDetect2Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 0;
	nbr_s = 2;
}



bool	OnsetDetect2Desc::do_prefer_stereo () const
{
	return false;
}



int	OnsetDetect2Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	OnsetDetect2Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osdet2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
