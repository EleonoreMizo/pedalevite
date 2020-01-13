/*****************************************************************************

        DistApf.h
        Author: Laurent de Soras, 2019

Effect based on this idea:
Jussi Pekonen,
Coefficient-modulated first-order allpass filter as distortion effect,
Proceedings of the 11th International Conference on Digital Audio Effects
(DAFx-08), 2008

Here, the modulator sets the center frequency of the analogue all-pass filter,
hence modulating the coefficient of the digital APF is a way which is less
dependent of the sampling rate.

TO DO:
- Option to side-chain the modulation signal

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distapf_DistApf_HEADER_INCLUDED)
#define mfx_pi_distapf_DistApf_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/pi/distapf/DistApfDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace distapf
{



class DistApf final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistApf ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _freq_min     =   500;  // Minimum frequency, Hz
	static const int  _freq_max     = 21550;
	static const int  _ovrspl_ratio = 4;
	static const int  _nbr_coef_42  = 3;
	static const int  _nbr_coef_21  = 8;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		float          _srl_state = 0;
		float          _apf_mem_x = 0;
		float          _apf_mem_y = 0;
		dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21>
		               _ovrspl_up;
		dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21>
		               _ovrspl_dw;
	};

	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           update_param (bool force_flag = false);
	void           update_freq ();
	void           clear_buffers ();

	static void    init_coef ();

	State          _state;

	DistApfDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_ovrspl;

	float          _sample_freq;
	float          _inv_fs;
	ChannelArray   _chn_arr;
	BufAlign       _buf_tmp;
	BufAlign       _buf_ovr;

	float          _gain_cur;
	float          _gain_old;
	float          _map_a;
	float          _map_b;
	float          _freq_scale;
	float          _srl_scale; // Hz -> units/sample
	float          _srl;
	bool           _ovrspl_flag;

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistApf (const DistApf &other)           = delete;
	DistApf &      operator = (const DistApf &other)        = delete;
	bool           operator == (const DistApf &other) const = delete;
	bool           operator != (const DistApf &other) const = delete;

}; // class DistApf



}  // namespace distapf
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distapf/DistApf.hpp"



#endif   // mfx_pi_distapf_DistApf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
