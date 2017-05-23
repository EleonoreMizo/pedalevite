/*****************************************************************************

        Delay2Desc.cpp
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

#include "mfx/pi/dly2/Cst.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dly2/FilterType.h"
#include "mfx/pi/dly2/Param.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/param/TplPan.h"
#include "mfx/pi/ParamMapFdbk.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Delay2Desc::Delay2Desc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapS <false> > TplS;

	// Tap gain in
	param::Simple *   sim_ptr =
		new param::Simple ("Tap gain in\nTap G in\nTap in\nTGI");
	_desc_set.add_glob (ParamLine_GAIN_IN, sim_ptr);

	// Tap global volume 
	TplPll *       pll_ptr = new TplPll (
		0, 4,
		"Tap global volume\nTap glob vol\nTap vol\nTapV\nTV",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (Param_TAPS_VOL, pll_ptr);

	// Dry volume 
	pll_ptr = new TplPll (
		0, 4,
		"Dry volume\nDry vol\nDryV\nDV",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (Param_DRY_VOL, pll_ptr);

	// Dry spread
	TplS *         maps_ptr = new TplS (
		-1, 1,
		"Dry spread\nDry Spr\nDSpr\nDS",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%+6.1f"
	);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_DRY_SPREAD, maps_ptr);

	// Freeze lines
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Off\nOn",
		"Freeze lines\nFreeze\nFrz",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_FREEZE, enu_ptr);

	// Number of feedback lines
	param::TplInt *   int_ptr = new param::TplInt (
		0, Cst::_nbr_lines,
		"Number of feedback lines\nNb of feedback lines\nFeedback lines\nFdbk lines\nNLines\nNL",
		"",
		0,
		"%.0f"
	);
	_desc_set.add_glob (Param_NBR_LINES, int_ptr);

	// Cross-freedback
	maps_ptr = new TplS (
			0, 1,
			"Ping-pong feedback\nPing-pong\nPiPo\nPP",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_X_FDBK, maps_ptr);

	// Taps
	for (int index = 0; index < Cst::_nbr_taps; ++index)
	{
		init_tap (index);
	}

	// Delay lines
	for (int index = 0; index < Cst::_nbr_lines; ++index)
	{
		init_line (index);
	}
}



ParamDescSet &	Delay2Desc::use_desc_set ()
{
	return _desc_set;
}



int	Delay2Desc::get_tap_base (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_taps);

	return Param_BASE_TAP + index * ParamTap_NBR_ELT;
}



int	Delay2Desc::get_line_base (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_lines);

	return Param_BASE_LINE + index * ParamLine_NBR_ELT;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	Delay2Desc::do_get_unique_id () const
{
	return "dly2";
}



std::string	Delay2Desc::do_get_name () const
{
	return "SiemensGirlz\nSiemGirlz\nSGirlz";
}



void	Delay2Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	Delay2Desc::do_prefer_stereo () const
{
	return true;
}



int	Delay2Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Delay2Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Delay2Desc::init_tap (int index)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	const int      base = get_tap_base (index);

	// Tap input gain
	TplPll *       pll_ptr = new TplPll (
		0, 4,
		"Tap %d input gain\nTap %d gain\nT%d gain\nT%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamTap_GAIN_IN, pll_ptr);

	// Tap spread
	param::TplPan *   pan_ptr = new param::TplPan (
		"Tap %d spread\nTap %d spr\nT%d spr\nT%dS",
		index + 1
	);
	_desc_set.add_glob (base + ParamTap_SPREAD, pan_ptr);

	// Tap delay time, base
	pll_ptr = new TplPll (
		0, 4,
		"Tap %d delay time, base\nTap %d time base\nTap %d time B\nT%d time B\nT%dTB",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		index + 1,
		"%6.1f"
	);
	pll_ptr->use_mapper ().gen_log (5, 2);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (base + ParamTap_DLY_BASE, pll_ptr);

	// Tap delay time, relative
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 4,
		"Tap %d delay time, relative\nTap %d time rel\nTap %d time R\nT%d time R\nT%dTR",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamTap_DLY_REL, lin_ptr);

	// Tap pitch
	lin_ptr = new param::TplLin (
		-1, 1,
		"Tap %d pitch\nT%d pitch\nT%dP",
		"%",
		index + 1,
		"%+6.2f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_ptr->use_disp_num ().set_scale (12);
	_desc_set.add_glob (base + ParamTap_PITCH, lin_ptr);

	// Tap low-cut frequency
	pll_ptr = new TplPll (
		Cst::_eq_freq_min, Cst::_eq_freq_min * 1024.0,
		"Tap %d low-cut frequency\nTap %d low-cut\nTap %d LC\nT%dLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (5);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamTap_CUT_LO, pll_ptr);

	// Tap high-cut frequency
	pll_ptr = new TplPll (
		Cst::_eq_freq_max / 1024.0, Cst::_eq_freq_max,
		"Tap %d high-cut frequency\nTap %d high-cut\nTap %d HC\nT%dHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (5);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamTap_CUT_HI, pll_ptr);

	// Tap pan
	pan_ptr = new param::TplPan (
		"Tap %d stereo panning\nTap %d stereo pan\nTap %d pan\nT%dP",
		index + 1
	);
	_desc_set.add_glob (base + ParamTap_PAN, pan_ptr);
}



void	Delay2Desc::init_line (int index)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapS <false> > TplMaps;
	typedef param::TplMapped <ParamMapFdbk> TplFdbk;

	const int      base = get_line_base (index);

	// Line input gain
	TplPll *       pll_ptr = new TplPll (
		0, 16,
		"Line %d input gain\nLine %d gain\nL%d gain\nL%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamLine_GAIN_IN, pll_ptr);

	// Line delay time, base
	pll_ptr = new TplPll (
		1.0/256, 4,
		"Line %d delay time, base\nLine %d time base\nLine %d time B\nL%d time B\nL%dTB",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		index + 1,
		"%6.1f"
	);
	pll_ptr->use_mapper ().gen_log (5);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (base + ParamLine_DLY_BASE, pll_ptr);

	// Line delay time, relative
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 4,
		"Line %d delay time, relative\nLine %d time rel\nLine %d time R\nL%d time R\nL%dTR",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_DLY_REL, lin_ptr);

	// Line speed
	param::TplLog *   log_ptr = new param::TplLog (
		0.25f, Cst::_max_bbd_speed,
		"Line %d speed\nLine %d spd\nL%d spd\nL%dS",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%6.1f"
	);
	_desc_set.add_glob (base + ParamLine_DLY_BBD_SPD, log_ptr);

	// Line pitch
	lin_ptr = new param::TplLin (
		-1, 1,
		"Line %d pitch\nL%d pitch\nL%dP",
		"%",
		index + 1,
		"%+6.2f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_ptr->use_disp_num ().set_scale (12);
	_desc_set.add_glob (base + ParamLine_PITCH, lin_ptr);

	// Line feedback
#if 1
	lin_ptr = new param::TplLin (
		0, 1,
		"Line %d feedback\nLine %d fdbk\nL%d fdbk\nL%dF",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FDBK, lin_ptr);
#else
	pll_ptr = new TplPll (
		0, 4,
		"Line %d feedback\nLine %d fdbk\nL%d fdbk\nL%dF",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%6.1f"
	);
	pll_ptr->use_mapper ().set_first_value (0);
	pll_ptr->use_mapper ().add_segment (0.75, 1.0, false);
	pll_ptr->use_mapper ().add_segment (1.0 , 4.0, true );
	_desc_set.add_glob (base + ParamLine_FDBK, pll_ptr);
#endif

	// Line low-cut frequency
	pll_ptr = new TplPll (
		Cst::_eq_freq_min, Cst::_eq_freq_min * 1024.0,
		"Line %d low-cut frequency\nLine %d low-cut\nLine %d LC\nL%dLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (5);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_CUT_LO, pll_ptr);

	// Line high-cut frequency
	pll_ptr = new TplPll (
		Cst::_eq_freq_max / 1024.0, Cst::_eq_freq_max,
		"Line %d high-cut frequency\nLine %d high-cut\nLine %d HC\nL%dHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (5);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_CUT_HI, pll_ptr);

	// Line pan
	param::TplPan *   pan_ptr = new param::TplPan (
		"Line %d stereo panning\nLine %d stereo pan\nLine %d pan\nL%dP",
		index + 1
	);
	_desc_set.add_glob (base + ParamLine_PAN, pan_ptr);

	// Line volume
	pll_ptr = new TplPll (
		0, 4,
		"Line %d volume\nLine %d volume\nLine %d vol\nL%d vol\nL%dV",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamLine_VOL, pll_ptr);

	// Line filter type
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"PK\nLP\nBP\nHP",
		"Line %d filter type\nL%d filt type\nL%d FType\nL%dFT",
		"",
		index + 1,
		"%s"
	);
	assert (enu_ptr->get_nat_max () == FilterType_NBR_ELT - 1);
	_desc_set.add_glob (base + ParamLine_FX_FLT_T, enu_ptr);

	// Line filter frequency
	log_ptr = new param::TplLog (
		20, 20480,
		"Line %d filter frequency\nLine %d filt freq\nL%d filt freq\nL%d FFreq\nL%dFF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_FX_FLT_F, log_ptr);

	// Line filter resonance
	pll_ptr = new TplPll (
		1.0 / 16, 16,
		"Line %d filter resonance\nLine %d filt reso\nL%d filt reso\nL%d FReso\nL%dFR",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().set_first_value (pll_ptr->get_nat_min ());
	pll_ptr->use_mapper ().add_segment (0.10 ,  0.125, false);
	pll_ptr->use_mapper ().add_segment (0.20 ,  0.25 , false);
	pll_ptr->use_mapper ().add_segment (0.40 ,  1.0  , false);
	pll_ptr->use_mapper ().add_segment (0.75 ,  2.0  , false);
	pll_ptr->use_mapper ().add_segment (0.875,  4.0  , false);
	pll_ptr->use_mapper ().add_segment (1.00 ,  pll_ptr->get_nat_max (), false);
	_desc_set.add_glob (base + ParamLine_FX_FLT_R, pll_ptr);

	// Line filter selectivity
	pll_ptr = new TplPll (
		0.1, 10,
		"Line %d filter selectivity\nLine %d filt Q\nL%d filt Q\nL%d FQ",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.2f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (base + ParamLine_FX_FLT_Q, pll_ptr);

	// Line distortion amount
	lin_ptr = new param::TplLin (
		0, 1,
		"Line %d distortion amount\nLine %d dist amt\nLine %d dist a\nL%d dist a\nL%dDA",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FX_DIST_A, lin_ptr);

	// Line distortion foldback
	lin_ptr = new param::TplLin (
		0, 1,
		"Line %d distortion floldback\nLine %d dist fold\nLine %d dist f\nL%d dist f\nL%dDF",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FX_DIST_F, lin_ptr);

	// Line high shelf frequency
	pll_ptr = new TplPll (
		80, 20480,
		"Line %d high shelf frequency\nLine %d shelf freq\nLine %d sh freq\nL%dSF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (8);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_FX_SHLF_F, pll_ptr);

	// Line high shelf level
	lin_ptr = new param::TplLin (
		0, 1,
		"Line %d high shelf level\nLine %d shelf level\nLine %d sh lvl\nL%dSL",
		"dB",
		index + 1,
		"%+5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB);
	_desc_set.add_glob (base + ParamLine_FX_SHLF_L, lin_ptr);

	// Line reverb mix
	TplMaps *      maps_ptr = new TplMaps (
		0, 1,
		"Line %d reverb mix\nLine %d rev mix\nL%d rev mix\nL%dRM",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_MX, maps_ptr);

	// Line reverb decay
	TplFdbk *      fdbk_ptr = new TplFdbk (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Line %d reverb decay\nLine %d rev decay\nLine %d rev dcy\nL%d rev dcy\nL%dRDc",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_DC, fdbk_ptr);

	// Line reverb damp
	fdbk_ptr = new TplFdbk (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Line %d reverb damp\nLine %d rev damp\nL%d rev dmp\nL%dRDa",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_DA, fdbk_ptr);
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
