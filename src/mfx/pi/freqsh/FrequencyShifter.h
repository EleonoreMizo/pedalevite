/*****************************************************************************

        FrequencyShifter.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_freqsh_FrequencyShifter_HEADER_INCLUDED)
#define mfx_pi_freqsh_FrequencyShifter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/pi/cdsp/FreqShift.h"
#include "mfx/pi/freqsh/FreqShiftDesc.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace freqsh
{



class FrequencyShifter final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       FrequencyShifter (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           clear_buffers ();

	piapi::HostInterface &
	               _host;
	State          _state;

	FreqShiftDesc  _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	cdsp::FreqShift
	               _freq_shift;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FrequencyShifter ()                               = delete;
	               FrequencyShifter (const FrequencyShifter &other)  = delete;
	               FrequencyShifter (FrequencyShifter &&other)       = delete;
	FrequencyShifter &
	               operator = (const FrequencyShifter &other)        = delete;
	FrequencyShifter &
	               operator = (FrequencyShifter &&other)             = delete;
	bool           operator == (const FrequencyShifter &other) const = delete;
	bool           operator != (const FrequencyShifter &other) const = delete;

}; // class FrequencyShifter



}  // namespace freqsh
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/freqsh/FrequencyShifter.hpp"



#endif   // mfx_pi_freqsh_FrequencyShifter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
