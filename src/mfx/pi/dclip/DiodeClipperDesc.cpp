/*****************************************************************************

        DiodeClipperDesc.cpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/dclip/DiodeClipperDesc.h"
#include "mfx/pi/dclip/Param.h"
#include "mfx/pi/dclip/Shape.h"
#include "mfx/pi/param/MapPiecewiseLinLog.h"
#include "mfx/pi/param/TplLog.h"
#include "mfx/pi/param/TplEnum.h"
#include "mfx/pi/param/TplMapped.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace dclip
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const double	DiodeClipperDesc::_gain_min = 0.1;
const double	DiodeClipperDesc::_gain_max = 1000;



DiodeClipperDesc::DiodeClipperDesc ()
:	_desc_set (Param_NBR_ELT, 0)
,	_info ()
{
	_info._unique_id = "dclip";
	_info._name      = "Diode clipper\nDiode clip\nD. clip\nDClip";

	typedef param::TplMapped <param::MapPiecewiseLinLog> TplPll;

	// Gain
	auto           log_sptr = std::make_shared <param::TplLog> (
		double (_gain_min), double (_gain_max),
		"Distortion gain\nDist gain\nGain\nG",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	_desc_set.add_glob (Param_GAIN, log_sptr);

	// Low-pass filter cutoff frequency
	log_sptr = std::make_shared <param::TplLog> (
		20, 20480,
		"Low-pass filter cutoff frequency\nLPF cutoff frequency"
		"\nLPF frequency\nLPF freq\nLPF",
		"Hz",
		param::HelperDispNum::Preset_FLOAT_STD,
		0,
		"%5.0f"
	);
	log_sptr->set_categ (piapi::ParamDescInterface::Categ_FREQ_HZ);
	_desc_set.add_glob (Param_CUTOFF, log_sptr);

	// Shape
	auto           enu_sptr = std::make_shared <param::TplEnum> (
		"Sym-H\nAsym-H\nSym-S\nAsym-S",
		"Shape\nShp",
		"",
		0,
		"%s"
	);
	assert (enu_sptr->get_nat_max () == Shape_NBR_ELT - 1);
	_desc_set.add_glob (Param_SHAPE, enu_sptr);

	// Preclip level
	auto           pll_sptr = std::make_shared <TplPll> (
		1, 100,
		"Preclip level\nPreclip lvl\nPreclip\nPCl",
		"dB",
		param::HelperDispNum::Preset_DB,
		0,
		"%+5.1f"
	);
	pll_sptr->use_mapper ().gen_log (8);
	_desc_set.add_glob (Param_PRECLIP, pll_sptr);
}



ParamDescSet &	DiodeClipperDesc::use_desc_set ()
{
	return _desc_set;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInfo	DiodeClipperDesc::do_get_info () const
{
	return _info;
}



void	DiodeClipperDesc::do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const
{
	nbr_i = 1;
	nbr_o = 1;
	nbr_s = 0;
}



bool	DiodeClipperDesc::do_prefer_stereo () const
{
	return false;
}



int	DiodeClipperDesc::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc_set.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	DiodeClipperDesc::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc_set.use_param (categ, index);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dclip
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
