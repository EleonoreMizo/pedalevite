/*****************************************************************************

        ProgSwitcher.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/BufPack.h"
#include "mfx/ProgSwitcher.h"

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ProgSwitcher::ProgSwitcher (const ProcessingContext * &ctx_ptr, WaMsgQueue &queue_to_cmd, BufPack &buf_pack)
:	_ctx_ptr (ctx_ptr)
,	_queue_to_cmd (queue_to_cmd)
,	_buf_pack (buf_pack)
,	_fade_len (64)
,	_prog_switch_mode (doc::ProgSwitchMode::DIRECT)
,	_switch_flag (false)
,	_delayed_ctx_msg_ptr (nullptr)
{
	dsp::mix::Align::setup ();
}



ProgSwitcher::~ProgSwitcher ()
{
	if (_delayed_ctx_msg_ptr != nullptr)
	{
		_queue_to_cmd.enqueue (*_delayed_ctx_msg_ptr);
		_delayed_ctx_msg_ptr = nullptr;
	}
}



void	ProgSwitcher::reset (double sample_freq, int max_block_size)
{
	fstb::unused (max_block_size);
	assert (_delayed_ctx_msg_ptr == nullptr);
	assert (sample_freq > 0);
	assert (max_block_size > 0);

	_prog_switch_mode = doc::ProgSwitchMode::DIRECT;
	_switch_flag      = false;
	_fade_len         = fstb::round_int (sample_freq * 0.0015);
}



bool	ProgSwitcher::frame_beg ()
{
	bool           ctx_update_flag = false;

	if (_prog_switch_mode == doc::ProgSwitchMode::FADE_OUT_IN)
	{
		if (_switch_flag)
		{
			assert (_delayed_ctx_msg_ptr != nullptr);

			std::swap (_ctx_ptr, _delayed_ctx_msg_ptr->_val._content._ctx._ctx_ptr);
			_queue_to_cmd.enqueue (*_delayed_ctx_msg_ptr);

			_delayed_ctx_msg_ptr = 0;
			_switch_flag         = false;
			ctx_update_flag      = true;
		}
		else
		{
			_prog_switch_mode = doc::ProgSwitchMode::DIRECT;
		}
	}

	return ctx_update_flag;
}



void	ProgSwitcher::handle_msg_ctx (WaMsgQueue::CellType &cell)
{
	// At this point we shouldn't have a programmed switch
	assert (! is_ctx_delayed ());

	_prog_switch_mode = cell._val._content._ctx._prog_switch_mode;
	if (_prog_switch_mode == doc::ProgSwitchMode::FADE_OUT_IN)
	{
		_switch_flag         = true;
		_delayed_ctx_msg_ptr = &cell;
	}
	else
	{
		std::swap (_ctx_ptr, cell._val._content._ctx._ctx_ptr);
	}
}



bool	ProgSwitcher::is_ctx_delayed () const
{
	return (   _prog_switch_mode == doc::ProgSwitchMode::FADE_OUT_IN
	        && _switch_flag);
}



void	ProgSwitcher::process_buffers_i (const ProcessingContextNode::Side &side, int nbr_spl)
{
	assert (nbr_spl > 0);

	switch (_prog_switch_mode)
	{
	case doc::ProgSwitchMode::DIRECT:
		// Nothing to do
		break;

	case doc::ProgSwitchMode::FADE_OUT_IN:
		fade_inout_buf (side, nbr_spl, _switch_flag);
		break;

	default:
		assert (false);
		break;
	}

}



/*** To do:
Crossfade mode.
Fading the input is not enough, we should also mix the output with a
fade out version of the extrapolation of the previous output.
We can use LPC: build a filter (16 coef?) from the last known output
samples (buffer them first), filter with LPC the previous buffer, use
the filter output + a time-reverted version of it to feed the inverse
LPC filter and keep the second part. Fade it out and mix.
LPC code:
https://www.dsprelated.com/showthread/comp.dsp/119064-1.php
https://www.dsprelated.com/showthread/comp.dsp/134663-1.php
***/

void	ProgSwitcher::process_buffers_o (const ProcessingContextNode::Side &side, int nbr_spl)
{
	assert (nbr_spl > 0);

	switch (_prog_switch_mode)
	{
	case doc::ProgSwitchMode::DIRECT:
		// Nothing to do
		break;

	case doc::ProgSwitchMode::FADE_OUT_IN:
		fade_inout_buf (side, nbr_spl, _switch_flag);
		break;

	default:
		assert (false);
		break;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ProgSwitcher::fade_inout_buf (const ProcessingContextNode::Side &side, int nbr_spl, bool fadeout_flag)
{
	assert (nbr_spl > 0);

	int            work_len = std::min (_fade_len, nbr_spl);
   int            ofs      = 0;
	float          vol_beg  = 0;
	float          vol_end  = 1;
	if (fadeout_flag)
	{
		ofs      = nbr_spl - work_len;
		ofs     &= ~3;    // Makes sure the pointer will be properly aligned
		work_len = nbr_spl - ofs;
		vol_beg  = 1;
		vol_end  = 0;
	}

	for (int chn = 0; chn < side._nbr_chn_tot; ++chn)
	{
		const int      buf_index = side._buf_arr [chn];
		float *        buf_ptr   = _buf_pack.use (buf_index);
		dsp::mix::Align::scale_1_vlr (buf_ptr + ofs, work_len, vol_beg, vol_end);
	}
}



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
