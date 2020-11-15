/*****************************************************************************

        PhaserDesc.cpp
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
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/phase1/Cst.h"
#include "mfx/pi/phase1/Param.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/pi/phase1/StereoOut.h"
#include "mfx/pi/ParamMapFdbk.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace phase1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PhaserDesc::PhaserDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "phaser1";
	_info._name      = "Phaser HT";
	_info._tag_list  = { piapi::Tag::_modulation_0 };
	_info._chn_pref  = piapi::ChnPref::STEREO;

	typedef param::TplMapped <ParamMapFdbk> TplFdbk;
	typedef param::TplMapped <param::MapS <false> > TplMaps;

	// Speed
	auto           log_sptr = std::make_shared <param::TplLog> (
		0.01, 100,
		"S\nSpd\nSpeed",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_SPEED, log_sptr);

	// Depth
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"4\n8\n16\n32",
		"D\nDpt\nDepth",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == Cst::_nbr_phase_filters - 1);
	_desc_set.add_glob (Param_DEPTH, enu_sptr);

	// Feedback Level
	auto           fbi_sptr = std::make_shared <TplFdbk> (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"F\nFdbk\nFdbk Lvl\nFdbk Level\nFeedback Level",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_FDBK_LEVEL, fbi_sptr);

	// Feedback Color
	auto           lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"C\nCol\nFdbk Col\nFdbk Color\nFeedback Color",
		"%",
		0,
		"%3.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_FDBK_COLOR, lin_sptr);

	// Phase Mix
	auto           maps_sptr = std::make_shared <TplMaps> (
		0, 1,
		"M\nMix\nPh Mix\nPhase Mix",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_PHASE_MIX, maps_sptr);

	// Phase Offset
	lin_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		"PO\nPh.Ofs\nOffset\nPhase Offset",
		"%",
		0,
		"%+5.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_MANUAL, lin_sptr);

	// Phase Set
	lin_sptr = std::make_shared <param::TplLin> (
		0, 1,
		"P\nPh.Set\nPhase Set",
		"\xC2\xB0", // U+00B0 DEGREE SIGN
		0,
		"%3.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_sptr);

	// Hold
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"H\nHold",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_HOLD, enu_sptr);

	// Band-Pass Filter Cutoff Frequency
	log_sptr = std::make_shared <param::TplLog> (
		40, 10240,
		"F\nFreq\nBPF F\nBPF Freq\nBand-Pass Filter Cutoff Frequency",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_BPF_CUTOFF, log_sptr);

	// Band-Pass Filter Selectivity
	log_sptr = std::make_shared <param::TplLog> (
		0.1, 10,
		"Q\nBPF Q\nBPF Selectivity\nBand-Pass Filter Selectivity",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.1f"
	);
	_desc_set.add_glob (Param_BPF_Q, log_sptr);

	// Direction
	enu_sptr = std::make_shared <param::TplEnum> (
		"Down\nUp",
		"Direction\nDir\nD",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DIR, enu_sptr);

	// Mono phase mix
	enu_sptr = std::make_shared <param::TplEnum> (
		"Left\nMixed",
		"Mono phase mix\nMono mix\nMono\nM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_OP_MONO, enu_sptr);
	
	// Stereo phase mix
	enu_sptr = std::make_shared <param::TplEnum> (
		"Spat mix\nSpat sep\nBi-mono",
		"Stereo phase mix\nStereo mix\nStereo\nS",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == StereoOut_NBR_ELT - 1);
	_desc_set.add_glob (Param_OP_STEREO, enu_sptr);

	// All-pass delay
	lin_sptr = std::make_shared <param::TplLin> (
		0, Cst::_max_apf_delay_time,
		"All-pass delay\nAllP delay\nAP delay\nAP dly\nD",
		"us",
		0,
		"%4.0f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_MICRO
	);
	_desc_set.add_glob (Param_AP_DELAY, lin_sptr);

	// All-pass coefficient
	lin_sptr = std::make_shared <param::TplLin> (
		-0.5, 0.5,
		"All-pass coefficient\nAll-pass coef\nAllP coef\nAP coef\nC",
		"",
		0,
		"%+5.2f"
	);
	_desc_set.add_glob (Param_AP_COEF, lin_sptr);
}



ParamDescSet &	PhaserDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	PhaserDesc::do_get_info () const
{
	return _info;
}



void	PhaserDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	PhaserDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	PhaserDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
