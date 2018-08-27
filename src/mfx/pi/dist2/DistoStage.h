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
#include "mfx/dsp/shape/FncFiniteAsym.h"
#include "mfx/pi/dist2/DistoDspAttract.h"
#include "mfx/pi/dist2/DistoDspRandWalk.h"
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
		Type_DIODE_CLIPPER = 0,
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

		Type_NBR_ELT
	};

	               DistoStage ();
	virtual        ~DistoStage () = default;

	void           reset (double sample_freq, int max_block_size);
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
		float          _slew_rate_val = 0;
		dsp::dyn::LimiterRms
		               _porridge_limiter;
		DistoDspAttract
		               _attractor;
		DistoDspRandWalk
		               _random_walk;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	class FncTanh
	{
	public:
		double         operator () (double x)
		{
			return tanh (x);
		}
	};

	class FncAtan
	{
	public:
		double         operator () (double x)
		{
			return (2 / fstb::PI) * atan (fstb::PI * 0.5 * x);
		}
	};

	class FncDiodeClipper
	{
	public:
		double         operator () (double x)
		{
			return asinh (x);
		}
	};

	// A, B and C are std::ratio templates
	template <typename A, typename B, typename C>
	class FncProgClipper
	{
	public:
		double         operator () (double x)
		{
			const double   a  = double (A::num) / double (A::den);
			const double   bi = double (B::den) / double (B::num);
			const double   c  = double (C::num) / double (C::den);
			const double   f1 = tanh (x * bi);
			const double   f2 = (2 / fstb::PI) * atan (fstb::PI * 0.5 * x * (1 / (1 - a) - bi));
			const double   x3 = x * x * x;
			const double   z  = fabs (x * 2) + 1;
			const double   z2 = z * z;
			const double   z5 = z2 * z2 * z;
			const double   f3 = 8 * x3 / z5;
			const double   y  = a * f1 + (1 - a) * f2 - c * f3;
			return y;
		}
	};

	// Minimum input range: [-8; +8]
	class FncPuncherA
	{
	public:
		double         operator () (double x)
		{
			const double   xx = fstb::limit (
				x,
				double (fstb::PI * -2.5),
				double (fstb::PI * +2.5)
			);

			return sin (xx);
		}
	};

	// Minimum input range: [-20; +20]
	template <int N>
	class FncPuncherB
	{
		static_assert ((N >= 0), "");
	public:
		double         operator () (double x)
		{
			const double   m  = 20; // Range
			const double   z  = log (m + 1);
			const double   a  = ((1.5 + N) * fstb::PI - z) / (z * z);
			const double   xx = fstb::limit (x, -m, +m);
			const double   u  = log (fabs (xx) + 1);

			return std::copysign (sin ((a * u + 1) * u), x);
		}
	};

	class FncOvershoot
	{
	public:
		double         operator () (double x)
		{
			const double   a   = exp (x - 1);
			const double   b   = exp (-x);
			const double   num = a - b - (1 / fstb::EXP1) + 1;
			const double   den = a + b;

			return num / den;
		}
	};

	// Minimum input range: [-8; +8]
	template <int B>
	class FncSmartE
	{
		static_assert ((B >= 1), "");
	public:
		double         operator () (double x)
		{
			const double   a    = -0.86;  // Range: [-0.86, 2.1]
			const double   xabs = fabs (x);
			const double   bx   = B * x;
			const double   num  = bx * (xabs + a);
			const double   den  = bx * x + xabs * (a - 1) + 1;

			return num / den;
		}
	};

	class FncLopsided
	{
	public:
		double         operator () (double x)
		{
			return std::copysign (1 - cos (x) * exp (-fabs (x)), x);
		}
	};

	template <class FNC>
	using ShaperLong = dsp::shape::FncFiniteAsym <
		-256, 256, FNC
	>;

	template <class FNC>
	using ShaperStd = dsp::shape::FncFiniteAsym <
		-20, 20, FNC, 2
	>;

	template <class FNC>
	using ShaperRes = dsp::shape::FncFiniteAsym <
		-16, 16, FNC, 4
	>;

	template <class FNC>
	using ShaperShort = dsp::shape::FncFiniteAsym <
		-8, 8, FNC, 4
	>;

	typedef ShaperShort <FncTanh> ShaperTanh;
	typedef ShaperStd <FncAtan> ShaperAtan;
	typedef ShaperLong <FncDiodeClipper> ShaperDiode;
	typedef ShaperStd <FncProgClipper <
		std::ratio < 2, 4>,
		std::ratio < 4, 1>,
		std::ratio < 2, 1>
	> > ShaperProg1;
	typedef ShaperStd <FncProgClipper <
		std::ratio < 3, 4>,
		std::ratio <10, 1>,
		std::ratio < 0, 1>
	> > ShaperProg2;
	typedef ShaperStd <FncPuncherB <0> > ShaperPuncher1;
	typedef ShaperStd <FncPuncherB <1> > ShaperPuncher2;
	typedef ShaperShort <FncPuncherA> ShaperPuncher3;
	typedef ShaperShort <FncOvershoot> ShaperOvershoot;
	typedef ShaperShort <FncLopsided> ShaperLopsided;
	typedef ShaperRes <FncSmartE <1> > ShaperSmartE1;
	typedef ShaperShort <FncSmartE <2> > ShaperSmartE2;

	void           init_coef ();
	void           set_next_block ();
	void           update_hpf_pre ();
	void           update_lpf_post ();
	void           update_lpf_bias ();
	void           distort_block (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	template <typename S>
	void           distort_block_shaper (S &shaper, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_asym1 (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_rcp1 (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_rcp2 (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_hardclip (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_bitcrush (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_slewrate_limit (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_sqrt (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_belt (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);
	void           distort_block_badmood (Channel &chn, float dst_ptr [], const float src_ptr [], int nbr_spl);

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
	float          _slew_rate_limit; // Amplitude units per sample
	Type           _type;
	BufAlign       _buf_x1;
	BufAlign       _buf_ovr;

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;

	static ShaperTanh
	               _shaper_tanh;
	static ShaperAtan
	               _shaper_atan;
	static ShaperDiode
	               _shaper_diode_clipper;
	static ShaperProg1
	               _shaper_prog1;
	static ShaperProg2
	               _shaper_prog2;
	static ShaperPuncher1
	               _shaper_puncher1;
	static ShaperPuncher2
	               _shaper_puncher2;
	static ShaperPuncher3
	               _shaper_puncher3;
	static ShaperOvershoot
	               _shaper_overshoot;
	static ShaperLopsided
	               _shaper_lopsided;
	static ShaperSmartE1
	               _shaper_smarte1;
	static ShaperSmartE2
	               _shaper_smarte2;

	static const float
	               _asym1_m_9;
	static const float
	               _asym1_m_2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistoStage (const DistoStage &other)        = delete;
	DistoStage &   operator = (const DistoStage &other)        = delete;
	bool           operator == (const DistoStage &other) const = delete;
	bool           operator != (const DistoStage &other) const = delete;

}; // class DistoStage



}  // namespace dist2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist2/DistoStage.hpp"



#endif   // mfx_pi_dist2_DistoStage_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
