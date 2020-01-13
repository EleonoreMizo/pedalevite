/*****************************************************************************

        DelayDesc.cpp
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

#include "mfx/pi/dly1/Cst.h"
#include "mfx/pi/dly1/DelayDesc.h"
#include "mfx/pi/dly1/Param.h"
#include "mfx/pi/param/MapPseudoLog.h"
#include "mfx/pi/param/MapS.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplLin.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplMapped.h"
#include "mfx/pi/ParamMapFdbk.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dly1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



DelayDesc::DelayDesc ()
:	_desc_set (Param_NBR_ELT, 0)
{
	init_level (false);
	init_level (true);
	for (int chn = 0; chn < Cst::_nbr_lines; ++chn)
	{
		init_time (chn);
		init_fdbk (chn);
		init_filter (chn);
	}

	// Link
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Off\nOn",
		"Link\nChn Link\nChannel Link",
		"",
		0,
		"%s"
	);
	_desc_set.add_glob (Param_LINK, enu_sptr);

	// Cross-freedback
	auto           maps_sptr =
		std::make_shared <param::TplMapped <param::MapS <false> > > (
			0, 1,
			"PP\nPiPo\nPing-pong\nPing-pong feedback",
			"%",
			param::HelperDispNum::Preset_FLOAT_PERCENT,
			0,
			"%5.1f"
		);
	maps_sptr->use_mapper ().config (
		maps_sptr->get_nat_min (),
		maps_sptr->get_nat_max ()
	);
	_desc_set.add_glob (Param_X_FDBK, maps_sptr);
}



ParamDescSet &	DelayDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	DelayDesc::do_get_unique_id () const
{
	return "delay1";
}



std::string	DelayDesc::do_get_name () const
{
	return "Delay";
}



void	DelayDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DelayDesc::do_prefer_stereo () const
{
	return true;
}



int	DelayDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DelayDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void DelayDesc::init_level (bool dry_flag)
{
	typedef param::TplMapped <param::MapPseudoLog> TplPsl;
	const char *	name_0 =
		  (dry_flag)
		? "MixI\nMix In\nInput mix\nInput mix level"
		: "MixD\nMix Dly\nDelay mix\nDelay mix level";
	const int      index  = (dry_flag) ? Param_LVL_IN : Param_LVL_OUT;
	auto           pl_sptr = std::make_shared <TplPsl> (
		0, 10,
		name_0,
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (index, pl_sptr);
}



void	DelayDesc::init_time (int chn)
{
	assert (chn >= 0);
	assert (chn < Cst::_nbr_lines);

	const char *	name_0 =
		  (chn == 0)
		? "TL\nTime L\nTime Left"
		: "TR\nTime R\nTime Right";
	const Param		index = (chn == 0) ? Param_L_TIME : Param_R_TIME;
	auto           p_sptr = std::make_shared <param::TplLog> (
		1.0 / 64, Cst::_max_delay / 1000.0,
		name_0,
		"ms",
		param::HelperDispNum::Preset_FLOAT_MILLI,
		0,
		"%6.1f"
	);
	_desc_set.add_glob (index, p_sptr);
}



void	DelayDesc::init_fdbk (int chn)
{
	assert (chn >= 0);
	assert (chn < Cst::_nbr_lines);

	const char *	name_0 =
		  (chn == 0)
		? "FbkL\nFdbk L\nFeedback L\nFeedback Left"
		: "FbkR\nFdbk R\nFeedback R\nFeedback Right";
	const Param		index = (chn == 0) ? Param_L_FDBK : Param_R_FDBK;
	auto           p_sptr = std::make_shared <param::TplLin> (
		0, 1,
		name_0,
		"%",
		0,
		"%5.1f"
	);
	p_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (index, p_sptr);
}



void	DelayDesc::init_filter (int chn)
{
	assert (chn >= 0);
	assert (chn < Cst::_nbr_lines);

	const char *	name_0 =
		  (chn == 0)
		? "FltL\nFilter L\nFilter Left"
		: "FltR\nFilter R\nFilter Right";
	const Param		index = (chn == 0) ? Param_L_FILTER : Param_R_FILTER;
	auto           p_sptr = std::make_shared <param::TplLin> (
		-1, 1,
		name_0,
		"%",
		0,
		"%+6.1f"
	);
	p_sptr->use_disp_num ().set_preset (
		param::HelperDispNum::Preset_FLOAT_PERCENT
	);
	_desc_set.add_glob (index, p_sptr);
}



}  // namespace dly1
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
