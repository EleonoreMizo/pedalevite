/*****************************************************************************

        Dist3Proc.h
        Author: Laurent de Soras, 2020

Objects of this class should be aligned on 16-bytes boudaries.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist3_Dist3Proc_HEADER_INCLUDED)
#define mfx_pi_dist3_Dist3Proc_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/dsp/dly/DelayPushPop.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/ctrl/VarBlock.h"
#include "mfx/dsp/shape/DistSlewRateLim.h"
#include "mfx/pi/dist3/Psu.h"
#include "mfx/pi/dist3/Shaper.h"
#include "mfx/pi/dist3/UpsplLerp4x.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dist3
{



class Dist3Proc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn = piapi::PluginInterface::_max_nbr_chn;

	               Dist3Proc ();
	virtual        ~Dist3Proc () = default;

	void           reset (double sample_freq, int max_block_size, double &latency);
	void           set_freq_hpf_pre (float f);
	void           set_freq_lpf_bias (float f);
	void           set_freq_lpf_post (float f);
	void           set_slew_rate_pre (float srl_hz);
	void           set_slew_rate_post (float srl_hz);
	void           set_gain (float gain);
	void           set_type (Shaper::Type type);
	void           set_bias_s (float bs);
	void           set_bias_d (float bd);
	void           set_bias_fdbk (bool fdbk_flag);
	void           set_class_b (bool b_flag);
	void           enable_psu (bool flag);
	void           set_psu_ac_freq (float f);
	void           set_psu_load (float load);
	void           process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _ovrspl      = 4;
	static const int  _nbr_coef_42 = 3;
	static const int  _nbr_coef_21 = 8;
	static const int  _fdbk_dly    = 16; // Must be a multiple of 4

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
	typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

	class Channel
	{
	public:
		dsp::iir::Biquad              // HPF 1 pole + DC killer 1 pole
			            _hpf_pre;
		dsp::iir::Biquad              // For the bias stuff
		               _lpf_bias;
		dsp::shape::DistSlewRateLim
		               _srl_pre;
		dsp::iir::Biquad
		               _hpf_srl;
		UpsplLerp4x    _us_bias;
		UpSpl          _us;
		std::array <Shaper, 2>
		               _shaper_arr;
		DwSpl          _ds;
		Psu            _psu;
		dsp::shape::DistSlewRateLim
		               _srl_post;
		dsp::iir::Biquad              // LPF 1 pole + DC killer 1 pole
			            _lpf_post;
		dsp::dly::DelayPushPop
		               _bias_dly;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           update_dc_killer ();
	void           update_hpf_pre ();
	void           update_lpf_post ();
	void           update_lpf_bias ();
	void           update_hpf_srl ();
	void           set_z_coef_w_dc_kill (dsp::iir::Biquad &biq, const float b1_z [2], const float a1_z [2]);
	void           process_sub_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);
	void           compute_bias (Channel &chn, const float src_ptr [], int nbr_spl);
	void           process_dist (Channel &chn, int nbr_spl);
	float          conv_srl_hz_to_s (float f);

	static void    init_ovrs_coef ();

	float          _sample_freq;     // Hz, > 0. 0 = not initialised
	float          _inv_fs;          // s, > 0. 0 = not initialised

	// Signal processors
	ChannelArray   _chn_arr;

	// Internal variables
	float          _freq_hpf_pre;    // Hz, ]0 ; fs/2[
	float          _freq_lpf_bias;   // Hz, ]0 ; fs/2[
	float          _freq_lpf_post;   // Hz, ]0 ; fs/2[
	dsp::ctrl::VarBlock
	               _gain_pre;
	dsp::ctrl::VarBlock
	               _gain_post;
	dsp::ctrl::VarBlock
	               _bias_s;
	dsp::ctrl::VarBlock
	               _bias_d;
	bool           _bias_fdbk_flag;
	bool           _class_b_flag;
	bool           _psu_flag;

	bool           _hpf_pre_dirty_flag;

	// z-coef for the DC killers. Order: b[], a[] (num, den)
	std::array <std::array <float, 2>, 2>
	               _dck_coef_arr;

	// Temporary
	BufAlign       _buf_x1;
	BufAlign       _buf_bias;
	std::array <BufAlign, 2>
	               _buf_ovr_arr;

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Dist3Proc (const Dist3Proc &other)         = delete;
	               Dist3Proc (Dist3Proc &&other)              = delete;
	Dist3Proc &    operator = (const Dist3Proc &other)        = delete;
	Dist3Proc &    operator = (Dist3Proc &&other)             = delete;
	bool           operator == (const Dist3Proc &other) const = delete;
	bool           operator != (const Dist3Proc &other) const = delete;

}; // class Dist3Proc



}  // namespace dist3
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist3/Dist3Proc.hpp"



#endif   // mfx_pi_dist3_Dist3Proc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
