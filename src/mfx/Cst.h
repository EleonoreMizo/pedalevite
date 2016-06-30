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

#include <string>



namespace mfx
{



class Cst
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_pedals      =  12;
	static const int  _nbr_presets_per_bank = 64;
	static const int  _nbr_banks       =  16;  // Per setup

	static const int  _max_nbr_buf     = 256;
	static const int  _max_nbr_input   =   1; // Per real plug-in (not dry/wet/bypass mixer)
	static const int  _max_nbr_output  =   1; // Per real plug-in (not dry/wet/bypass mixer)
	static const int  _max_nbr_plugins = 256;
	static const int  _nbr_chn_in      =   2;
	static const int  _nbr_chn_out     =   2;

	static const int  _tempo_min       =  60;
	static const int  _tempo_max       = 240;
	static const int  _tempo_ref       = 120;
	static const int  _tempo_detection_max = 2 * 1000*1000; // Maximum time between two tempo pedal actions. Microseconds
	static const int  _tempo_detection_min =      100*1000; // Minimum time, microseconds

	static const int  _key_time_hold   =  500*1000; // Microseconds
	static const int  _key_time_repeat =  150*1000; // Microseconds

	static const double  _step_param;         // Unit step on normalized parameter value

	enum BufSpecial
	{
		BufSpecial_SILENCE = 0,
		BufSpecial_TRASH,

		BufSpecial_NBR_ELT
	};

	static const std::string
	               _plugin_mix;
	static const std::string
	               _plugin_tuner;



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
