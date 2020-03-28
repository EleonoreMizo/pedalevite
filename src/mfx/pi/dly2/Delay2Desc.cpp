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
,	_info ()
{
	_info._unique_id = "dly2";
	_info._name      = "SiemensGirlz\nSiemGirlz\nSGirlz";

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapS <false> > TplS;

	// Tap gain in
	auto           sim_sptr = std::make_shared <param::Simple> (
		"Tap gain in\nTap G in\nTap in\nTGI"
	);
	_desc_set.add_glob (ParamLine_GAIN_IN, sim_sptr);

	// Tap global volume
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Tap global volume\nTap glob vol\nTap vol\nTapV\nTV",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (Param_TAPS_VOL, pll_sptr);

	// Dry volume
	pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Dry volume\nDry vol\nDryV\nDV",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (Param_DRY_VOL, pll_sptr);

	// Dry spread
	auto           maps_sptr = std::make_shared <TplS> (
		-1, 1,
		"Dry spread\nDry Spr\nDSpr\nDS",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%+6.1f"
	);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_DRY_SPREAD, maps_sptr);

	// Freeze lines
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Freeze lines\nFreeze\nFrz",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_FREEZE, enu_sptr);

	// Number of feedback lines
	auto           int_sptr = std::make_shared <param::TplInt> (
		0, int (Cst::_nbr_lines),
		"Number of feedback lines\nNb of feedback lines\nFeedback lines"
		"\nFdbk lines\nNLines\nNL",
		"",
		0,
		"%.0f"
	);
	_desc_set.add_glob (Param_NBR_LINES, int_sptr);

	// Cross-freedback
	maps_sptr = std::make_shared <TplS> (
			0, 1,
			"Ping-pong feedback\nPing-pong\nPiPo\nPP",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_X_FDBK, maps_sptr);

	// Ducking sensitivity
	pll_sptr = std::make_shared <TplPll> (
		1.0/1024, 1,
		"Ducking sensitivity\nDuck sensitivity\nDuck sens\nDuck S\nDS",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (10, 2);
	_desc_set.add_glob (Param_DUCK_SENS, pll_sptr);

	// Ducking time
	pll_sptr = std::make_shared <TplPll> (
		1.0/256, 1,
		"Ducking time\nDuck time\nDuck T\nDT",
		"dB",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (Param_DUCK_TIME, pll_sptr);

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



piapi::PluginInfo	Delay2Desc::do_get_info () const
{
	return _info;
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
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Tap %d input gain\nTap %d gain\nT%d gain\nT%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamTap_GAIN_IN, pll_sptr);

	// Tap spread
	auto           pan_sptr = std::make_shared <param::TplPan> (
		"Tap %d spread\nTap %d spr\nT%d spr\nT%dS",
		index + 1
	);
	_desc_set.add_glob (base + ParamTap_SPREAD, pan_sptr);

	// Tap delay time, base
	pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Tap %d delay time, base\nTap %d time base\nTap %d time B\nT%d time B\nT%dTB",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		index + 1,
		"%6.1f"
	);
	pll_sptr->use_mapper ().gen_log (5, 2);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (base + ParamTap_DLY_BASE, pll_sptr);

	// Tap delay time, relative
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 4,
		"Tap %d delay time, relative\nTap %d time rel\nTap %d time R\nT%d time R\nT%dTR",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamTap_DLY_REL, lin_sptr);

	// Tap pitch
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Tap %d pitch\nT%d pitch\nT%dP",
		"%",
		index + 1,
		"%+6.2f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (12);
	_desc_set.add_glob (base + ParamTap_PITCH, lin_sptr);

	// Tap low-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		double (Cst::_eq_freq_min), double (Cst::_eq_freq_min) * 1024,
		"Tap %d low-cut frequency\nTap %d low-cut\nTap %d LC\nT%dLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (5);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamTap_CUT_LO, pll_sptr);

	// Tap high-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		double (Cst::_eq_freq_max) / 1024, double (Cst::_eq_freq_max),
		"Tap %d high-cut frequency\nTap %d high-cut\nTap %d HC\nT%dHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (5);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamTap_CUT_HI, pll_sptr);

	// Tap pan
	pan_sptr = std::make_shared <param::TplPan> (
		"Tap %d stereo panning\nTap %d stereo pan\nTap %d pan\nT%dP",
		index + 1
	);
	_desc_set.add_glob (base + ParamTap_PAN, pan_sptr);
}



void	Delay2Desc::init_line (int index)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;
	typedef param::TplMapped <param::MapS <false> > TplMaps;
	typedef param::TplMapped <ParamMapFdbk> TplFdbk;

	const int      base = get_line_base (index);

	// Line input gain
	auto           pll_sptr = std::make_shared <TplPll> (
		0, 16,
		"Line %d input gain\nLine %d gain\nL%d gain\nL%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamLine_GAIN_IN, pll_sptr);

	// Line delay time, base
	pll_sptr = std::make_shared <TplPll> (
		1.0/256, 4,
		"Line %d delay time, base\nLine %d time base\nLine %d time B\nL%d time B\nL%dTB",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		index + 1,
		"%6.1f"
	);
	pll_sptr->use_mapper ().gen_log (5);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (base + ParamLine_DLY_BASE, pll_sptr);

	// Line delay time, relative
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 4,
		"Line %d delay time, relative\nLine %d time rel\nLine %d time R\nL%d time R\nL%dTR",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_DLY_REL, lin_sptr);

	// Line speed
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.25f, double (Cst::_max_bbd_speed),
		"Line %d speed\nLine %d spd\nL%d spd\nL%dS",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%6.1f"
	);
	_desc_set.add_glob (base + ParamLine_DLY_BBD_SPD, log_sptr);

	// Line pitch
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"Line %d pitch\nL%d pitch\nL%dP",
		"%",
		index + 1,
		"%+6.2f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (12);
	_desc_set.add_glob (base + ParamLine_PITCH, lin_sptr);

	// Line feedback
#if 1
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d feedback\nLine %d fdbk\nL%d fdbk\nL%dF",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FDBK, lin_sptr);
#else
	pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Line %d feedback\nLine %d fdbk\nL%d fdbk\nL%dF",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%6.1f"
	);
	pll_sptr->use_mapper ().set_first_value (0);
	pll_sptr->use_mapper ().add_segment (0.75, 1.0, false);
	pll_sptr->use_mapper ().add_segment (1.0 , 4.0, true );
	_desc_set.add_glob (base + ParamLine_FDBK, pll_sptr);
#endif

	// Line low-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		double (Cst::_eq_freq_min), double (Cst::_eq_freq_min) * 1024,
		"Line %d low-cut frequency\nLine %d low-cut\nLine %d LC\nL%dLC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (5);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_CUT_LO, pll_sptr);

	// Line high-cut frequency
	pll_sptr = std::make_shared <TplPll> (
		double (Cst::_eq_freq_max) / 1024, double (Cst::_eq_freq_max),
		"Line %d high-cut frequency\nLine %d high-cut\nLine %d HC\nL%dHC",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (5);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_CUT_HI, pll_sptr);

	// Line pan
	auto           pan_sptr = std::make_shared <param::TplPan> (
		"Line %d stereo panning\nLine %d stereo pan\nLine %d pan\nL%dP",
		index + 1
	);
	_desc_set.add_glob (base + ParamLine_PAN, pan_sptr);

	// Line volume
	pll_sptr = std::make_shared <TplPll> (
		0, 4,
		"Line %d volume\nLine %d volume\nLine %d vol\nL%d vol\nL%dV",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8, 2);
	_desc_set.add_glob (base + ParamLine_VOL, pll_sptr);

	// Line ducking amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d ducking amount\nLine %d duck amount\nLine %d duck amt\nL%d duck\nL%dDK",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_DUCK_AMT, lin_sptr);

	// Line filter type
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"PK\nLP\nBP\nHP",
		"Line %d filter type\nL%d filt type\nL%d FType\nL%dFT",
		"",
		index + 1,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == FilterType_NBR_ELT - 1);
	_desc_set.add_glob (base + ParamLine_FX_FLT_T, enu_sptr);

	// Line filter frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Line %d filter frequency\nLine %d filt freq\nL%d filt freq\nL%d FFreq\nL%dFF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_FX_FLT_F, log_sptr);

	// Line filter resonance
	pll_sptr = std::make_shared <TplPll> (
		1.0 / 16, 16,
		"Line %d filter resonance\nLine %d filt reso\nL%d filt reso\nL%d FReso\nL%dFR",
		"dB",
		param::HelperDispNum::Preset_DB,
		index + 1,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().set_first_value (pll_sptr->get_nat_min ());
	pll_sptr->use_mapper ().add_segment (0.10 ,  0.125, false);
	pll_sptr->use_mapper ().add_segment (0.20 ,  0.25 , false);
	pll_sptr->use_mapper ().add_segment (0.40 ,  1.0  , false);
	pll_sptr->use_mapper ().add_segment (0.75 ,  2.0  , false);
	pll_sptr->use_mapper ().add_segment (0.875,  4.0  , false);
	pll_sptr->use_mapper ().add_segment (1.00 ,  pll_sptr->get_nat_max (), false);
	_desc_set.add_glob (base + ParamLine_FX_FLT_R, pll_sptr);

	// Line filter selectivity
	pll_sptr = std::make_shared <TplPll> (
		0.1, 10,
		"Line %d filter selectivity\nLine %d filt Q\nL%d filt Q\nL%d FQ",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.2f"
	);
	pll_sptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (base + ParamLine_FX_FLT_Q, pll_sptr);

	// Line filter mix
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d filter mix\nLine %d filt mix\nL%d filt mix\nL%d FM",
		"dB",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FX_FLT_M, lin_sptr);

	// Line distortion amount
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d distortion amount\nLine %d dist amt\nLine %d dist a"
		"\nL%d dist a\nL%dDA",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FX_DIST_A, lin_sptr);

	// Line distortion foldback
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d distortion floldback\nLine %d dist fold\nLine %d dist f"
		"\nL%d dist f\nL%dDF",
		"%",
		index + 1,
		"%5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamLine_FX_DIST_F, lin_sptr);

	// Line high shelf frequency
	pll_sptr = std::make_shared <TplPll> (
		80, 20480,
		"Line %d high shelf frequency\nLine %d shelf freq\nLine %d sh freq\nL%dSF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%5.0f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_FX_SHLF_F, pll_sptr);

	// Line high shelf level
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"Line %d high shelf level\nLine %d shelf level\nLine %d sh lvl\nL%dSL",
		"dB",
		index + 1,
		"%+5.1f"
	);
	lin_sptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_DB);
	_desc_set.add_glob (base + ParamLine_FX_SHLF_L, lin_sptr);

	// Line frequency shifting
	pll_sptr = std::make_shared <TplPll> (
		double (-Cst::_max_freq_shift), double (Cst::_max_freq_shift),
		"Line %d frequency shift\nLine %d freq shift\nLine %d f shift\nL%d f shift\nL%dFS",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		index + 1,
		"%+7.1f"
	);
	pll_sptr->use_mapper ().set_first_value (-Cst::_max_freq_shift);
	pll_sptr->use_mapper ().add_segment (0.4, -0.004 * Cst::_max_freq_shift, true);
	pll_sptr->use_mapper ().add_segment (0.6,  0.004 * Cst::_max_freq_shift, false);
	pll_sptr->use_mapper ().add_segment (1.0,          Cst::_max_freq_shift, true);
	pll_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (base + ParamLine_FX_FSH_F, pll_sptr);

	// Line reverb mix
	auto           maps_sptr = std::make_shared <TplMaps> (
		0, 1,
		"Line %d reverb mix\nLine %d rev mix\nL%d rev mix\nL%dRM",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_MX, maps_sptr);

	// Line reverb decay
	auto           fdbk_sptr = std::make_shared <TplFdbk> (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Line %d reverb decay\nLine %d rev decay\nLine %d rev dcy\nL%d rev dcy\nL%dRDc",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_DC, fdbk_sptr);

	// Line reverb damp
	fdbk_sptr = std::make_shared <TplFdbk> (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"Line %d reverb damp\nLine %d rev damp\nL%d rev dmp\nL%dRDa",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		index + 1,
		"%5.1f"
	);
	_desc_set.add_glob (base + ParamLine_FX_REV_DA, fdbk_sptr);
}



}  // namespace dly2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
