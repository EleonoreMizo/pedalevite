/*****************************************************************************

        HyperCombDesc.cpp
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

#include "mfx/pi/hcomb/Cst.h"
#include "mfx/pi/hcomb/HyperCombDesc.h"
#include "mfx/pi/hcomb/Param.h"
#include "mfx/pi/param/HelperDispNum.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace hcomb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



HyperCombDesc::HyperCombDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Tune
	param::TplLin *   lin_ptr = new param::TplLin (
		-12, 12,
		"Tune",
		"semitones",
		0,
		"%+6.2f"
	);
	_desc_set.add_glob (Param_TUNE, lin_ptr);

	// Duration
	TplPll *       pll_ptr = new TplPll (
		0.01, 100,
		"Duration\nDur",
		"s",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%7.2f"
	);
	pll_ptr->use_mapper ().gen_log (8);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_TIME_S);
	_desc_set.add_glob (Param_TIME, pll_ptr);

	// Volume
	pll_ptr = new TplPll (
		0.01, 1,
		"Volume\nVol",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (Param_VOLUME, pll_ptr);

	// Filter frequency
	pll_ptr = new TplPll (
		20, 20480,
		"Filter frequency\nFilter freq\nFlt freq\nFFreq\nFreq\nFF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	pll_ptr->use_mapper ().gen_log (10);
	pll_ptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_FLT_FREQ, pll_ptr);

	// Filter resonance
	param::Simple *   sim_ptr = new param::Simple (
		"Filter resonance\nFilter reso\nflt reso\nFReso\nReso\nFR"
	);
	_desc_set.add_glob (Param_FLT_RESO, sim_ptr);

	// Filter damping
	sim_ptr = new param::Simple (
		"Filter damping\nDamping\nFDamp\nDamp\nFD"
	);
	_desc_set.add_glob (Param_FLT_DAMP, sim_ptr);

	// Voices
	for (int index = 0; index < Cst::_nbr_voices; ++index)
	{
		add_voice (index);
	}
}



ParamDescSet &	HyperCombDesc::use_desc_set ()
{
	return _desc_set;
}



int	HyperCombDesc::get_base_voice (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_voices);

	return Param_BASE_VOICE + ParamVoice_NBR_ELT * index;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	HyperCombDesc::do_get_unique_id () const
{
	return "hypercomb";
}



std::string	HyperCombDesc::do_get_name () const
{
	return "Hypercomb\nHComb";
}



void	HyperCombDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	HyperCombDesc::do_prefer_stereo () const
{
	return false;
}



int	HyperCombDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	HyperCombDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	HyperCombDesc::add_voice (int index)
{
	assert (index >= 0);
	assert (index < Cst::_nbr_voices);

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	const int      base = get_base_voice (index);

	// Polarity
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Off\nPos\nNeg",
		"Voice %d polarity\nV%d polarity\nV%d pol\nV%dP",
		"",
		index,
		"%s"
	);
	_desc_set.add_glob (base + ParamVoice_POLARITY, enu_ptr);

	// Note
	param::TplInt *   int_ptr = new param::TplInt (
		12, 92,
		"Voice %d note\nV%d note\nV%dN",
		"",
		index,
		"%.0f"
	);
	int_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_MIDI_NOTE);
	_desc_set.add_glob (base + ParamVoice_NOTE, int_ptr);

	// Finetune
	param::TplLin *   lin_ptr = new param::TplLin (
		-1, +1,
		"Voice %d finetune\nV%d finetune\nV%d ftune\nV%dFT",
		"cents",
		index,
		"%.0f"
	);
	lin_ptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (base + ParamVoice_FINETUNE, lin_ptr);

	// Duration multiplier
	TplPll *       pll_ptr = new TplPll (
		0.1, 10,
		"Voice %d duration multiplier\nVoice %d dur mult\nVoice %d dur\nV%d dur\nV%dD",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		index,
		"%6.3f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (base + ParamVoice_TIME_MULT, pll_ptr);

	// Gain
	pll_ptr = new TplPll (
		0.1, 10,
		"Voice %d gain\nV%d gain\nV%dG",
		"dB",
		param::HelperDispNum::Preset_DB,
		index,
		"%+5.1f"
	);
	pll_ptr->use_mapper ().gen_log (4);
	_desc_set.add_glob (base + ParamVoice_GAIN, pll_ptr);
}



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
