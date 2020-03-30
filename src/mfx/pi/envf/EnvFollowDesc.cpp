/*****************************************************************************

        EnvFollowDesc.cpp
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

#include "fstb/def.h"
#include "mfx/pi/envf/Cst.h"
#include "mfx/pi/envf/Param.h"
#include "mfx/pi/envf/EnvFollowDesc.h"
#include "mfx/pi/param/MapPseudoLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/piapi/Tag.h"

#include <array>

#include <cassert>



namespace mfx
{
namespace pi
{
namespace envf
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



EnvFollowDesc::EnvFollowDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "envf";
	_info._name      = "Envelope Follower\nEnvelope Fol\nEnv Fol";
	_info._tag_list  = { piapi::Tag::_analyser_0, piapi::Tag::_control_gen_0 };

	typedef param::TplMapped <param::MapPseudoLog> TplPsl;

	class ParamInfo
	{
	public:
		Param          _p;
		const char *   _name_0;
	};
	static const std::array <ParamInfo, 3> info_list =
	{{
		{ Param_ATTACK , "Attack Time\nAttack\nAtk\nA"   },
		{ Param_RELEASE, "Release Time\nRelease\nRls\nR" },
		{ Param_HOLD   , "Hold Time\nHold\nH"            }
	}};
	for (size_t cnt = 0; cnt < info_list.size (); ++ cnt)
	{
		auto           pl_sptr = std::make_shared <TplPsl> (
			0, 100,
			info_list [cnt]._name_0,
			"ms",
			param::HelperDispNum::Preset_FLOAT_MILLI,
			0,
			"%8.1f"
		);
		pl_sptr->use_mapper ().set_curvature (4 * fstb::LN10);
		_desc_set.add_glob (info_list [cnt]._p, pl_sptr);
	}

	// Gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		1, 100,
		"Gain\nG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_sptr);

	// Threshold
	auto           pl_sptr = std::make_shared <TplPsl> (
		0, 10,
		"Threshold\nThresh\nThr\nT",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_THRESH, pl_sptr);

	// Mode
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Linear\nLog",
		"Mode\nM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_MODE, enu_sptr);

	// Clip envelope
	enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Clip envelope\nClip env\nClip E\nClpE",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_CLIP_E, enu_sptr);

	// Clip source
	log_sptr = std::make_shared <param::TplLog> (
		0.01, Cst::_clip_max,
		"Clip source\nClip src\nClip S\nClpS",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_CLIP_S, log_sptr);

	// Low-cut frequency
	log_sptr = std::make_shared <param::TplLog> (
		Cst::_lc_freq_min, Cst::_lc_freq_min * 1024,
		"Low-cut frequency\nLow-cut freq\nLC freq\nLCF",
		"dB",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%6.1f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_LC_FREQ, log_sptr);
}



ParamDescSet &	EnvFollowDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	EnvFollowDesc::do_get_info () const
{
	return _info;
}



void	EnvFollowDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 0;
	nbr_s = 1;
}



bool	EnvFollowDesc::do_prefer_stereo () const
{
	return false;
}



int	EnvFollowDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	EnvFollowDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace envf
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
