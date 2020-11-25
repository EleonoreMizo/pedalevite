/*****************************************************************************

        LatticeReverb.h
        Author: Laurent de Soras, 2020

Reverberation effect based on ryukau's LatticeReverb
https://ryukau.github.io/VSTPlugins/manual/LatticeReverb/LatticeReverb_en.html

The main difference is the all-pass delay which is based on a 4-point FIR
interpolator, preserving better the high frequency content than the original
double linear interpolator.

Template parameters:

- T: sample data type (floating point)

- N: number of all-pass delays. 16 for the standard LatticeReverb.

- MAXDT: maximum delay time is seconds, specified as an std::ratio.
	Should be >= 0.1 s.
	Default is 1 s, like the original LatticeReverb.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_ltc_LatticeReverb_HEADER_INCLUDED)
#define mfx_dsp_spat_ltc_LatticeReverb_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/RndXoroshiro128p.h"
#include "mfx/dsp/iir/Lpf1p.h"
#include "mfx/dsp/ctrl/SmootherLpf.h"
#include "mfx/dsp/spat/ltc/LatticeStereo.h"
#include "mfx/dsp/spat/Cascade.h"
#include "mfx/dsp/spat/DelayAllPass.h"

#include <array>
#include <ratio>
#include <utility>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace ltc
{



template <typename T, int N, typename MAXDT = std::ratio <1, 1> >
class LatticeReverb
{
	static_assert (N > 0, "N must be greater than 0");
	static_assert (
		std::ratio_greater_equal <MAXDT, std::ratio <1, 10> >::value,
		"Maximum delay time should be >= 0.1 s"
	);

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _depth = N;
	static constexpr float  _time_min = 0.1e-3f; // s
	static constexpr float  _time_max = float (double (MAXDT::num) / double (MAXDT::den)); // s

	enum Stereo
	{
		// Main parameter value
		Stereo_BASE = 0,

		// Stereo spread, [-1 ; 1]
		// -1: 2x more to the left, 0 on the right
		//  0: same parameter for both channels
		// +1: 2x more to the right, 0 on the left
		Stereo_SPREAD,

		Stereo_NBR_ELT
	};

	               LatticeReverb ();

	void           set_sample_freq (double sample_freq);

	void           set_smooth_time (float t);
	void           set_cross_feedback (float cf);
	void           set_stereo_spread (float spread);

	void           set_time_mul (Stereo s, float m);
	void           set_fdbk_apf_mul (Stereo s, float m);
	void           set_fdbk_ltc_mul (Stereo s, float m);

	void           set_time (Stereo s, int idx, float t);
	void           set_fdbk_apf (Stereo s, int idx, float c);
	void           set_fdbk_ltc (Stereo s, int idx, float c);

	void           set_damping (int idx, float d);

	void           set_lfo_freq (float f);
	void           set_lfo_depth (int idx, float t);

	std::pair <T, T>
	               process_sample (T xl, T xr);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef DelayAllPass <T, _depth> DelayApf;
	typedef iir::Lpf1p <T> Lpf;
	typedef Cascade <DelayApf, Lpf> DelayApfLpf;
	typedef LatticeStereo <DelayApfLpf, _depth> LtcSt;

	static constexpr int _nbr_chn        = LtcSt::_nbr_chn;
	static constexpr int _lfo_update_per = 64; // Samples, > 0

	typedef ctrl::SmootherLpf <T> Smoother;
	typedef std::array <Smoother, _depth> SmootherArray;
	typedef std::array <T, _depth> ScalarArray;
	typedef std::array <SmootherArray, _nbr_chn> SmootherArrayS;

	typedef std::array <Lpf, _depth> LpfArray;
	typedef std::array <LpfArray, _nbr_chn> LpfArrayS;

	class ParamCellSt
	{
	public:
		std::array <ScalarArray, Stereo_NBR_ELT>
			         _val_arr {};
		std::array <T, Stereo_NBR_ELT>
		            _mul_arr {};
		bool        _dirty_flag = true;
	};

	void           update_internal_parameters ();
	void           update_lfos ();
	void           update_lfo_freq ();

	fstb_FORCEINLINE static std::array <T, LatticeReverb <T, N, MAXDT>::_nbr_chn>
	               combine_spread (T pan, T mul);

	float          _sample_freq = 0.f;  // Hz, > 0. 0 = not set
	float          _inv_fs      = 0.f;

	// Parameters
	ParamCellSt    _time;
	ParamCellSt    _fdbk_apf;
	ParamCellSt    _fdbk_ltc;

	T              _lfo_freq { 0.1f }; // Hz
	ScalarArray    _lfo_depth {};

	// Smoothed values
	Smoother       _cross_fdbk;
	Smoother       _stereo_spread;

	LpfArrayS      _time_lfo_smth;      // Runs at _sample_freq / _lfo_update_per
	SmootherArrayS _time_smth;
	SmootherArrayS _fdbk_apf_smth;
	SmootherArrayS _fdbk_ltc_smth;
	SmootherArray  _lpf_coef_smth;

	LtcSt          _lattice_st;
	fstb::RndXoroshiro128p
	               _rnd_gen;
	int            _block_pos = 0;      // [0 ; _lfo_update_per[



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

#if 0
	               LatticeReverb ()                               = delete;
	               LatticeReverb (const LatticeReverb &other)     = delete;
	               LatticeReverb (LatticeReverb &&other)          = delete;
	LatticeReverb &
	               operator = (const LatticeReverb &other)        = delete;
	LatticeReverb &
	               operator = (LatticeReverb &&other)             = delete;
#endif
	bool           operator == (const LatticeReverb &other) const = delete;
	bool           operator != (const LatticeReverb &other) const = delete;

}; // class LatticeReverb



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/ltc/LatticeReverb.hpp"



#endif   // mfx_dsp_spat_ltc_LatticeReverb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
