/*****************************************************************************

        Lipidipi.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lipid_Lipidipi_HEADER_INCLUDED)
#define mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/dly/DelayLine.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/rspl/InterpolatorHermite43.h"
#include "mfx/pi/lipid/Cst.h"
#include "mfx/pi/lipid/LipidipiDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace pi
{
namespace lipid
{



class Lipidipi final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Lipidipi (piapi::HostInterface &host);
	               ~Lipidipi () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr double _avg_dly         = 0.012; // s
	static constexpr double _max_depth       = 0.010; // s
	static_assert (_max_depth <= _avg_dly, "");
	static constexpr double _lpf_cutoff_freq = 1.0;   // Hz
	static constexpr int    _seg_len         = 64;    // Linear segment length, samples
	static_assert (fstb::is_pow_2 (_seg_len), "");
	static constexpr int    _seg_msk         = _seg_len - 1;
	static constexpr int    _vc_per_grp      = 4;     // Number of voices per group, > 0
	static constexpr int    _max_voices      = Cst::_max_groups * _vc_per_grp;
	static constexpr float  _f1_hz           =    5.f;
	static constexpr double _f_beg_hz        =  100.0; // Hz
	static constexpr double _f_end_hz        = 3200.0; // Hz

	typedef std::vector <
		float, fstb::AllocAlign <float, fstb_SIMD128_ALIGN>
	> BufAlign;

	class Voice
	{
	public:
		// Random generator state. It is just a counter being hashed.
		uint32_t       _rnd_state;

		// Low-pass filter on the signed rnd result
		std::array <dsp::iir::Biquad, 2>
		               _lpf_arr;

		// Turns the filtered result into a delay value
		float          _dly_scale;

		// Maximum delay change, in second per segment (_seg_len).
		float          _rate_min; // < 0
		float          _rate_max; // > 0

		// Final delay values for the segment, s, >= 0.
		// _seg_pos indicates where we are in the segment.
		float          _delay_beg;
		float          _delay_end;

		// Bandpass filter, for audio
/*** To do:
put the filter in a group structure, and premix the whole group before
filtering it at once.
***/
		dsp::iir::Biquad
		               _bpf;
	};
	typedef std::array <Voice, _max_voices> VoiceArray;

	class Channel
	{
	public:
		dsp::dly::DelayLine
		               _delay;
		VoiceArray     _voice_arr;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers () noexcept;
	void           update_param (bool force_flag = false) noexcept;
	void           start_new_segment () noexcept;

	static uint32_t
	               compute_initial_rnd_state (int chn_idx, int vc_idx) noexcept;

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	LipidipiDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq  = 0;   // Hz, > 0. <= 0: not initialized
	float          _inv_fs       = 0;   // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;

	mfx::dsp::rspl::InterpolatorHermite43
	               _interp;             // Delay interpolator
	BufAlign       _buf_dly;            // Temp buffer for the delay output
	BufAlign       _buf_mix;            // Temp buffer for the delay mix
	float          _min_dly_time = 0;   // s, > 0. 0 = not initialized

	float          _fatness      = 0;

	// Equivalent to ceil (_fatness). The last voice may be faded, depending
	// on the fractional part.
	int            _nbr_groups   = 0;

	// Segment position for the delay ramps. [0 ; _seg_len-1].
	// We compute a new segment when _seg_pos == 0 at the beginning of a block.
	int            _seg_pos      = 0;

	// Overall volume, linear
	dsp::ctrl::Ramp
	               _vol_dry      { 0.5f };
	dsp::ctrl::Ramp
	               _vol_wet      { 0.5f };
	bool           _stereo_flag  = true;

	float          _f_beg_l2     = 0; // log2 (_f_beg_hz / fs)

	static const float
	               _f_rat_l2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Lipidipi ()                               = delete;
	               Lipidipi (const Lipidipi &other)          = delete;
	               Lipidipi (Lipidipi &&other)               = delete;
	Lipidipi &     operator = (const Lipidipi &other)        = delete;
	Lipidipi &     operator = (Lipidipi &&other)             = delete;
	bool           operator == (const Lipidipi &other) const = delete;
	bool           operator != (const Lipidipi &other) const = delete;

}; // class Lipidipi



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lipid/Lipidipi.hpp"



#endif   // mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
