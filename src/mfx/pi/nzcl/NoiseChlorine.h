/*****************************************************************************

        NoiseChlorine.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_nzcl_NoiseChlorine_HEADER_INCLUDED)
#define mfx_pi_nzcl_NoiseChlorine_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/pi/nzcl/Cst.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/nzcl/Notch.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace nzcl
{



class NoiseChlorine final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       NoiseChlorine (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Channel
	{
	public:
		std::array <Notch, Cst::_nbr_notches>
		               _notch_arr;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           clear_buffers ();
	void           update_param (bool force_flag);
	void           update_all_levels ();

	piapi::HostInterface &
	               _host;
	State          _state;

	NoiseChlorineDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq, <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	std::array <fstb::util::NotificationFlagCascadeSingle, Cst::_nbr_notches>
	               _param_change_flag_notch_arr;

	ChannelArray   _chn_arr;

	float          _lvl_base;
	std::array <float, Cst::_nbr_notches>
	               _lvl_notch_arr;

	std::array <BufAlign, 2>
	               _buf_tmp_arr;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               NoiseChlorine ()                               = delete;
	               NoiseChlorine (const NoiseChlorine &other)     = delete;
	               NoiseChlorine (NoiseChlorine &&other)          = delete;
	NoiseChlorine& operator = (const NoiseChlorine &other)        = delete;
	NoiseChlorine& operator = (NoiseChlorine &&other)             = delete;
	bool           operator == (const NoiseChlorine &other) const = delete;
	bool           operator != (const NoiseChlorine &other) const = delete;

}; // class NoiseChlorine



}  // namespace nzcl
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/nzcl/NoiseChlorine.hpp"



#endif   // mfx_pi_nzcl_NoiseChlorine_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
