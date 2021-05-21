/*****************************************************************************

        BigMuff1.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_bmp1_BigMuff1_HEADER_INCLUDED)
#define mfx_pi_bmp1_BigMuff1_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/va/BigMuffPi.h"
#include "mfx/pi/bmp1/BigMuff1Desc.h"
#include "mfx/pi/bmp1/Ovrspl.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace bmp1
{



class BigMuff1 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BigMuff1 (piapi::HostInterface &host);
	               ~BigMuff1 () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _ovrspl_ratio_max = 4;
	static constexpr int _nbr_coef_42      = 3;
	static constexpr int _nbr_coef_21      = 8;

	// Signal scale: input is amplified by X and output attenuated by 1/X
	static constexpr float  _sig_scale     = 4.f;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		dsp::va::BigMuffPi
		               _bmp;
		dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21>
		               _uspl;
		dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21>
		               _dspl;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_oversampling ();

	piapi::HostInterface &
	               _host;
	State          _state;

	BigMuff1Desc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_core;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;

	ChannelArray   _chn_arr;
	Ovrspl         _ovrspl;
	int            _ovrspl_rate;
	float          _gain_cur;           // Cumulates _sig_scale with the gain
	float          _gain_old;

	BufAlign       _buf_tmp;
	BufAlign       _buf_ovr;

	// 4x, 77 dB
	// Total GD    : 4.0 spl
	// Bandwidth   : 21335 Hz @ 44100 Hz

	// Attenuation : 79.6658 dB
	// Trans BW    : 0.212423
	// Group delay : 2.06 spl
	// GD rel freq : 0.00566893
	static constexpr std::array <double, _nbr_coef_42>
	               _coef_42 {{
							0.07819449364682253,
							0.30699114982473047,
							0.70279393407418822
						}};
	// Attenuation : 77.468 dB
	// Trans BW    : 0.016203
	// Group delay : 2.97 spl
	// GD rel freq : 0.0113379
	static constexpr std::array <double, _nbr_coef_21>
	               _coef_21 {{
							0.063197755826959232,
							0.22379856090736752,
							0.41991987445736001,
							0.60001868664013414,
							0.74142961506492877,
							0.84386929889458695,
							0.917212853263219,
							0.97391838508705231
						}};



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BigMuff1 ()                               = delete;
	               BigMuff1 (const BigMuff1 &other)          = delete;
	               BigMuff1 (BigMuff1 &&other)               = delete;
	BigMuff1 &     operator = (const BigMuff1 &other)        = delete;
	BigMuff1 &     operator = (BigMuff1 &&other)             = delete;
	bool           operator == (const BigMuff1 &other) const = delete;
	bool           operator != (const BigMuff1 &other) const = delete;

}; // class BigMuff1



}  // namespace bmp1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/bmp1/BigMuff1.hpp"



#endif   // mfx_pi_bmp1_BigMuff1_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
