/*****************************************************************************

        TestGenDesc.cpp
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

#include "mfx/pi/testgen/LoopPause.h"
#include "mfx/pi/testgen/Param.h"
#include "mfx/pi/testgen/SweepDur.h"
#include "mfx/pi/testgen/TestGenDesc.h"
#include "mfx/pi/testgen/ToneFreq.h"
#include "mfx/pi/testgen/Type.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace testgen
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TestGenDesc::TestGenDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "testgen";
	_info._name      = "Test generator\nTest gen";
	_info._tag_list  = { piapi::Tag::_utility_0, piapi::Tag::_synth_0 };

	// State
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"State\nSt",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_STATE, enu_sptr);

	// Type
	enu_sptr = std::make_shared <param::TplEnum> (
		"Pink noise\nWhite noise\nTone\nSweep\nPulse",
		"Type\nTy",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == Type_NBR_ELT - 1);
	_desc_set.add_glob (Param_TYPE, enu_sptr);

	// Level
	auto           log_sptr = std::make_shared <param::TplLog> (
		1e-5, 1,
		"Level\nLv",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+6.1f"
	);
	_desc_set.add_glob (Param_LVL, log_sptr);

	// Noise correlation
	enu_sptr = std::make_shared <param::TplEnum> (
		"Mono\nMulti",
		"Noise correlation\nNoise correl\nNz cor\nNC",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_NZ_COR, enu_sptr);

	// Tone frequency
	enu_sptr = std::make_shared <param::TplEnum> (
		"30\n40\n50\n100\n500\n1k\n3k\n10k\n15k",
		"Tone frequency\nTone freq\nTF",
		"Hz",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == ToneFreq_NBR_ELT - 1);
	_desc_set.add_glob (Param_TONE_FRQ, enu_sptr);

	// Sweep duration
	enu_sptr = std::make_shared <param::TplEnum> (
		"6\n15\n30\n60\n180",
		"Sweep duration\nSweep dur\nSD",
		"s",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == SweepDur_NBR_ELT - 1);
	_desc_set.add_glob (Param_SWEEP_DUR, enu_sptr);

	// Pause
	enu_sptr = std::make_shared <param::TplEnum> (
		"No loop\n1\n3\n10\n30",
		"Pause\nPa",
		"s",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == LoopPause_NBR_ELT - 1);
	_desc_set.add_glob (Param_LOOP_PAUSE, enu_sptr);
}



ParamDescSet &	TestGenDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	TestGenDesc::do_get_info () const
{
	return _info;
}



void	TestGenDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	TestGenDesc::do_prefer_stereo () const
{
	return true;
}



int	TestGenDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	TestGenDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace testgen
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
