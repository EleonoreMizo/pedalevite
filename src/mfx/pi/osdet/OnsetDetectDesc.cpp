/*****************************************************************************

        OnsetDetectDesc.cpp
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

#include "mfx/pi/osdet/OnsetDetectDesc.h"
#include "mfx/pi/osdet/Param.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace osdet
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



OnsetDetectDesc::OnsetDetectDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "osdet";
	_info._name      = "Onset detector\nOnset det\nOnset";
	_info._tag_list  = { piapi::Tag::_analyser_0, piapi::Tag::_control_gen_0 };
	_info._chn_pref  = piapi::ChnPref::NONE;

	// Velocity clipping
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Velocity clipping",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_VELO_CLIP, enu_sptr);

	// Attack threshold
	auto           log_sptr = std::make_shared <param::TplLog> (
		1e-4, 1e-1,
		"Attack threshold\nAttack thr\nAtk thr\nAT",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_ATK_THR, log_sptr);

	// Attack ratio
	log_sptr = std::make_shared <param::TplLog> (
		1, 8,
		"Attack ratio\nAtk ratio\nAtk R\nAR",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%3.0f"
	);
	_desc_set.add_glob (Param_ATK_RATIO, log_sptr);

	// Release threshold
	log_sptr = std::make_shared <param::TplLog> (
		1e-4, 1e-1,
		"Release threshold\nRelease thr\nRls thr\nRT",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_RLS_THR, log_sptr);

	// Release ratio
	log_sptr = std::make_shared <param::TplLog> (
		1, 8,
		"Release ratio\nRls ratio\nRls R\nRR",
		"%",
		param::HelperDispNum::Preset_FLOAT_PERCENT,
		0,
		"%3.0f"
	);
	_desc_set.add_glob (Param_RLS_RATIO, log_sptr);

}



ParamDescSet &	OnsetDetectDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	OnsetDetectDesc::do_get_info () const
{
	return _info;
}



void	OnsetDetectDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 0;
	nbr_s = 2;
}



int	OnsetDetectDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	OnsetDetectDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
