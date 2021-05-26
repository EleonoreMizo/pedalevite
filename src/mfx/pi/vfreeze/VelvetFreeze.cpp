/*****************************************************************************

        VelvetFreeze.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/wnd/XFadeEqPowPoly8.h"
#include "mfx/pi/vfreeze/Param.h"
#include "mfx/pi/vfreeze/VelvetFreeze.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace vfreeze
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VelvetFreeze::VelvetFreeze (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();
	dsp::mix::Generic::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		const int      base = Param_SLOT_BASE + slot_idx * ParamSlot_NBR_ELT;
		auto &         pcf  = _param_change_flag_slot_arr [slot_idx];

		_state_set.set_val_nat (desc_set, base + ParamSlot_FREEZE, 0);
		_state_set.set_val_nat (desc_set, base + ParamSlot_GAIN  , 1);

		_state_set.add_observer (base + ParamSlot_FREEZE, pcf);
		_state_set.add_observer (base + ParamSlot_GAIN  , pcf);

		pcf.add_observer (_param_change_flag);
	}

	_state_set.set_val_nat (desc_set, Param_XFADE  ,  0);
	_state_set.set_val_nat (desc_set, Param_XFGAIN ,  0);
	_state_set.set_val_nat (desc_set, Param_DMODE  ,  0);
	_state_set.set_val_nat (desc_set, Param_GSIZE  ,  0.250);
	_state_set.set_val_nat (desc_set, Param_DENSITY, 32);

	_state_set.add_observer (Param_XFADE  , _param_change_flag_misc);
	_state_set.add_observer (Param_XFGAIN , _param_change_flag_misc);
	_state_set.add_observer (Param_DMODE  , _param_change_flag_misc);
	_state_set.add_observer (Param_GSIZE  , _param_change_flag_misc);
	_state_set.add_observer (Param_DENSITY, _param_change_flag_misc);

	_param_change_flag_misc.add_observer (_param_change_flag);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	VelvetFreeze::do_get_state () const
{
	return _state;
}



double	VelvetFreeze::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	VelvetFreeze::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      max_grain_len = fstb::ceil_int (
		(Cst::_max_grain_size_ms / 1000.0) * sample_freq
	);

	_buf_len = 1 << fstb::get_next_pow_2 (max_grain_len);
	_buf_msk = _buf_len - 1;
	_buf_pos = 0;
	_buf_capture.resize (_buf_len);

	const int      mbl_align    = (max_buf_len + 3) & ~3;
	const int      buf_tmp_size = std::max (mbl_align, max_grain_len);
	_buf_tmp.resize (buf_tmp_size);

	constexpr float   env_time = 0.030f;
	for (auto &chn : _chn_arr)
	{
		for (auto &slot : chn._slot_arr)
		{
			slot._frz_state = FreezeState::NONE;
			slot._grain_lvl = 0;
			slot._grain_gen.reserve (max_grain_len);
			slot._env.set_sample_freq (sample_freq);
			slot._env.set_times (env_time, env_time);
		}
	}

	for (auto &pcf : _param_change_flag_slot_arr)
	{
		pcf.set ();
	}

	update_param (true);
	_param_proc.req_steady ();

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	VelvetFreeze::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// Signal processing
	const int      nbr_spl = proc._nbr_spl;
	for (int chn_idx = 0; chn_idx < nbr_chn_proc; ++chn_idx)
	{
		Channel &      chn = _chn_arr [chn_idx];

		// Records the input signal into the capture buffer
		dsp::dly::RingBufVectorizer   rbv (_buf_len);
		const float *  src_ptr = proc._src_arr [chn_idx];
		for (rbv.start (nbr_spl, _buf_pos); rbv.end (); rbv.next ())
		{
			const int      seg_len = rbv.get_seg_len ();
			const int      seg_pos = rbv.get_curs_pos (0);
			fstb::copy_no_overlap (&_buf_capture [seg_pos], src_ptr, seg_len);
			src_ptr += seg_len;
		}

		for (auto &slot : chn._slot_arr)
		{
			// If required, makes a grain from the capture buffer
			if (slot._frz_state == FreezeState::CAPTURE)
			{
				capture_slot (slot, nbr_spl);
				slot._frz_state = FreezeState::REPLAY;
			}
		}

		// Playback
		synthesise_channel (chn, proc._dst_arr [chn_idx], nbr_spl);

		// Mixing
		check_dry_level (chn);
		int            blk_pos = 0;
		do
		{
			const int      work_len =
				std::min (nbr_spl - blk_pos, int (_max_block_size));

			chn._vol_dry.tick (work_len);
			const float    dry_pos_beg = chn._vol_dry.get_beg ();
			const float    dry_pos_end = chn._vol_dry.get_end ();
			const float    dry_lvl_beg = conv_pos_to_dry_lvl (dry_pos_beg);
			const float    dry_lvl_end = conv_pos_to_dry_lvl (dry_pos_end);
			dsp::mix::Generic::mix_1_1_vlrauto (
				proc._dst_arr [chn_idx] + blk_pos,
				proc._src_arr [chn_idx] + blk_pos,
				work_len,
				dry_lvl_beg,
				dry_lvl_end
			);

			blk_pos += work_len;
		}
		while (blk_pos < nbr_spl);
	}

	_buf_pos = (_buf_pos + nbr_spl) & _buf_msk;

	// Duplicates the remaining output channels
	for (int chn_idx = nbr_chn_proc; chn_idx < nbr_chn_dst; ++chn_idx)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_idx],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	VelvetFreeze::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		for (auto &slot : chn._slot_arr)
		{
			slot._env.clear_buffers ();
			slot._grain_gen.clear_buffers ();
			slot._gain_final = 0;
		}
		chn._vol_dry.clear_buffers ();
	}
}



void	VelvetFreeze::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		// Slot parameters
		for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
		{
			const int      base = Param_SLOT_BASE + slot_idx * ParamSlot_NBR_ELT;

			if (_param_change_flag_slot_arr [slot_idx] (true) || force_flag)
			{
				const bool     frz_flag =
					_state_set.get_val_bool (base + ParamSlot_FREEZE);
				const float    gain     =
					float (_state_set.get_val_end_nat (base + ParamSlot_GAIN));

				for (auto &chn : _chn_arr)
				{
					auto &         slot = chn._slot_arr [slot_idx];

					if (! frz_flag)
					{
						slot._frz_state = FreezeState::NONE;
					}
					else if (slot._frz_state == FreezeState::NONE)
					{
						slot._frz_state = FreezeState::CAPTURE;
					}

					slot._gain = gain;
				}
			}
		}

		// Misc
		if (_param_change_flag_misc (true) || force_flag)
		{
			_xfade_pos  = float (_state_set.get_val_end_nat (Param_XFADE ));
			_xfade_gain = float (_state_set.get_val_end_nat (Param_XFGAIN));
			_dry_mode   = _state_set.get_val_enum <DMode> (Param_DMODE);
			const float    grain_dur =
				float (_state_set.get_val_end_nat (Param_GSIZE));
			const float    density   =
				float (_state_set.get_val_end_nat (Param_DENSITY));

			_grain_len  = fstb::round_int (grain_dur * _sample_freq);
			_grain_win.setup (_grain_len);

			const int      vd_len = (_grain_len + 1) >> 1;
			const float    vd_stp = 1.f / vd_len;

			for (auto &chn : _chn_arr)
			{
				for (auto &slot : chn._slot_arr)
				{
					slot._grain_gen.set_density (density);
				}

				chn._vol_dry.set_time (vd_len, vd_stp);
			}
		}
	}
}



void	VelvetFreeze::capture_slot (Slot &slot, int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	// Copy from the ring buffer
	float *        dst_ptr = _buf_tmp.data ();
	const int      pos_rd  =
		(_buf_pos + nbr_spl - _grain_len) & _buf_msk;
	dsp::dly::RingBufVectorizer   rbv (_buf_len);
	for (rbv.start (_grain_len, pos_rd); rbv.end (); rbv.next ())
	{
		const int      seg_len = rbv.get_seg_len ();
		const int      seg_pos = rbv.get_curs_pos (0);
		fstb::copy_no_overlap (dst_ptr, &_buf_capture [seg_pos], seg_len);
		dst_ptr += seg_len;
	}

	// Volume evaluation
	double         sq_sum = 0;
	for (int pos = 0; pos < _grain_len; ++pos)
	{
		sq_sum += fstb::sq (_buf_tmp [pos]);
	}
	slot._grain_lvl = sqrtf (float (sq_sum) / _grain_len);

	// Windowing
	_grain_win.process_frame_mul (_buf_tmp.data ());

	// Done
	slot._grain_gen.set_granule (_buf_tmp.data (), _grain_len, true);
}



void	VelvetFreeze::synthesise_channel (Channel &chn, float dst_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	bool           mix_flag = false;
	for (int slot_idx = 0; slot_idx < Cst::_nbr_slots; ++slot_idx)
	{
		auto &         slot = chn._slot_arr [slot_idx];
		if (slot._frz_state == FreezeState::REPLAY)
		{
			float          gain = slot._gain;

			// 0 = pure slot, >= 1 = silent
			float          xf_pos_rel = std::min (
				fabsf (_xfade_pos - slot_idx),
				fabsf (_xfade_pos - slot_idx - Cst::_nbr_slots)
			);
			if (xf_pos_rel < 1)
			{
#if 1
				// This curve is a bit flat at the top which is great when
				// automating/controlling the crossfade position, so the "pure"
				// slot position is a bit wider and therefore less sensitive to
				// position errors. Calculation complexity is similar to the
				// sine curve.
				const auto     xf_res  =
					dsp::wnd::XFadeEqPowPoly8 <false>::compute_gain (xf_pos_rel);
				const float    gain_xf = xf_res [0];
#else
				// Old version, for reference: sine-based crossfade curve
				const float    gain_xf =
					fstb::Approx::sin_rbj_halfpi (1 - xf_pos_rel);
#endif
				gain += gain_xf * _xfade_gain;
			}

			synthesise_slot (slot, dst_ptr, mix_flag, gain, nbr_spl);
			mix_flag = true;
		}
	}

	if (! mix_flag)
	{
		dsp::mix::Align::clear (dst_ptr, nbr_spl);
	}
}



void	VelvetFreeze::synthesise_slot (Slot &slot, float dst_ptr [], bool mix_flag, float gain, int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (nbr_spl > 0);

	// Maximum fix gain
	constexpr float   max_fix = 16.f;

	// Constant to avoid division by 0
	constexpr float   eps     = 1e-9f;

	slot._grain_gen.process_block (_buf_tmp.data (), nbr_spl);

	int            blk_pos = 0;
	do
	{
		const int      work_len =
			std::min (nbr_spl - blk_pos, int (_max_block_size));
		float *        slot_ptr = &_buf_tmp [blk_pos];
		const float    lvl      =
			slot._env.analyse_block (slot_ptr, work_len);
		const float    fix      =
			std::min (slot._grain_lvl / (lvl + eps), max_fix);
		const float    g_f_new  = gain * fix;

		if (mix_flag)
		{
			dsp::mix::Generic::mix_1_1_vlrauto (
				dst_ptr + blk_pos, slot_ptr, work_len, slot._gain_final, g_f_new
			);
		}
		else
		{
			dsp::mix::Generic::copy_1_1_vlrauto (
				dst_ptr + blk_pos, slot_ptr, work_len, slot._gain_final, g_f_new
			);
		}

		slot._gain_final = g_f_new;
		blk_pos         += work_len;
	}
	while (blk_pos < nbr_spl);
}



void	VelvetFreeze::check_dry_level (Channel &chn) noexcept
{
	bool           play_mode_flag = false;
	for (auto &slot : chn._slot_arr)
	{
		if (slot._frz_state == FreezeState::REPLAY)
		{
			play_mode_flag = true;
			break;
		}
	}

	// Updates the dry level
	float          dry_lvl = 1;
	if (    _dry_mode == DMode_MUTE
	    || (_dry_mode == DMode_CUT  && play_mode_flag))
	{
		dry_lvl = 0;
	}
	if (chn._vol_dry.get_tgt () != dry_lvl)
	{
		chn._vol_dry.set_val (dry_lvl);
	}
}



float	VelvetFreeze::conv_pos_to_dry_lvl (float x) noexcept
{
	assert (x >= 0);
	assert (x <= 1);

	x = 1 - fstb::sq (std::max (1.f - 1.25f * x, 0.f));

	return x;
}



}  // namespace vfreeze
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
