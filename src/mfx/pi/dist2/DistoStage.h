/*****************************************************************************

        DistoStage.h
        Author: Laurent de Soras, 2016

Objects of this class should be aligned on 16-bytes boudaries.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist2_DistoStage_HEADER_INCLUDED)
#define mfx_pi_dist2_DistoStage_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dyn/LimiterRms.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/shape/DistAttract.h"
#include "mfx/dsp/shape/DistBounce.h"
#include "mfx/dsp/shape/DistRandWalk.h"
#include "mfx/dsp/shape/DistSlewRateLim.h"
#include "mfx/pi/cdsp/ShaperBag.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <ratio>
#include <vector>

#include <cmath>



namespace mfx
{
namespace pi
{
namespace dist2
{



class DistoStage
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn = piapi::PluginInterface::_max_nbr_chn;

	enum Type
	{
		Type_ASINH = 0,
		Type_ASYM1,
		Type_PROG1,
		Type_PROG2,
		Type_PROG3,
		Type_SUDDEN,
		Type_HARDCLIP,
		Type_PUNCHER1,
		Type_PUNCHER2,
		Type_PUNCHER3,
		Type_OVERSHOOT,
		Type_BITCRUSH,
		Type_SLEWRATE,
		Type_LOPSIDED,
		Type_PORRIDGE,
		Type_SMARTE1,
		Type_SMARTE2,
		Type_ATTRACT,
		Type_RANDWALK,
		Type_SQRT,
		Type_BELT,
		Type_BADMOOD,
		Type_BOUNCE,
		Type_LIGHT1,
		Type_LIGHT2,
		Type_LIGHT3,
		Type_TANH,
		Type_TANHLIN,
		Type_BREAK,
		Type_ASYM2,

		Type_NBR_ELT
	};

	               DistoStage ();

	void           reset (double sample_freq, int max_block_size, double &latency);
	void           set_hpf_pre_cutoff (float f);
	void           set_lpf_post_cutoff (float f);
	void           set_bias (float bias);
	void           set_bias_freq (float f);
	void           set_type (Type type);
	void           set_gain_pre (float g);
	void           set_gain_post (float g);
	void           process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int     _ovrspl      = 4;
	static const int     _nbr_coef_42 = 3;
	static const int     _nbr_coef_21 = 8;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
	typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

	class Channel
	{
	public:
		dsp::iir::Biquad
		               _hpf_pre;
		dsp::iir::Biquad
		               _lpf_post;
		dsp::iir::Biquad                 // For the bias stuff
		               _lpf_env;
		UpSpl          _us;
		DwSpl          _ds;
		dsp::dyn::LimiterRms
		               _porridge_limiter;
		dsp::shape::DistAttract
		               _attractor;
		dsp::shape::DistRandWalk
		               _random_walk;
		dsp::shape::DistBounce
		               _bounce;
		dsp::shape::DistSlewRateLim
		               _slew_rate_lim;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           init_coef ();
	void           set_next_block ();
	void           update_hpf_pre ();
	void           update_lpf_post ();
	void           update_lpf_bias ();
	void           distort_block (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	template <typename S>
	void           distort_block_shaper (S &shaper, float dst_ptr [], const float src_ptr [], int nbr_spl);

	cdsp::ShaperBag &
	               _sbag;
	ChannelArray   _chn_arr;
	float          _sample_freq;
	float          _inv_fs;
	float          _freq_hpf_pre;
	float          _freq_lpf_post;
	float          _freq_lpf_bias;
	float          _gain_pre;
	float          _gain_pre_old;
	float          _gain_post;
	float          _gain_post_old;
	float          _bias;
	float          _bias_old;
	Type           _type;
	BufAlign       _buf_x1;
	BufAlign       _buf_ovr;
	bool           _post_clear_flag;

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistoStage (const DistoStage &other)        = delete;
	               DistoStage (const DistoStage &&other)       = delete;
	DistoStage &   operator = (const DistoStage &other)        = delete;
	DistoStage &   operator = (const DistoStage &&other)       = delete;
	bool           operator == (const DistoStage &other) const = delete;
	bool           operator != (const DistoStage &other) const = delete;

}; // class DistoStage



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/DistoStage.hpp"



#endif   // mfx_pi_dist2_DistoStage_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
