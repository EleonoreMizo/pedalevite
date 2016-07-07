/*****************************************************************************

        IIFix.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_iifix_IIFix_HEADER_INCLUDED)
#define mfx_pi_iifix_IIFix_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/pi/iifix/IIFixDesc.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace iifix
{



class IIFix
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               IIFix ();
	virtual        ~IIFix () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <dsp::iir::OnePole, _max_nbr_chn> ChannelArray;

	void           update_filter ();

	State          _state;

	IIFixDesc   _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	float          _inv_fs;
	ChannelArray   _chn_arr;
	float          _level;              // > 0
	float          _freq;               // Hz, > 0



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               IIFix (const IIFix &other)          = delete;
	IIFix &     operator = (const IIFix &other)        = delete;
	bool           operator == (const IIFix &other) const = delete;
	bool           operator != (const IIFix &other) const = delete;

}; // class IIFix



}  // namespace iifix
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/iifix/IIFix.hpp"



#endif   // mfx_pi_iifix_IIFix_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
