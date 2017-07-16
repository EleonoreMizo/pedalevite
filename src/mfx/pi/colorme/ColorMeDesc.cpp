/*****************************************************************************

        ColorMeDesc.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/pi/colorme/ColorMeDesc.h"
#include "mfx/pi/colorme/Param.h"
#include "mfx/pi/param/Simple.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplInt.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace colorme
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ColorMeDesc::ColorMeDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	// Vowel morphing
	param::Simple *   sim_ptr = new param::Simple (
		"Vowel morphing\nMorphing\nMorph\nMor"
	);
	sim_ptr->set_flags (piapi::ParamDescInterface::Flags_AUTOLINK);
	_desc_set.add_glob (Param_VOW_MORPH, sim_ptr);

	// Formant resonance
	param::TplLog *   log_ptr = new param::TplLog (
		1.0, 100.0,
		"Formant resonance\nFormant reso\nReso\nRes",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_VOW_RESO, log_ptr);

	// Formant selectivity
	log_ptr = new param::TplLog (
		1.0, 100.0,
		"Formant selectivity\nFormant Q\nQ",
		"",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_VOW_Q, log_ptr);

	// Formant transpose
	param::TplLin *   lin_ptr = new param::TplLin (
		-12.0, 12.0,
		"Formant transpose\nFormant transp\nTranspose\nTransp\nTrn",
		"semitones",
		0,
		"%+5.1f"
	);
	lin_ptr->use_disp_num ().set_preset (param::HelperDispNum::Preset_FLOAT_STD);
	_desc_set.add_glob (Param_VOW_TRANSP, lin_ptr);

	// Number of formants
	param::TplInt *   int_ptr = new param::TplInt (
		2, Cst::_nbr_formants,
		"Number of formants\nN formants\nFormants\nFrm",
		""
	);
	_desc_set.add_glob (Param_VOW_NBR_FORM, int_ptr);

	// Number of vowels
	int_ptr = new param::TplInt (
		2, Cst::_nbr_vow_morph,
		"Number of vowels\nN vowels\nVowels\nVow",
		""
	);
	_desc_set.add_glob (Param_VOW_NBR_VOW, int_ptr);

	// Vowels
	for (int index = 0; index < Cst::_nbr_vow_morph; ++index)
	{
		const int      base = Param_VOW_LIST + index * ParamVowel_NBR_ELT;

		// Vowel N type
		param::TplEnum *   enu_ptr = new param::TplEnum (
			"i\n\xC3\xA9\n\xC3\xA8\na\nu\neu\n\xC5\x93\nou\nau\no",
			"Vowel %d type\nV %d type\nV%dT",
			"",
			index + 1,
			"%s"
		);
		assert (enu_ptr->get_nat_max () == Cst::_nbr_vowels - 1);
		_desc_set.add_glob (base + ParamVowel_TYPE, enu_ptr);

		// Vowel N set
		enu_ptr = new param::TplEnum (
			"Call\nGD\nG&A",
			"Vowel %d set\nV %d set\nV%dS",
			"",
			index + 1,
			"%s"
		);
		assert (enu_ptr->get_nat_max () == Cst::_nbr_vow_sets - 1);
		_desc_set.add_glob (base + ParamVowel_SET, enu_ptr);
	}
}



ParamDescSet &	ColorMeDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	ColorMeDesc::do_get_unique_id () const
{
	return "colorme";
}



std::string	ColorMeDesc::do_get_name () const
{
	return "Color Me\nColorMe\nColMe";
}



void	ColorMeDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	ColorMeDesc::do_prefer_stereo () const
{
	return false;
}



int	ColorMeDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	ColorMeDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace colorme
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
