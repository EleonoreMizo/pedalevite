/*****************************************************************************

        LatticeReverb.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_ltc_LatticeReverb_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_ltc_LatticeReverb_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace ltc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Default parameters are not great.
template <typename T, int N, typename MAXDT>
LatticeReverb <T, N, MAXDT>::LatticeReverb ()
{
	for (int idx = 0; idx < _depth; ++idx)
	{
		// Base time
#if 0
		constexpr float   tr_lo = 0.50f;
		constexpr float   tr_hi = 0.95f;
		constexpr float   i_scl = 8.f / fstb::sq (float (std::max (_depth - 1, 1)));
		const float       t_rel =
			tr_hi - (tr_hi - tr_lo) / (1.f + i_scl * idx * idx);
#else
		const float       t_rel =
			fstb::ipowpc <4> (0.50f + 0.30f * float (idx) / _depth);
#endif
		const float       t_s   = _time_max * t_rel;
		_time._val_arr [Stereo_BASE  ] [idx]     = T (t_s);
		_time._val_arr [Stereo_SPREAD] [idx]     = T (0.125f);

		_fdbk_apf._val_arr [Stereo_BASE  ] [idx] = T (0.75f);
		_fdbk_apf._val_arr [Stereo_SPREAD] [idx] = T (0.0f);

		_fdbk_ltc._val_arr [Stereo_BASE  ] [idx] = T (-0.25f);
		_fdbk_ltc._val_arr [Stereo_SPREAD] [idx] = T (0.0f);

		_lfo_depth [idx] = T (1.0f);

		_lpf_coef_smth [idx].set_val (T (0.8f));
	}

	_time._mul_arr [Stereo_BASE  ]     = T (1.f);
	_time._mul_arr [Stereo_SPREAD]     = T (1.f);
	_time._dirty_flag                  = true;

	_fdbk_apf._mul_arr [Stereo_BASE  ] = T (1.f);
	_fdbk_apf._mul_arr [Stereo_SPREAD] = T (1.f);
	_fdbk_apf._dirty_flag              = true;

	_fdbk_ltc._mul_arr [Stereo_BASE  ] = T (1.f);
	_fdbk_ltc._mul_arr [Stereo_SPREAD] = T (1.f);
	_fdbk_ltc._dirty_flag              = true;

	_lfo_freq  = 0.01f;

	_cross_fdbk.set_val (T (0.125f));
	_stereo_spread.set_val (T (0.5f));

	_rnd_gen.set_seed (); // Uses default seed values

	_block_pos = 0;
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	_sample_freq = float (      sample_freq);
	_inv_fs      = float (1.0 / sample_freq);

	const int      delay_max_spl = fstb::ceil_int (sample_freq * _time_max);

	_cross_fdbk.set_sample_freq (sample_freq);
	_stereo_spread.set_sample_freq (sample_freq);
	for (int idx = 0; idx < _depth; ++idx)
	{
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			_time_smth [chn] [idx].set_sample_freq (sample_freq);
			_fdbk_apf_smth [chn] [idx].set_sample_freq (sample_freq);
			_fdbk_ltc_smth [chn] [idx].set_sample_freq (sample_freq);

			DelayApfLpf &  apflpf = _lattice_st.use_delay (chn, idx);
			apflpf.use_proc_1 ().set_max_len (delay_max_spl);
		}

		_lpf_coef_smth [idx].set_sample_freq (sample_freq);
	}

	update_lfo_freq ();
}



// Sets the time constant for the LPF on the parameter changes
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_smooth_time (float t) noexcept
{
	assert (_sample_freq > 0);
	assert (t >= _time_min);

	_cross_fdbk.set_time (t);
	_stereo_spread.set_time (t);
	for (int idx = 0; idx < _depth; ++idx)
	{
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			_time_smth [chn] [idx].set_time (t);
			_fdbk_apf_smth [chn] [idx].set_time (t);
			_fdbk_ltc_smth [chn] [idx].set_time (t);
		}
		_lpf_coef_smth [idx].set_time (t);
	}
}



// 0 = independant channels
// 1 = feedbacks are completely crossed (and inverted)
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_cross_feedback (float cf) noexcept
{
	assert (cf >= 0);
	assert (cf <= 1);

	_cross_fdbk.set_val (cf);
}



// 0   = mono output
// 0.5 = standard stereo
// 1   = 200 % wide
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_stereo_spread (float spread) noexcept
{
	assert (spread >= 0);
	assert (spread <= 1);

	_stereo_spread.set_val (spread);
}



// Global multiplier on the time values
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_time_mul (Stereo s, float m) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (s != Stereo_BASE   || m >= 0);
	assert (s != Stereo_BASE   || m <= 1);
	assert (s != Stereo_SPREAD || m >= -1);
	assert (s != Stereo_SPREAD || m <= +1);

	_time._mul_arr [s] = m;
	// Changes will be taken into account at the next block beginning
}



// Global multiplier on the all-pass filter values
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_fdbk_apf_mul (Stereo s, float m) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (s != Stereo_BASE   || m >= 0);
	assert (s != Stereo_BASE   || m <= 1);
	assert (s != Stereo_SPREAD || m >= -1);
	assert (s != Stereo_SPREAD || m <= +1);

	_fdbk_apf._mul_arr [s] = m;
	_fdbk_apf._dirty_flag  = true;
}



// Global multiplier on the lattice coefficients
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_fdbk_ltc_mul (Stereo s, float m) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (s != Stereo_BASE   || m >= 0);
	assert (s != Stereo_BASE   || m <= 1);
	assert (s != Stereo_SPREAD || m >= -1);
	assert (s != Stereo_SPREAD || m <= +1);

	_fdbk_ltc._mul_arr [s] = m;
	_fdbk_ltc._dirty_flag  = true;
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_time (Stereo s, int idx, float t) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (idx >= 0);
	assert (idx < _depth);
	assert (s != Stereo_BASE   || t >= _time_min);
	assert (s != Stereo_BASE   || t <= _time_max);
	assert (s != Stereo_SPREAD || t >= -1);
	assert (s != Stereo_SPREAD || t <= +1);

	_time._val_arr [s] [idx] = t;
	// Changes will be taken into account at the next block beginning
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_fdbk_apf (Stereo s, int idx, float c) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (idx >= 0);
	assert (idx < _depth);
	assert (s != Stereo_BASE   || c >= -1);
	assert (s != Stereo_BASE   || c <= +1);
	assert (s != Stereo_SPREAD || c >= -1);
	assert (s != Stereo_SPREAD || c <= +1);

	_fdbk_apf._val_arr [s] [idx] = c;
	_fdbk_apf._dirty_flag        = true;
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_fdbk_ltc (Stereo s, int idx, float c) noexcept
{
	assert (s >= 0);
	assert (s < Stereo_NBR_ELT);
	assert (idx >= 0);
	assert (idx < _depth);
	assert (s != Stereo_BASE   || c >= -1);
	assert (s != Stereo_BASE   || c <= +1);
	assert (s != Stereo_SPREAD || c >= -1);
	assert (s != Stereo_SPREAD || c <= +1);

	_fdbk_ltc._val_arr [s] [idx] = c;
	_fdbk_ltc._dirty_flag        = true;
}



// Damping coefficient for the LPF within the all-pass delays
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_damping (int idx, float d) noexcept
{
	assert (idx >= 0);
	assert (idx < _depth);
	assert (d >= 0);
	assert (d < 1);

	_lpf_coef_smth [idx].set_val (1 - d);
}



// The LFO modulates the time of the delay lines to improve the diffusion.
template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_lfo_freq (float f) noexcept
{
	assert (f > 0);

	_lfo_freq = f;
	update_lfo_freq ();
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::set_lfo_depth (int idx, float t) noexcept
{
	assert (idx >= 0);
	assert (idx < _depth);
	assert (t >= 0);

	_lfo_depth [idx] = t;
}



template <typename T, int N, typename MAXDT>
std::pair <T, T>	LatticeReverb <T, N, MAXDT>::process_sample (T xl, T xr) noexcept
{
	-- _block_pos;
	if (_block_pos <= 0)
	{
		update_internal_parameters ();
		_block_pos = _lfo_update_per;
	}

	// Parameter smoothing
	for (int idx = 0; idx < _depth; ++idx)
	{
		const T        v_lpf = _lpf_coef_smth [idx].process_sample ();

		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			const T        v_time = _time_smth [chn] [idx].process_sample ();
			const T        v_apf  = _fdbk_apf_smth [chn] [idx].process_sample ();
			const T        v_ltc  = _fdbk_ltc_smth [chn] [idx].process_sample ();

			DelayApfLpf &  combo  = _lattice_st.use_delay (chn, idx);
			DelayApf &     apf    = combo.use_proc_1 ();
			Lpf &          lpf    = combo.use_proc_2 ();

			apf.set_delay_flt (v_time);
			apf.set_coef (v_apf);
			lpf.set_coef (v_lpf);
			_lattice_st.set_coef (chn, idx, v_ltc);
		}
	}

	const T        cross_fdbk = _cross_fdbk.process_sample ();
	_lattice_st.set_cross_feedback (cross_fdbk);

	const T        spread = _stereo_spread.process_sample ();

	// Audio processing
	std::pair <T, T>  yp { _lattice_st.process_sample (xl, xr) };
	const T        mid  = yp.first + yp.second;
	const T        side = yp.first - yp.second;
	yp.first  = mid - spread * (mid - side);
	yp.second = mid - spread * (mid + side);

	return yp;
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::clear_buffers () noexcept
{
	_rnd_gen.set_seed (0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL);
	_block_pos = 0;
	update_internal_parameters ();

	_cross_fdbk.clear_buffers ();
	_stereo_spread.clear_buffers ();

	for (int idx = 0; idx < _depth; ++idx)
	{
		_lpf_coef_smth [idx].clear_buffers ();
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			_time_smth [chn] [idx].clear_buffers ();
			_fdbk_apf_smth [chn] [idx].clear_buffers ();
			_fdbk_ltc_smth [chn] [idx].clear_buffers ();
		}
	}

	_lattice_st.clear_buffers ();
}



template <typename T, int N, typename MAXDT>
constexpr int	LatticeReverb <T, N, MAXDT>::_depth;
template <typename T, int N, typename MAXDT>
constexpr float	LatticeReverb <T, N, MAXDT>::_time_min;
template <typename T, int N, typename MAXDT>
constexpr float	LatticeReverb <T, N, MAXDT>::_time_max;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::update_internal_parameters () noexcept
{
	update_lfos ();

	if (_fdbk_apf._dirty_flag)
	{
		const T        mul_base   = _fdbk_apf._mul_arr [Stereo_BASE  ];
		const T        mul_spread = _fdbk_apf._mul_arr [Stereo_SPREAD];

		for (int idx = 0; idx < _depth; ++idx)
		{
			const T        val_base   = _fdbk_apf._val_arr [Stereo_BASE  ] [idx];
			const T        val_spread = _fdbk_apf._val_arr [Stereo_SPREAD] [idx];
			const auto     spread_lr {
				combine_spread (val_spread, mul_spread)
			};
			const T        val_com = mul_base * val_base;
			_fdbk_apf_smth [0] [idx].set_val (spread_lr [0] * val_com);
			_fdbk_apf_smth [1] [idx].set_val (spread_lr [1] * val_com);
		}

		_fdbk_apf._dirty_flag = false;
	}

	if (_fdbk_ltc._dirty_flag)
	{
		const T        mul_base   = _fdbk_ltc._mul_arr [Stereo_BASE  ];
		const T        mul_spread = _fdbk_ltc._mul_arr [Stereo_SPREAD];

		for (int idx = 0; idx < _depth; ++idx)
		{
			const T        val_base   = _fdbk_ltc._val_arr [Stereo_BASE  ] [idx];
			const T        val_spread = _fdbk_ltc._val_arr [Stereo_SPREAD] [idx];
			const auto     spread_lr {
				combine_spread (val_spread, mul_spread)
			};
			const T        val_com = mul_base * val_base;
			_fdbk_ltc_smth [0] [idx].set_val (spread_lr [0] * val_com);
			_fdbk_ltc_smth [1] [idx].set_val (spread_lr [1] * val_com);
		}

		_fdbk_ltc._dirty_flag = false;
	}
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::update_lfos () noexcept
{
	const T        time_mul_base   = _time._mul_arr [Stereo_BASE  ];
	const T        time_mul_spread = _time._mul_arr [Stereo_SPREAD];

	for (int idx = 0; idx < _depth; ++idx)
	{
		const T        time_base   = _time._val_arr [Stereo_BASE  ] [idx];
		const T        time_spread = _time._val_arr [Stereo_SPREAD] [idx];
		const auto     time_spread_lr {
			combine_spread (time_spread, time_mul_spread)
		};
		const T        time_lfo_depth = _lfo_depth [idx];

		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			const float    rnd_val  = _rnd_gen.gen_flt ()/* - 0.5f*/;
			const T        lfo_val  =
				_time_lfo_smth [chn] [idx].process_sample (T (rnd_val));

			const T        time_s   = fstb::limit (
				  time_spread_lr [chn] * time_mul_base * time_base
				+ time_lfo_depth * lfo_val,
				T (_time_min),
				T (_time_max)
			);
			const T        time_spl = time_s * _sample_freq;
			_time_smth [chn] [idx].set_val (time_spl);
		}
	}
}



template <typename T, int N, typename MAXDT>
void	LatticeReverb <T, N, MAXDT>::update_lfo_freq () noexcept
{
	assert (_sample_freq > 0);

	const float    p = _sample_freq / (_lfo_freq * _lfo_update_per);
	const float    u = 1.f / (0.501f + p);
	const float    c = std::min (u, 1.f);

	for (auto &chn : _time_lfo_smth)
	{
		for (auto &lpf : chn)
		{
			lpf.set_coef (c);
		}
	}
}



// Combines a specific stereo spread value with a global multiplier
// pan = -1 -> { 1 - mul, 0       }
// pan =  0 -> { 1      , 1       }
// pan = +1 -> { 0      , 1 - mul }
// Result in [0 ; 2]
template <typename T, int N, typename MAXDT>
inline std::array <T, LatticeReverb <T, N, MAXDT>::_nbr_chn>	LatticeReverb <T, N, MAXDT>::combine_spread (T pan, T mul) noexcept
{
	assert (pan >= -1.f);
	assert (pan <= +1.f);
	assert (mul >= -1.f);
	assert (mul <= +1.f);

	const T        premul = mul * pan;
	T              l      = 1.f;
	T              r      = 1.f;
	if (pan < 0)
	{
		l += premul;
	}
	else
	{
		r -= premul;
	}

	return { l, r };
}



}  // namespace ltc
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_ltc_LatticeReverb_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
