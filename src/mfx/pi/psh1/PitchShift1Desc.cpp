/*****************************************************************************

        PitchShift1Desc.cpp
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

#include "mfx/pi/psh1/Cst.h"
#include "mfx/pi/psh1/PitchShift1Desc.h"
#include "mfx/pi/psh1/Param.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace psh1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PitchShift1Desc::PitchShift1Desc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "psh1";
	_info._name      = "Pitch shifter\nPitch sh";
	_info._tag_list  = { piapi::Tag::_pitch_shift_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Pitch
	auto           lin_sptr = std::make_shared <param::TplLin> (
		Cst::_max_pitch_dw, Cst::_max_pitch_up,
		"Pitch\nPtch\nP",
		"%",
		0,
		"%+6.2f"
	);
	lin_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_STD
	);
	lin_sptr->use_disp_num ().set_scale (12);
	_desc_set.add_glob (Param_PITCH, lin_sptr);

	// Window duration
	auto           log_sptr = std::make_shared <param::TplLog> (
		Cst::_max_win_size / (1000.0 * 32), Cst::_max_win_size / 1000.0,
		"Window duration\nWin duration\nWin dur\nW dur\nWD",
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_WIN_DUR, log_sptr);
}



ParamDescSet &	PitchShift1Desc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	PitchShift1Desc::do_get_info () const
{
	return _info;
}



void	PitchShift1Desc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



int	PitchShift1Desc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	PitchShift1Desc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace psh1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
