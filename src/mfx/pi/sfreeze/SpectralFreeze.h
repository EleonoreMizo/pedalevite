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

#include "ffft/FFTRealFixLen.h"
#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/spec/FrameOverlapAna.h"
#include "mfx/dsp/spec/FrameOverlapSyn.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/pi/sfreeze/Cst.h"
#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



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

	               SpectralFreeze ();
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
	static constexpr int _fft_len_l2 = 12;
	static constexpr int _fft_len    = 1 << _fft_len_l2;

	// Range of "useful" bins. DC is 0 and Nyquist is _bin_end
	static constexpr int _nbr_bins   = _fft_len / 2;
	static constexpr int _bin_beg    = 1;
	static constexpr int _bin_end    = _nbr_bins;

	// Base-2 log of the overlap, in samples. Must be <= _fft_len_l2
	static constexpr int _hop_size_l2 = _fft_len_l2 - 2;
	static constexpr int _hop_size    = 1 << _hop_size_l2;

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
		std::array <float, _fft_len>
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
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	int            conv_freq_to_bin (float f) const noexcept;
	void           analyse_bins (Channel &chn) noexcept;
	void           analyse_capture1 (Slot &slot) noexcept;
	void           analyse_capture2 (Slot &slot) noexcept;
	void           synthesise_bins (Channel &chn) noexcept;
	void           synthesise_playback (Slot &slot, float gain) noexcept;

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

	ChannelArray   _chn_arr;

	ffft::FFTRealFixLen <_fft_len_l2>
	               _fft;
	dsp::wnd::ProcHann <float, (_fft_len_l2 - _hop_size_l2 == 1)>
	               _frame_win;
	std::array <float, _fft_len>
	               _buf_pcm;
	std::array <float, _fft_len>
	               _buf_bins;

	// Crossfading position, [0 ; Cst::_nbr_slots]. Integer = pure slot.
	// Wraps to 0 for Cst::_nbr_slots
	float          _xfade_pos  = 0;

	// Linear volume for the crossfade result
	float          _xfade_gain = 0;

	// Cycles/hop
	float          _phasing    = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpectralFreeze (const SpectralFreeze &other)   = delete;
	               SpectralFreeze (SpectralFreeze &&other)        = delete;
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
