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

#if fstb_IS (SYS, LINUX)
#define mfx_Cst_ROOT_DIR "/opt/pedalevite/"
#else
#define mfx_Cst_ROOT_DIR "../../../"
#endif



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const float	Cst::_clip_lvl = 0.999f;   // We subtract a tiny margin to 0 dB to be safe

const std::chrono::microseconds	Cst::_tempo_detection_max (
	std::chrono::seconds (2)
);
const std::chrono::microseconds	Cst::_tempo_detection_min (
	std::chrono::milliseconds (100)
);

const std::chrono::microseconds	Cst::_key_time_hold (
	std::chrono::milliseconds (500)
);
const std::chrono::microseconds	Cst::_key_time_repeat (
	std::chrono::milliseconds (150)
);

const double	Cst::_step_param = 0.05;

const std::string	Cst::_plugin_dwm     = "\?drywetmix";
const std::string	Cst::_plugin_mix     = "\?mix";
const std::string	Cst::_plugin_dly     = "\?delay0";
const std::string	Cst::_plugin_tuner   = "\?tuner";

const std::string	Cst::_empty_preset_name = "<Empty prog>";

const std::string	Cst::_config_current = "current";
const std::string	Cst::_config_factory = "factory-default";
const std::string	Cst::_config_dir     = mfx_Cst_ROOT_DIR "etc/config";
const std::string	Cst::_audiodump_dir  = mfx_Cst_ROOT_DIR "var/audiodump";
const std::string	Cst::_d2d_file       = "raw-in-out.wav";
const std::string	Cst::_rw_cmd_script_pathname = mfx_Cst_ROOT_DIR "bin/cmd_rofs.sh";

const Cst::QueueTypeArray	Cst::_queue_type_arr =
{{
	// UserInputType_SW
	{{
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,

		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,

		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD  ,
		UserInputQueueType_CMD  , UserInputQueueType_CMD
	}},

	// UserInputType_POT
	{{
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,

		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,

		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO
	}},

	// UserInputType_ROTENC
	{{
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_AUDIO,
		UserInputQueueType_AUDIO, UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,

		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,

		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI  ,
		UserInputQueueType_GUI  , UserInputQueueType_GUI
	}}
}};



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#undef mfx_Cst_ROOT_DIR



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
