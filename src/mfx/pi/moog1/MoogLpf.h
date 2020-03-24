/*****************************************************************************

        MoogLpf.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_moog1_MoogLpf_HEADER_INCLUDED)
#define mfx_pi_moog1_MoogLpf_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/DcKiller1p.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/va/MoogLadderDAngelo.h"
#include "mfx/pi/moog1/FltMode.h"
#include "mfx/pi/moog1/MoogLpfDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace moog1
{



class MoogLpf final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MoogLpf ();
	               ~MoogLpf () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _max_blk_size = 64; // Samples, standard sampling rate, multiple of 4
	static const int  _ovrspl       = 4;
	static const int  _nbr_coef_42  = 3;
	static const int  _nbr_coef_21  = 8;
	static const int  _nbr_weights  = 5;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
	typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

	typedef std::array <float, _nbr_weights> WeightList;

	class Channel
	{
	public:
		UpSpl          _upspl_m;         // Main signal
		UpSpl          _upspl_s;         // Sidechain
		DwSpl          _dwspl;

		float          _self_mod_save;   // Output value from the previous sample

		// The following filters are oversampled
		dsp::va::MoogLadderDAngelo <4>
		               _flt;
		dsp::iir::DcKiller1p
		               _dckill;
		std::array <dsp::iir::Biquad, 2>
		               _lpf_selfmod_arr;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_lpf_selfmod (float f);
	void           process_subblock_lp4_no_self (Channel &chn, int nbr_spl);
	void           process_subblock_standard (Channel &chn, int nbr_spl);
	void           set_stage_weights (FltMode flt_mode);

	static void    init_ovrspl_coef ();

	State          _state;

	MoogLpfDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq
	float          _inv_fs_ovr;

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_type;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_param;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_mod;

	ChannelArray   _chn_arr;

	FltMode        _flt_mode;
	dsp::ctrl::Ramp
	               _mod_sc_amp;
	dsp::ctrl::Ramp
	               _mod_self_amp;
	bool           _mod_sc_flag;        // Sidechain modulation enabled
	bool           _mod_self_flag;      // Self modulation enabled

	std::array <BufAlign, 2>            // Oversampled buffers. 0 = main, 1 = sidechain
	               _buf_arr;
	WeightList     _stage_weight_arr;   // Inputs of 4 stages + output

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;

	static const float
	               _sig_scale;
	static const std::array <WeightList, FltMode_NBR_ELT>
	               _weight_table;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MoogLpf (const MoogLpf &other)           = delete;
	               MoogLpf (MoogLpf &&other)                = delete;
	MoogLpf &      operator = (const MoogLpf &other)        = delete;
	MoogLpf &      operator = (MoogLpf &&other)             = delete;
	bool           operator == (const MoogLpf &other) const = delete;
	bool           operator != (const MoogLpf &other) const = delete;

}; // class MoogLpf



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/moog1/MoogLpf.hpp"



#endif   // mfx_pi_moog1_MoogLpf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
