/*****************************************************************************

        ReverbDattorro.h
        Author: Laurent de Soras, 2020

Based on:
Jon Dattorro, Effect Design, Part 1: Reverberator and Other Filters,
Journal of the Audio Engineering Society, Vol. 45, No 9, pp. 660-684, 1997-09

TO DO:

- Crossfading when reaching an edge of the LFO shape to smooth the
	discontinuity

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_ReverbDattorro_HEADER_INCLUDED)
#define mfx_dsp_spat_ReverbDattorro_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/RndXoroshiro128p.h"
#include "mfx/dsp/ctrl/SmootherLpf.h"
#include "mfx/dsp/dly/DelaySimple.h"
#include "mfx/dsp/spat/ApfLine.h"

#include <array>
#include <utility>



namespace mfx
{
namespace dsp
{
namespace spat
{



class ReverbDattorro
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int    _nbr_chn       = 2;
	static constexpr float  _min_room_size = 0.0025f; // As a ratio of the original size
	static constexpr float  _max_room_size = 4.f;

	void           set_sample_freq (double sample_freq);

	void           set_room_size (float sz) noexcept;
	void           set_decay (float decay) noexcept;
	void           set_shimmer_pitch (float cents, bool all_flag) noexcept;
	void           set_diffusion_input (float amount) noexcept;
	void           set_diffusion_tank (float amount) noexcept;
	void           set_filter_input_bp (float lo, float hi) noexcept;
	void           set_filter_input_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m) noexcept;
	void           set_filter_tank_bp (float lo, float hi) noexcept;
	void           set_filter_tank_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m, bool override_freeze_flag = false) noexcept;
	void           freeze_tank (bool freeze_flag) noexcept;
	
	std::pair <float, float>
	               process_sample (float xl, float xr) noexcept;
	void           process_block (float dst_l_ptr [], float dst_r_ptr [], const float src_l_ptr [], const float src_r_ptr [], int nbr_spl) noexcept;
	void           flush_tank () noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _max_blk_size    = ApfLine_max_blk_size;
	static constexpr int _nbr_apd_per_chn = 2;
	static constexpr int _nbr_lfo_tot     = _nbr_chn * _nbr_apd_per_chn;
	static constexpr int _nbr_input_apd   = 4; // Number of allpass delays in the input line
	static constexpr int _nbr_taps_out    = 7;

	// Default random period, s (note 14)
	// Because we use a random number generator instead of a sine or tri,
	// we have to shorten the period.
	static constexpr float  _rnd_per_base = 1.f / 4;

	// Default LFO period for the pitch shift effect, s
	static constexpr float  _lfo_per_base = 1.f;

	// Maximum random excursion within the delay line, in s
	// Note 14 suggests the equivalent to 8 samples at 29.8 kHz (268 us),
	// but it seems that a longer excursion sounds more diffuse.
	static constexpr float  _rnd_max_depth_s = 0.005f; 

	// Same for the pitch shifting LFO, larger values here
	static constexpr float  _lfo_max_depth_s = 0.030f; 

	class ModDlyState
	{
	public:
		float          _dly_nosz   = 1000;  // Allpass delay base (unsized), samples
		float          _dly_nomod  = 1000;  // Allpass delay base (unmodulated), samples
		int            _dly_max    = 1000;  // Maximum delay, samples
		float          _lfo_val    = 0;     // [-1 ; 1]
		float          _lfo_step   = 0;     // Per sample, >= 0
		int            _rnd_per    = 10000; // Period between 2 random points, samples
		int            _rnd_pos    = 0;     // Within the period, samples
		float          _rnd_step   = 0;     // Per sample
		float          _rnd_val    = 0;     // Samples
	};

	class Channel
	{
	public:
		typedef std::array <ModDlyState, _nbr_apd_per_chn> MdsArray;
		ApfLine <_nbr_input_apd, false, true>
		               _input;
		ApfLine <1, true, true>
		               _tank_1;
		ApfLine <1, true, false>
		               _tank_2;
		MdsArray       _lfo_arr;      // For the all-pass delay of each tank part
		float          _dly_1_nosz = 1000; // Delay time unsized, samples
		float          _dly_2_nosz = 1000;

		std::array <int, _nbr_taps_out> // Offsets in samples for the output taps
		               _tap_dly_arr;
	};
	typedef std::array <Channel, _nbr_chn> ChannelArray;

	class FilterSpec
	{
	public:
		float          _f_lo = 0;     // Hz, > 0. 0 = user-provided coefs
		float          _f_hi = 0;     // Hz, > 0. 0 = user-provided coefs

		float          _g0   = 0;     // SVF coefficients
		float          _g1   = 0;
		float          _g2   = 0;
		float          _v0m  = 1;     // SVF mixing values. Default = neutral
		float          _v1m  = 0;     // (out = in)
		float          _v2m  = 0;
	};

	void           update_diffusion_input () noexcept;
	void           update_diffusion_tank () noexcept;
	void           update_delay_times () noexcept;
	void           compute_update_filter (FilterSpec &spec, void (ReverbDattorro::*set_coefs) (float g0, float g1, float g2, float v0m, float v1m, float v2m)) noexcept;
	void           compute_filter_coef (FilterSpec &spec) const noexcept;
	void           update_filter_input_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m) noexcept;
	void           update_filter_tank_coefs (float g0, float g1, float g2, float v0m, float v1m, float v2m) noexcept;
	void           reset_lfo () noexcept;
	inline void    process_predelay (float &xl, float &xr) noexcept;
	void           process_predelay_block (float dst_l_ptr [], float dst_r_ptr [], const float src_l_ptr [], const float src_r_ptr [], int nbr_spl) noexcept;
	float          process_modulation (ModDlyState &mds) noexcept;
	void           process_modulation_block (int32_t dly_ptr [], ModDlyState &mds, int nbr_spl) noexcept;
	inline void    check_mod_counters (ModDlyState &mds) noexcept;

	float          _sample_freq = 0; // Hz, > 0. 0 = not set
	float          _inv_fs      = 0; // s, > 0. 0 = not set
	float          _lfo_max_depth_spl =  8.f;  // Samples
	float          _min_mod_dly_time  = 44.1f; // Samples

	ChannelArray   _chn_arr;
	float          _decay       = 0.5f;
	float          _diffuse_in  = 1.f;  // [0 ; 1]
	float          _diffuse_tnk = 1.f;  // [0 ; 1]
	float          _room_size   = 1.f;  // Time scale on the delays and allpass, [0.0025 ; 4.0]
	float          _lfo_speed   = 1.f;  // Hz, >= 0
	float          _lfo_depth   = 0.f;  // Samples, positive or negative
	ctrl::SmootherLpf <float>
	               _freeze;

	// Previous output of channel 1 tank 2. The length of this delay, which is
	// _max_blk_size, is subtracted from channel 1 tank 2 delay, so the overall
	// delay is kept constant.
	dly::DelaySimple <float>
	               _state;

	fstb::RndXoroshiro128p           // Random generator for the delay modulation
	               _rnd_gen;
	float          _rnd_depth   = 0;

	FilterSpec     _filt_spec_input;
	FilterSpec     _filt_spec_tank;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ReverbDattorro &other) const = delete;
	bool           operator != (const ReverbDattorro &other) const = delete;

}; // class ReverbDattorro



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/spat/ReverbDattorro.hpp"



#endif   // mfx_dsp_spat_ReverbDattorro_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
