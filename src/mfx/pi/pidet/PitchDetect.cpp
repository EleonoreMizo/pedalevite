/*****************************************************************************

        PitchDetect.cpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/pidet/Param.h"
#include "mfx/pi/pidet/PitchDetect.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"
#include "mfx/piapi/ProcInfo.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace pidet
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PitchDetect::PitchDetect (piapi::HostInterface &host)
:	_host (host)
,	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_param_proc (_state_set)
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_buf_prev_spl ()
,	_nbr_spl_in_buf (0)
,	_sub_spl (1)
,	_analyser ()
,	_freq (0)
,	_last_valid_output (0)
,	_output_type (OutType_PITCH)
,	_buffer ()
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FREQ_MIN,   30);
	_state_set.set_val_nat (desc_set, Param_FREQ_MAX, 1000);

	_state_set.add_observer (Param_FREQ_MIN, _param_change_flag);
	_state_set.add_observer (Param_FREQ_MAX, _param_change_flag);

	_analyser.set_smoothing (0.5f, 0.0f);
	_analyser.set_threshold (1e-4f); /*** To do: make it a parameter ***/
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	PitchDetect::do_get_state () const
{
	return _state;
}



double	PitchDetect::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	PitchDetect::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_buffer.resize (max_buf_len);
	_analyser.set_sample_freq (sample_freq / _sub_spl);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	PitchDetect::do_process_block (piapi::ProcInfo &proc)
{
	// Events
	_param_proc.handle_msg (proc);

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();
	if (_param_proc.is_full_reset ())
	{
		clear_buffers ();
	}

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Signal processing

	if (_sub_spl == 1)
	{
		_freq = _analyser.process_block (proc._src_arr [0], proc._nbr_spl);
	}

	else
	{
		// Downsampling with buffering
		const float *  spl_ptr = proc._src_arr [0];
		const int      nbr_spl = proc._nbr_spl;
		int            pos     = 0;
		int            pos_sub = 0;
		if (_nbr_spl_in_buf > 0)
		{
			if (_nbr_spl_in_buf + nbr_spl - pos >= _sub_spl)
			{
				float          sum     = 0;
				int            buf_pos = 0;
				do
				{
					sum += _buf_prev_spl [buf_pos];
					++ buf_pos;
				}
				while (buf_pos < _nbr_spl_in_buf);
				do
				{
					sum += spl_ptr [pos - _nbr_spl_in_buf + buf_pos];
					++ buf_pos;
				}
				while (buf_pos < _sub_spl);
				_buffer [pos_sub] = sum;
				++ pos_sub;
				pos += _sub_spl - _nbr_spl_in_buf;
				_nbr_spl_in_buf = 0;
			}
			else
			{
				do
				{
					_buf_prev_spl [_nbr_spl_in_buf] = spl_ptr [pos];
					++ _nbr_spl_in_buf;
					++ pos;
				}
				while (pos < nbr_spl);
				assert (_nbr_spl_in_buf < _sub_spl);
			}
		}
		const int      nbr_sub_full = (nbr_spl - pos) / _sub_spl;
		const int      end_sub      = pos_sub + nbr_sub_full;
		if (_sub_spl == 16)
		{
			while (pos_sub < end_sub)
			{
				auto           s0 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos     );
				auto           s1 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos +  4);
				auto           s2 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos +  8);
				auto           s3 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos + 12);
				auto           s  = (s0 + s1) + (s2 + s3);
				_buffer [pos_sub] = fstb::ToolsSimd::sum_h_flt (s);
				pos += 16;
				++ pos_sub;
			}
		}
		else
		{
			assert (_sub_spl == 8);
			while (pos_sub < end_sub)
			{
				auto           s0 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos     );
				auto           s1 = fstb::ToolsSimd::loadu_f32 (spl_ptr + pos +  4);
				auto           s  = s0 + s1;
				_buffer [pos_sub] = fstb::ToolsSimd::sum_h_flt (s);
				pos += 8;
				++ pos_sub;
			}
		}
		if (pos < nbr_spl)
		{
			do
			{
				_buf_prev_spl [_nbr_spl_in_buf] = spl_ptr [pos];
				++ _nbr_spl_in_buf;
				++ pos;
			}
			while (pos < nbr_spl);
			assert (_nbr_spl_in_buf < _sub_spl);
		}

		// Pitch detection
		_freq = _analyser.process_block (&_buffer [0], pos_sub);
	}

	switch (_output_type)
	{
	case OutType_FREQ:
		// Frequency in kHz
		if (_freq > 0)
		{
			_last_valid_output = _freq * 0.001f;
		}
		break;
	case OutType_PITCH:
		// Output: 0 = Middle C, one unit per octave, -10 or below = not found
		{
			float          pitch = -10;
			if (_freq > 0)
			{
				// 220 * 2^0.25
				const float    fmult = 1.0f / 261.6255653005986346778499935233f;
				pitch = log2 (_freq * fmult);
				_last_valid_output = pitch;
			}
		}
		break;
	default:
		assert (false);
	}

	proc._sig_arr [0] [0] = _last_valid_output;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PitchDetect::clear_buffers ()
{
	_analyser.clear_buffers ();
	_nbr_spl_in_buf    = 0;
	_freq              = 0;
	_last_valid_output = 0;
}



void	PitchDetect::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		const float    fmin = float (_state_set.get_val_tgt_nat (Param_FREQ_MIN));
		const float    fmax = float (_state_set.get_val_tgt_nat (Param_FREQ_MAX));
		const int      type = fstb::round_int (
			_state_set.get_val_tgt_nat (Param_OUTPUT)
		);

		_analyser.set_freq_bot (fmin);
		_analyser.set_freq_top (fmax);
		_output_type = OutType (type);
	}
}



}  // namespace pidet
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
