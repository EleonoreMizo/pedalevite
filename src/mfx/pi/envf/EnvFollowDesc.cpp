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
#include "mfx/pi/envf/Param.h"
#include "mfx/pi/envf/EnvFollowDesc.h"
#include "mfx/pi/param/MapPseudoLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"

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
{
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
		TplPsl *       pl_ptr = new TplPsl (
			0, 100,
			info_list [cnt]._name_0,
			"ms",
			param::HelperDispNum::Preset_FLOAT_MILLI,
			0,
			"%8.1f"
		);
		pl_ptr->use_mapper ().set_curvature (4 * fstb::LN10);
		_desc_set.add_glob (info_list [cnt]._p, pl_ptr);
	}

	// Gain
	param::TplLog *   log_ptr = new param::TplLog (
		1, 100,
		"Gain\nG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_ptr);

	// Threshold
	TplPsl *       pl_ptr = new TplPsl (
		0, 10,
		"Threshold\nThresh\nThr\nT",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%5.1f"
	);
	_desc_set.add_glob (Param_THRESH, pl_ptr);

	// Mode
	param::TplEnum *  enu_ptr = new param::TplEnum (
		"Linear\nLog",
		"Mode\nM",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_MODE, enu_ptr);

	// Clip
	enu_ptr = new param::TplEnum (
		"Off\nOn",
		"Clip\nC",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_CLIP, enu_ptr);
}



ParamDescSet &	EnvFollowDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	EnvFollowDesc::do_get_unique_id () const
{
	return "envf";
}



std::string	EnvFollowDesc::do_get_name () const
{
	return "Envelope Follower\nEnvelope Fol\nEnv Fol";
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
