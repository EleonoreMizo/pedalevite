/*****************************************************************************

        Cst.cpp
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
#include "mfx/Cst.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const float	Cst::_clip_lvl = 0.999f;   // We subtract a tiny margin to 0 dB to be safe

const double	Cst::_step_param = 0.05;

const std::string	Cst::_plugin_mix     = "\?drywetmix";
const std::string	Cst::_plugin_tuner   = "\?tuner";

const std::string	Cst::_empty_preset_name = "<Empty preset>";

const std::string	Cst::_config_current = "current";      // Filename
const std::string	Cst::_config_factory = "factory-default";
const std::string	Cst::_config_dir     =
#if fstb_IS (SYS, LINUX)
	"/opt/pedalevite/etc/config";
#else
	"../etc/config";
#endif
const std::string	Cst::_copy_script_pathname = "/opt/pedalevite/bin/mv_rofs.sh";



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
