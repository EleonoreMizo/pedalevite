/*****************************************************************************

        PitchDetect2.cpp
        Author: Laurent de Soras, 2021

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
#include "mfx/pi/pidet2/Param.h"
#include "mfx/pi/pidet2/PitchDetect2.h"
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
namespace pidet2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PitchDetect2::PitchDetect2 (piapi::HostInterface &host)
:	_host (host)
{
	dsp::mix::Align::setup ();

	_data_xptr->_dspl.set_coefs (
		_dspl_coef_84.data (), _dspl_coef_42.data (), _dspl_coef_21.data ()
	);

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_FREQ_MIN,   30);
	_state_set.set_val_nat (desc_set, Param_FREQ_MAX, 1000);

	_state_set.add_observer (Param_FREQ_MIN, _param_change_flag);
	_state_set.add_observer (Param_FREQ_MAX, _param_change_flag);

	auto &         postproc = _analyser.use_postproc ();
	postproc.set_responsiveness (0.5f);
	postproc.set_threshold (0.0f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	PitchDetect2::do_get_state () const
{
	return _state;
}



double	PitchDetect2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	fstb::unused (categ, note_id);
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	PitchDetect2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_buffer.resize (max_buf_len);
	_analyser.set_sample_freq (sample_freq / _sub_spl);
	_analyser.set_analysis_period (
		(std::min (max_buf_len, 64) + _sub_spl - 1) / _sub_spl
	);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag.set ();

	update_param (true);

	clear_buffers ();
	_param_proc.req_steady ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	PitchDetect2::do_process_block (piapi::ProcInfo &proc)
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
	const int      len_sub      = end_sub - pos_sub;
	if (len_sub > 0)
	{
		_data_xptr->_dspl.process_block (
			&_buffer [pos_sub], spl_ptr + pos, len_sub
		);
		pos += _sub_spl * len_sub;
		pos_sub = end_sub;
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
	if (pos_sub > 0)
	{
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



void	PitchDetect2::clear_buffers ()
{
	_data_xptr->_dspl.clear_buffers ();
	_analyser.clear_buffers ();
	_nbr_spl_in_buf    = 0;
	_freq              = 0;
	_last_valid_output = 0;
}



void	PitchDetect2::update_param (bool force_flag)
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



const std::array <double, PitchDetect2::_nc_84>	PitchDetect2::_dspl_coef_84 =
{
	0.11116740519894572,
	0.53837812120804285
};
const std::array <double, PitchDetect2::_nc_42>	PitchDetect2::_dspl_coef_42 =
{
	0.064957826108157521,
	0.27015213717425107,
	0.67154776115693604
};
const std::array <double, PitchDetect2::_nc_21>	PitchDetect2::_dspl_coef_21 =
{
	0.042153109027814516,
	0.15563989022979918,
	0.30958383558775832,
	0.4717493609227047,
	0.6205678058528159,
	0.74780968500509482,
	0.85549697199148433,
	0.95204827732052744
};



}  // namespace pidet2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
