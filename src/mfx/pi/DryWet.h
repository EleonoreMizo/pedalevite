/*****************************************************************************

        DryWet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_DryWet_HEADER_INCLUDED)
#define mfx_pi_DryWet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define mfx_pi_DryWet_GAIN_WET_ONLY

#include "fstb/AllocAlign.h"
#include "fstb/util/NotificationFlag.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"



namespace mfx
{
namespace pi
{



class DryWet
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Param
	{
		Param_BYPASS = 0,
		Param_WET,
		Param_GAIN,

		Param_NBR_ELT
	};

	               DryWet ();
	virtual        ~DryWet () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual int    do_init ();
	virtual int    do_restore ();
	virtual void   do_get_nbr_io (int &nbr_i, int &nbr_o) const;
	virtual bool   do_prefer_stereo () const;
	virtual int    do_get_nbr_param (piapi::ParamCateg categ) const;
	virtual const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           copy (const ProcInfo &proc, int chn_ofs, float lvl);
	void           mix (const ProcInfo &proc, float lvl_wet_beg, float lvl_wet_end, float lvl_dry_beg, float lvl_dry_end);

	State          _state;

	ParamDescSet   _desc_set;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	float          _level_wet;          // For steady state
	float          _level_dry;          // For steady state



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DryWet (const DryWet &other)            = delete;
	DryWet &       operator = (const DryWet &other)        = delete;
	bool           operator == (const DryWet &other) const = delete;
	bool           operator != (const DryWet &other) const = delete;

}; // class DryWet



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/DryWet.hpp"



#endif   // mfx_pi_DryWet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
