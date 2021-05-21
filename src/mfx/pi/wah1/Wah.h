/*****************************************************************************

        Wah.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_wah1_Wah_HEADER_INCLUDED)
#define mfx_pi_wah1_Wah_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/wah1/WahDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace wah1
{



class Wah final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Wah (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <
		dsp::iir::Biquad,
		piapi::PluginInterface::_max_nbr_chn
	>	FilterArray;

	void           clear_buffers ();

	piapi::HostInterface &
	               _host;
	State          _state;

	WahDesc        _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	FilterArray    _filter_arr;
	float          _inv_fs;
	float          _freq;
	float          _q;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Wah ()                               = delete;
	               Wah (const Wah &other)               = delete;
	               Wah (Wah &&other)                    = delete;
	Wah &          operator = (const Wah &other)        = delete;
	Wah &          operator = (Wah &&other)             = delete;
	bool           operator == (const Wah &other) const = delete;
	bool           operator != (const Wah &other) const = delete;

}; // class Wah



}  // namespace wah1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/wah1/Wah.hpp"



#endif   // mfx_pi_wah1_Wah_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
