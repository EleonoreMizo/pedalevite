/*****************************************************************************

        PhasedVoice.h
        Author: Laurent de Soras, 2016

Objects of this class must be aligned on 16-byte boundaries

Ref:

Scott Wardle, A Hilbert-Transformer Frequency Shifter for Audio,
First Workshop on Digital Audio Effects DAFx, 1998

Fabian Esqueda, Vesa Valimaki, Julian Parker, Barberpole Phasing and Flanging
Illusions, 18th Int. Conference on Digital Audio Effect (DAFx), 2015

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase1_PhasedVoice_HEADER_INCLUDED)
#define mfx_pi_phase1_PhasedVoice_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/iir/AllPass1pChain.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/osc/OscSinCosEulerSimd.h"
#include "mfx/pi/phase1/Cst.h"
#include "mfx/pi/phase1/Phase90Interface.h"
#include "mfx/pi/phase1/Phase90Simd.h"

#include <memory>



namespace mfx
{
namespace pi
{
namespace phase1
{



class PhasedVoice
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Buf
	{
		Buf_C = 0,
		Buf_S,
		Buf_FC_C,
		Buf_FC_S,

		Buf_NBR_ELT
	};

	static const int  _nbr_chn_out = 2;
	static const float
	                  _bpf_q_threshold;

	               PhasedVoice ();

	void           reset (float sample_freq, int max_buf_len, float *tmp_buf_ptr);

	void           set_polarity (bool neg_flag);
	void           set_speed (float speed);
	void           set_depth (int depth);
	void           set_fdbk_level (float lvl);
	void           set_fdbk_color (float col);
	void           set_phase (float phase);
	void           set_phase_shift (float phase);
	void           set_hold (bool hold_flag);
	void           set_bpf_cutoff (float freq);
	void           set_bpf_q (float q);
	void           set_dist (bool dist_flag);
	void           set_ap_delay (float dly);
	void           set_ap_coef (float coef);

	void           clear_buffers ();
	void           process_block (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::osc::OscSinCosEulerSimd Osc;

	float *        use_buf (Buf buf);
	void           setup_phase_filters ();
	void           update_phase_osc_step ();
	void           update_fdbk_color ();
	void           update_hold_phase ();
	void           update_osc_phase (int nbr_spl);
	void           update_bpf ();
	void           update_apf ();
	void           process_osc (int nbr_spl);
	void           process_osc_fdbk_col (int nbr_spl);
	void           process_block_cfc (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl);
	void           process_block_vfc (float * const dst_ptr_arr [_nbr_chn_out], const float src_ptr [], int nbr_spl);
	void           process_sample (float &dst_l, float &dst_r, float src, float cos_a, float sin_a);
	void           process_block_fx (float * const dst_ptr_arr [_nbr_chn_out], int nbr_spl);
	void           process_block_dist_mono (float dst_ptr [], int nbr_spl);

	static constexpr float
	               clip_feedback (float x);

	float          _sample_freq;        // Hz, > 0
	float          _inv_fs;             // 1 / _sample_freq
	int            _depth;              // [0 ; Cst::_nbr_phase_filters-1]
	float          _osc_freq;           // > 0, Hz. Oscillation frequency in Hz.
	bool           _osc_hold_flag;      // false = oscillator + manual, true = manual only
	bool           _osc_neg_flag;       // Negative phase increment
	float          _fdbk_level;         // [0 ; 1], multiplier
	float          _fdbk_color;         // [0 ; 1], multiplier. Should ramp when changed.
	float          _fdbk_color_old;     // For ramping
	float          _apfd_delay;         // All-pass filter chain delay, s, >= 0
	float          _apfd_coef;          // Global coefficient for the all-pass chain, [-1...1]

	float          _phase_osc_cur;      // [0 ; 2*PI[, rad
	float          _phase_osc_step;     // > 0, rad/sample. Not affected by Hold Oscillator parameter
	float          _phase_man;          // rad, manual phase
	float          _phase_ref;          // rad, absolute phase referencer
	float          _phase_man_cur;      // rad, sum of _phase_man and _phase_ref
	float          _phase_man_old;      // rad
	float          _phase_hold_cur;     // rad
	float          _phase_hold_old;     // rad
	float          _fdbk_lvl_cur;       // [0 ; 1], multiplier
	float          _fdbk_lvl_old;       // [0 ; 1], multiplier
	float          _fdbk_buf;	         // Feedback value between to processed samples
	float          _fdbk_col_c;         // Current feedback color. Used relatively to
	float          _fdbk_col_s;         // _osc_phase_pos_* as an offset. Real values
	float          _bpf_freq;
	float          _bpf_q;
	bool           _osc_freq_update_flag;  // Freq changes, hold changes, manual or osc phases change, after manual or osc phases changed (to set the oscilltor to its real frequency)
	bool           _hold_param_changed_flag;
	bool				_hold_phase_chanded_flag;
	bool           _bpf_changed_flag;
	bool           _bpf_flag;
	bool           _dist_flag;
	bool           _apf_changed_flag;

	Phase90Simd < 4>
	               _phase_filter_0;
	Phase90Simd < 8>
	               _phase_filter_1;
	Phase90Simd <16>
	               _phase_filter_2;
	Phase90Simd <32>
	               _phase_filter_3;
	std::array <Phase90Interface *, Cst::_nbr_phase_filters>
	               _phase_filter_list;
	Phase90Interface *                  // Current phase filter. 0 = not initialized
	               _phase_filter_ptr;
	Osc            _osc_phase;          // Set to 0 Hz in hold mode.
	Osc            _fdbk_color_ramper;
	float *        _tmp_buf_ptr;
	int            _max_buf_len;
	int            _buf_size;
	std::array <dsp::iir::Biquad, _nbr_chn_out>
	               _bpf_arr;

	dsp::iir::AllPass1pChain
	               _apf_delay;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PhasedVoice (const PhasedVoice &other)       = delete;
	               PhasedVoice (const PhasedVoice &&other)      = delete;
	PhasedVoice &  operator = (const PhasedVoice &other)        = delete;
	PhasedVoice &  operator = (const PhasedVoice &&other)       = delete;
	bool           operator == (const PhasedVoice &other) const = delete;
	bool           operator != (const PhasedVoice &other) const = delete;

}; // class PhasedVoice



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase1/PhasedVoice.hpp"



#endif   // mfx_pi_phase1_PhasedVoice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
