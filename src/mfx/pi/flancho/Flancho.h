/*****************************************************************************

        Flancho.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_flancho_Flancho_HEADER_INCLUDED)
#define mfx_pi_flancho_Flancho_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/dsp/rspl/InterpolatorLerp.h"
#include "mfx/pi/flancho/FlanchoChn.h"
#include "mfx/pi/flancho/FlanchoDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{
namespace pi
{
namespace flancho
{



class Flancho final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Flancho ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _update_resol = 64;  // Must be a multiple of 4
	static const int  _ovrspl       = 4;
	static const int  _nbr_coef_42  = 3;
	static const int  _nbr_coef_21  = 8;

	typedef std::shared_ptr <FlanchoChn> ChnSPtr;

	typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
	typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

	typedef std::vector <float> Buffer;
	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		UpSpl          _us;
		DwSpl          _ds;
		ChnSPtr        _fchn_sptr;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChnArray;

	void           clear_buffers ();
	void           init_coef ();
	void				update_param (bool force_flag = false);
	void				update_lfo_period ();

	State          _state;

	FlanchoDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_depth_fdbk;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_wf;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_speed;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_delay;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_voices;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_phase_set;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_dry;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_ovrspl;

	ChnArray			_chn_arr;
	dsp::rspl::InterpolatorHermite43
						_interp_cubic;
	dsp::rspl::InterpolatorLerp
	               _interp_linear;
	Buffer			_buf_tmp;
	Buffer			_buf_render;
	BufAlign       _buf_ovrspl_src;
	BufAlign       _buf_ovrspl_dst;

	// Cached
	int				_nbr_chn_in;			// > 0. 0 = not set
	int				_nbr_chn_out;			// > 0. 0 = not set
	float          _mix;
	bool           _neg_flag;
	bool           _ovrspl_flag;
	int            _ovrspl_cur;         // Current oversampling rate (1 or _ovrspl)

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Flancho (const Flancho &other)           = delete;
	Flancho &      operator = (const Flancho &other)        = delete;
	bool           operator == (const Flancho &other) const = delete;
	bool           operator != (const Flancho &other) const = delete;

}; // class Flancho



}  // namespace flancho
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/flancho/Flancho.hpp"



#endif   // mfx_pi_flancho_Flancho_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
