/*****************************************************************************

        SpectralFreeze.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_sfreeze_SpectralFreeze_HEADER_INCLUDED)
#define mfx_pi_sfreeze_SpectralFreeze_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/spec/FrameOverlapAna.h"
#include "mfx/dsp/spec/FrameOverlapSyn.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/pi/cdsp/FftParam.h"
#include "mfx/pi/cdsp/FFTRealRange.h"
#include "mfx/pi/sfreeze/Cst.h"
#include "mfx/pi/sfreeze/DMode.h"
#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
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
namespace sfreeze
{



class SpectralFreeze final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SpectralFreeze (piapi::HostInterface &host);
	               ~SpectralFreeze () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Base-2 log of the FFT length, in samples
	// _fft_len_l2_min is used at standard rates (< 50 kHz)
	static constexpr int _fft_len_l2_min = 12;
	static constexpr int _fft_len_l2_max = 16;

	// Radius for the crystalise effect, in bins.
	static constexpr int _cryst_rad = 16;

	// Offset of the 0th bin in _buf_pcm when used to store the bin magnitudes
	// The offset is required in order to provide some margins to avoid doing
	// tests for boundaries during analysis pass for the Crystalise effect.
	static constexpr int _cryst_ofs = _cryst_rad;

#if defined (fstb_HAS_SIMD)
	static constexpr int _simd_w = 4;
#endif

	enum class FreezeState
	{
		NONE = 0,
		CAPTURE1,
		CAPTURE2,
		REPLAY
	};

	class Slot
	{
	public:
		// Current freeze state for the channel
		FreezeState    _frz_state   = FreezeState::NONE;

		// Frozen data, stored as bins. Different content after each pass:
		// CAPTURE1: magnitude-normalised bins of the first frame
		// CAPTURE2: real = magnitude of the frozen bin, imag = normalised angle
		// (1 is 2 * pi rad) of the phase difference between the two frames
		// Bins 0 and _nbr_bins are never stored.
		// Length: _fft_len
		std::vector <float>
							_buf_freeze;

		// Number of hops elapsed since the freeze beginning. Helps to adjust
		// the phase of each bin.
		int            _nbr_hops    = 0;

		// Accumulated phase for the phasing effect
		float          _phase_acc   = 0;

		// Current gain for the slot
		float          _gain        = 1;
	};
	typedef std::array <Slot, Cst::_nbr_slots> SlotArray;

	class Channel
	{
	public:
		// Analysis and synthesis overlappers
		dsp::spec::FrameOverlapAna <float>
		               _fo_ana;
		dsp::spec::FrameOverlapSyn <float>
		               _fo_syn;

		SlotArray      _slot_arr;

		// Not the real volume actually, more a position within a fade curve.
		dsp::ctrl::Ramp
		               _vol_dry { 1.f };
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           compute_fft_var (double sample_freq) noexcept;
	int            conv_freq_to_bin (float f) const noexcept;
	void           analyse_bins (Channel &chn) noexcept;
	void           analyse_capture1 (Slot &slot) noexcept;
	void           analyse_capture2 (Slot &slot) noexcept;
	void           synthesise_bins (Channel &chn) noexcept;
	void           synthesise_playback (Slot &slot, float gain) noexcept;
	void           process_crystalise () noexcept;
	void           crystalise_precomp_mag () noexcept;
	void           crystalise_analyse () noexcept;
	void           crystalise_decimate () noexcept;
	void           check_dry_level (Channel &chn) noexcept;

	static float   conv_pos_to_dry_lvl (float x) noexcept;

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	SpectralFreezeDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs      = 0;    // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	std::array <fstb::util::NotificationFlagCascadeSingle, Cst::_nbr_slots>
	               _param_change_flag_slot_arr;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_misc;

	cdsp::FftParam <_fft_len_l2_min, _fft_len_l2_max>
	               _p;

	// Vector and scalar ranges and indexes
#if defined (fstb_HAS_SIMD)
	int            _bin_end_vec = 0;
	int            _bin_beg_sca = 0;
#else
	int            _bin_beg_sca = _bin_beg;
#endif

	ChannelArray   _chn_arr;

	cdsp::FFTRealRange <_fft_len_l2_min, _fft_len_l2_max>
	               _fft;
	dsp::wnd::ProcHann <float, false>
	               _frame_win;
	std::vector <float>          // Length: _fft_len. Also hosts the precalculated squared magnitudes for the crystalise effect
	               _buf_pcm;
	std::vector <float>          // Length: _fft_len
	               _buf_bins;
	std::vector <int32_t>        // Length: _nbr_bins. Only [_bin_beg ; bin_end[ range is valid
	               _weight_arr;

	// Crossfading position, [0 ; Cst::_nbr_slots]. Integer = pure slot.
	// Wraps to 0 for Cst::_nbr_slots
	float          _xfade_pos  = 0;

	// Linear volume for the crossfade result
	float          _xfade_gain = 0;

	// Cycles/hop
	float          _phasing    = 0;

	// Playback mode
	DMode          _dry_mode   = DMode_MIX;

	float          _cryst_amt  = 0; // Crystalise amount, [0 ; 1]. 0 = disabled.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpectralFreeze ()                               = delete;
	               SpectralFreeze (const SpectralFreeze &other)    = delete;
	               SpectralFreeze (SpectralFreeze &&other)         = delete;
	SpectralFreeze &
	               operator = (const SpectralFreeze &other)        = delete;
	SpectralFreeze &
	               operator = (SpectralFreeze &&other)             = delete;
	bool           operator == (const SpectralFreeze &other) const = delete;
	bool           operator != (const SpectralFreeze &other) const = delete;

}; // class SpectralFreeze



}  // namespace sfreeze
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/sfreeze/SpectralFreeze.hpp"



#endif   // mfx_pi_sfreeze_SpectralFreeze_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
