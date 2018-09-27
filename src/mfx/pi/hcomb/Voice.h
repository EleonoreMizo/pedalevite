/*****************************************************************************

        Voice.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_hcomb_Voice_HEADER_INCLUDED)
#define mfx_pi_hcomb_Voice_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dyn/LimiterRms.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/FilterComb.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/rspl/InterpFirMakerLagrange4.h"
#include "mfx/dsp/FilterCascade.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace hcomb
{



class Voice
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn   =    2;
	static const int  _note_freq_min =   20; // Hz
	static const int  _note_freq_max = 2000; // Hz

	               Voice ();
	virtual        ~Voice () = default;

	void           reset (double sample_freq, int max_block_size, float *tmp_buf_ptr);
	void           clear_buffers ();
	void           set_note (float note);
	void           set_decay_rt60 (float t);
	void           set_polarity (bool neg_flag);
	void           set_level (float lvl);
	void           set_filt_freq (float freq);
	void           set_filt_reso (float reso);
	void           set_filt_damp (float damp);
	void           compute_filt_param (float freq, float reso, float damp, float biq_b_z [3], float biq_a_z [3], float shf_b_z [2], float shf_a_z [2]);
	void           set_filt_param (float freq, float reso, float damp, const float biq_b_z [3], const float biq_a_z [3], const float shf_b_z [2], const float shf_a_z [2]);
	void           process_block (float * const dst_ptr_arr [_max_nbr_chn], const float * const src_ptr_arr [_max_nbr_chn], int nbr_spl, int nbr_chn);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::iir::FilterComb <
		dsp::rspl::InterpFirMakerLagrange4 <float>,
		dsp::FilterCascade <
			dsp::iir::Biquad,
			dsp::iir::OnePole,
			dsp::dyn::LimiterRms
		>
	> CombType;
	typedef std::array <CombType, _max_nbr_chn> CombArray;

	void           update_comb ();
	void           update_filter ();
	void           set_filter_eq (const float biq_b_z [3], const float biq_a_z [3], const float shf_b_z [2], const float shf_a_z [2]);
	void           set_next_block ();

	static float   compute_gain (float fdbk);

	// Main parameters
	float          _sample_freq;        // Hz, > 0
	float          _note;               // MIDI note, possibly fractional
	float          _decay_time;         // s, > 0. RT60 (time to decay to -60 dB)
	float          _level;              // Volume, linear
	float          _filt_freq;          // Filter frequency, Hz, > 0
	float          _filt_reso;          // Filter resonance, [0 ; 1]
	float          _filt_damp;          // Filter damping, [0 ; 1]
	int            _max_block_size;     // Samples, > 0
	bool           _neg_flag;           // Feedback polarity. When negative, the internal comb frequency is doubled to keep the same apparent pitch.
	float *        _tmp_buf_ptr;        // Temporary buffer, must be aligned on 16 bytes, contains at least _max_block_size samples rounded up to 4.

	// Internal data
	float          _inv_fs;             // s, > 0
	float          _gain;               // Input gain, to compensate large volumes generated with high feedback amounts. ]0 ; 1]
	float          _feedback;           // [0 ; 1]
	float          _comb_freq;          // > 0
	float          _gain_old;
	float          _feedback_old;
	float          _comb_freq_old;
	bool           _comb_dirty_flag;    // note/decay/polarity
	bool           _filt_dirty_flag;    // freq/reso
	CombArray      _comb_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Voice (const Voice &other)             = delete;
	Voice &        operator = (const Voice &other)        = delete;
	bool           operator == (const Voice &other) const = delete;
	bool           operator != (const Voice &other) const = delete;

}; // class Voice



}  // namespace hcomb
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/hcomb/Voice.hpp"



#endif   // mfx_pi_hcomb_Voice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
