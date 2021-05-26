/*****************************************************************************

        VelvetFreeze.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_vfreeze_VelvetFreeze_HEADER_INCLUDED)
#define mfx_pi_vfreeze_VelvetFreeze_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/grn/VelvetConv.h"
#include "mfx/dsp/wnd/ProcHann.h"
#include "mfx/pi/vfreeze/Cst.h"
#include "mfx/pi/vfreeze/DMode.h"
#include "mfx/pi/vfreeze/VelvetFreezeDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace vfreeze
{



class VelvetFreeze final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       VelvetFreeze (piapi::HostInterface &host);
	               ~VelvetFreeze () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// For volume compensation
	static constexpr int _max_block_size = 64;

	enum class FreezeState
	{
		NONE = 0,
		CAPTURE,
		REPLAY
	};

	class Slot
	{
	public:
		// Current freeze state for the channel
		FreezeState    _frz_state  = FreezeState::NONE;

		// Stored grain RMS volume, for reference
		float          _grain_lvl  = 0;

		// Grain generator
		dsp::grn::VelvetConv <float>
							_grain_gen;

		// Envelope detector, to track the output volume
		dsp::dyn::EnvFollowerRms
		               _env;

		// Current gain for the slot
		float          _gain       = 1;

		// Final gain, after volume compensation.
		float          _gain_final = 0;
	};
	typedef std::array <Slot, Cst::_nbr_slots> SlotArray;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		SlotArray      _slot_arr;

		// Not the real volume actually, more a position within a fade curve.
		dsp::ctrl::Ramp
		               _vol_dry { 1.f };
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           capture_slot (Slot &slot, int nbr_spl) noexcept;
	void           synthesise_channel (Channel &chn, float dst_ptr [], int nbr_spl) noexcept;
	void           synthesise_slot (Slot &slot, float dst_ptr [], bool mix_flag, float gain, int nbr_spl) noexcept;
	void           check_dry_level (Channel &chn) noexcept;

	static float   conv_pos_to_dry_lvl (float x) noexcept;

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	VelvetFreezeDesc
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

	// Ring buffer to capture the audio input
	BufAlign       _buf_capture;
	int            _buf_len = 0;
	int            _buf_msk = 0;
	int            _buf_pos = 0;

	// Uses:
	// - Volume analysis and windowing and for a captured grain,
	// - Grain rendering
	BufAlign       _buf_tmp;     

	dsp::wnd::ProcHann <float>
	               _grain_win;

	// Crossfading position, [0 ; Cst::_nbr_slots]. Integer = pure slot.
	// Wraps to 0 for Cst::_nbr_slots
	float          _xfade_pos  = 0;

	// Linear volume for the crossfade result
	float          _xfade_gain = 0;

	// Playback mode
	DMode          _dry_mode   = DMode_MIX;

	// Grain duration, s, > 0
	float          _grain_dur  = 0;

	// Grain length in samples, > 0
	int            _grain_len = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               VelvetFreeze ()                               = delete;
	               VelvetFreeze (const VelvetFreeze &other)      = delete;
	               VelvetFreeze (VelvetFreeze &&other)           = delete;
	VelvetFreeze & operator = (const VelvetFreeze &other)        = delete;
	VelvetFreeze & operator = (VelvetFreeze &&other)             = delete;
	bool           operator == (const VelvetFreeze &other) const = delete;
	bool           operator != (const VelvetFreeze &other) const = delete;

}; // class VelvetFreeze



}  // namespace vfreeze
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/vfreeze/VelvetFreeze.hpp"



#endif   // mfx_pi_vfreeze_VelvetFreeze_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
