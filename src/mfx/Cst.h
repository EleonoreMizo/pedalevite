/*****************************************************************************

        Cst.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_Cst_HEADER_INCLUDED)
#define mfx_Cst_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/PluginInterface.h"
#include "mfx/ui/UserInputType.h"

#include <array>
#include <chrono>
#include <string>



namespace mfx
{



class Cst
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Saved document format version
	static const int  _format_version  =  12;

	static const int  _nbr_pedals      =  12;
	static const int  _nbr_prog_per_bank = 16;
	static const int  _nbr_banks       =  64;  // Per setup

	static const int  _nbr_pot         =   3;

	enum RotEnc
	{
		RotEnc_GEN     = 0,
		RotEnc_NBR_GEN = 5,

		RotEnc_NAV_VER = RotEnc_GEN + RotEnc_NBR_GEN,
		RotEnc_NAV_HOR,

		RotEnc_NBR_ELT
	};

	enum UserInputQueueType
	{
		UserInputQueueType_CMD = 0,
		UserInputQueueType_GUI,
		UserInputQueueType_AUDIO,

		UserInputQueueType_NBR_ELT
	};

	static const int  _max_input_param =  24; // Maximum number of input parameter for each type (switches, expression pedals, rotary encoders...)
	static const int  _max_nbr_buf     = 256; // Number of buffers in the audio graph processing
	static const int  _max_nbr_input   = piapi::PluginInterface::_max_nbr_pins; // Per real plug-in (not dry/wet/bypass mixer)
	static const int  _max_nbr_output  = piapi::PluginInterface::_max_nbr_pins; // Per real plug-in (not dry/wet/bypass mixer)
	static const int  _max_nbr_sig     =   4;
	static const int  _max_nbr_sig_ports = 256;
	static const int  _max_nbr_plugins = 256;
	static const int  _max_nbr_send    =   4;
	static const int  _nbr_chn_inout   =   2; // Max of _nbr_chn_in and _nbr_chn_out
	static const int  _nbr_chn_in      = _nbr_chn_inout;
	static const int  _nbr_chn_out     = _nbr_chn_inout;
	static const float
	                  _clip_lvl;

	static const int  _tempo_min       =  30;
	static const int  _tempo_max       = 240;
	static const int  _tempo_ref       = 120;
	static const std::chrono::microseconds    // Maximum time between two tempo pedal actions. Microseconds
	                  _tempo_detection_max;
	static const std::chrono::microseconds    // Minimum time, microseconds
	                  _tempo_detection_min;

	static const std::chrono::microseconds    // Microseconds
	                  _key_time_hold;
	static const std::chrono::microseconds    // Microseconds
	                  _key_time_repeat;
	static const int  _max_named_targets = 64;// Maximum number of effects that can be addressed simultaneously in the audio thread for a parameter change, for example

	static const double  _step_param;         // Unit step on normalized parameter value

	// Number of buffers, per side, for the return device
	static const int  _nbr_buf_ret     = _nbr_chn_inout * _max_nbr_send;

	enum BufSpecial
	{
		BufSpecial_SILENCE = 0,
		BufSpecial_TRASH,
		BufSpecial_RET,

		BufSpecial_NBR_ELT = BufSpecial_RET + _nbr_buf_ret 
	};

	static const std::string      // Dry-wet mix
	               _plugin_dwm;
	static const std::string
	               _plugin_dly;
	static const std::string
	               _plugin_tuner;

	static const std::string
	               _empty_prog_name;

	static const std::string
	               _config_current;
	static const std::string
	               _config_factory;
	static const std::string
	               _config_dir;
	static const std::string
	               _font_dir;
	static const std::string
	               _audiodump_dir;
	static const std::string
	               _log_dir;
	static const std::string
	               _d2d_file;
	static const std::string
	               _rw_cmd_script_pathname;

	typedef std::array <
		std::array <UserInputQueueType, _max_input_param>,
		ui::UserInputType_NBR_ELT
	> QueueTypeArray;

	static const QueueTypeArray
	               _queue_type_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	virtual        ~Cst ()                              = delete;
	               Cst ()                               = delete;
	               Cst (const Cst &other)               = delete;
	Cst &          operator = (const Cst &other)        = delete;
	bool           operator == (const Cst &other) const = delete;
	bool           operator != (const Cst &other) const = delete;

}; // class Cst



}  // namespace mfx



//#include "mfx/Cst.hpp"



#endif   // mfx_Cst_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
