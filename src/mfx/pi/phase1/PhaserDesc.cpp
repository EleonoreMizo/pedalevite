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
#include "mfx/pi/ParamMapFdbk.h"

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
{
	typedef param::TplMapped <ParamMapFdbk> TplFdbk;
	typedef param::TplMapped <param::MapS <false> > TplMaps;

	// Speed
	param::TplLog *   log_ptr = new param::TplLog (
		0.01, 100,
		"S\nSpd\nSpeed",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.3f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_SPEED, log_ptr);

	// Depth
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"4\n8\n16\n32",
		"D\nDpt\nDepth",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_DEPTH, enu_ptr);
	assert (enu_ptr->get_nat_max () == Cst::_nbr_phase_filters - 1);

	// Feedback Level
	TplFdbk *      fbi_ptr = new TplFdbk (
		TplFdbk::Mapper::get_nat_min (),
		TplFdbk::Mapper::get_nat_max (),
		"F\nFdbk\nFdbk Lvl\nFdbk Level\nFeedback Level",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_FDBK_LEVEL, fbi_ptr);

	// Feedback Color
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 1,
		"C\nCol\nFdbk Col\nFdbk Color\nFeedback Color",
		"%",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_ptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_FDBK_COLOR, lin_ptr);

	// Phase Mix
	TplMaps *      maps_ptr = new TplMaps (
		0, 1,
		"M\nMix\nPh Mix\nPhase Mix",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%5.1f"
	);
	maps_ptr->use_mapper ().config (
		maps_ptr->get_nat_min (),
		maps_ptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_PHASE_MIX, maps_ptr);

	// Phase Offset
	lin_ptr = new param::TplLin (
		-1, 1,
		"PO\nPh.Ofs\nOffset\nPhase Offset",
		"%",
		0,
		"%+5.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_ptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_MANUAL, lin_ptr);

	// Phase Set
	lin_ptr = new param::TplLin (
		0, 1,
		"P\nPh.Set\nPhase Set",
		"deg",
		0,
		"%3.0f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	lin_ptr->use_disp_num ().set_scale (360);
	_desc_set.add_glob (Param_PHASE_SET, lin_ptr);

	// Hold
	enu_ptr = new param::TplEnum (
		"Off\nOn",
		"H\nHold",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_HOLD, enu_ptr);

	// Band-Pass Filter Cutoff Frequency
	log_ptr = new param::TplLog (
		40, 10240,
		"F\nFreq\nBPF F\nBPF Freq\nBand-Pass Filter Cutoff Frequency",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_BPF_CUTOFF, log_ptr);

	// Band-Pass Filter Selectivity
	log_ptr = new param::TplLog (
		0.1, 10,
		"Q\nBPF Q\nBPF Selectivity\nBand-Pass Filter Selectivity",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.1f"
	);
	_desc_set.add_glob (Param_BPF_Q, log_ptr);
}



ParamDescSet &	PhaserDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	PhaserDesc::do_get_unique_id () const
{
	return "phaser1";
}



std::string	PhaserDesc::do_get_name () const
{
	return "Phaser";
}



void	PhaserDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	PhaserDesc::do_prefer_stereo () const
{
	return true;
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
