/*****************************************************************************

        SpeakerEmuDesc.cpp
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

#include "mfx/pi/spkem/SpeakerEmuDesc.h"
#include "mfx/pi/spkem/Param.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplLin.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace spkem
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



SpeakerEmuDesc::SpeakerEmuDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Mid level
	param::TplLog *   log_ptr = new param::TplLog (
		1, 4,
		"Mid level\nMid lvl\nM Lvl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_MID_LVL, log_ptr);

	// Mid Frequency
	log_ptr = new param::TplLog (
		1500, 3000,
		"Mid frequency\nMid freq\nM freq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_MID_FREQ, log_ptr);

	// Treble level
	log_ptr = new param::TplLog (
		0.001, 0.1,
		"Treble level\nTreble lvl\nT Lvl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_TREBLE_LVL, log_ptr);

	// Treble Frequency
	log_ptr = new param::TplLog (
		4000, 8000,
		"Treble frequency\nTreble freq\nT freq",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%4.0f"
	);
	log_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_TREBLE_FREQ, log_ptr);

	// Comb level
	param::TplLin *   lin_ptr = new param::TplLin (
		0, 0.5,
		"Comb level\nComb lvl\nC Lvl",
		"%",
		0,
		"%4.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_PERCENT);
	_desc_set.add_glob (Param_COMB_LVL, lin_ptr);
}



ParamDescSet &	SpeakerEmuDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	SpeakerEmuDesc::do_get_unique_id () const
{
	return "spkemu1";
}



std::string	SpeakerEmuDesc::do_get_name () const
{
	return "Speaker emulator\nSpeaker emu\nSpk emu";
}



void	SpeakerEmuDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	SpeakerEmuDesc::do_prefer_stereo () const
{
	return false;
}



int	SpeakerEmuDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	SpeakerEmuDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace spkem
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
