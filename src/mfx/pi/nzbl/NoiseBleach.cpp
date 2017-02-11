/*****************************************************************************

        NoiseBleach.cpp
        Author: Laurent de Soras, 2017

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

#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/nzbl/NoiseBleach.h"
#include "mfx/pi/nzbl/Param.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace nzbl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



NoiseBleach::NoiseBleach ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_misc ()
,	_param_change_flag_notch_arr ()
,	_param_change_flag_band ()
,	_chn_arr ()
,	_lvl_base (1e-4f)
,	_lvl_notch_arr ()
,	_lvl_band_arr ()
,	_band_active_flag (false)
,	_buf_tmp_arr ()
{
	mfx::dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_LVL     , 1e-5);

	_state_set.add_observer (Param_LVL     , _param_change_flag_misc);
	
	_param_change_flag_misc.add_observer (_param_change_flag);
	_param_change_flag_band.add_observer (_param_change_flag);

	// Notches
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		const int      base  = NoiseBleachDesc::get_base_notch (index);
		auto &         cflag = _param_change_flag_notch_arr [index];

		_lvl_notch_arr [index] = 0;

		const double   freq = 80 << index;
		_state_set.set_val_nat (desc_set, base + ParamNotch_FREQ, freq);
		_state_set.set_val_nat (desc_set, base + ParamNotch_Q	  , 1);
		_state_set.set_val_nat (desc_set, base + ParamNotch_LVL , 0);

		_state_set.add_observer (base + ParamNotch_FREQ, cflag);
		_state_set.add_observer (base + ParamNotch_Q	  , cflag);
		_state_set.add_observer (base + ParamNotch_LVL , cflag);

		cflag.add_observer (_param_change_flag);
	}

	// Bands
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		const int      base  = NoiseBleachDesc::get_base_band (index);

		_lvl_band_arr [index] = 0;
		_state_set.set_val_nat (desc_set, base + ParamBand_LVL , 0);
		_state_set.add_observer (base + ParamBand_LVL , _param_change_flag_band);
	}

}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	NoiseBleach::do_get_state () const
{
	return _state;
}



double	NoiseBleach::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	NoiseBleach::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	const int      mbs_alig = (max_buf_len + 3) & -4;
	for (auto &buf : _buf_tmp_arr)
	{
		buf.resize (mbs_alig);
	}

	for (auto &chn : _chn_arr)
	{
		for (auto &notch : chn._notch_arr)
		{
			notch.reset (
				sample_freq,
				max_buf_len,
				&_buf_tmp_arr [0] [0],
				&_buf_tmp_arr [1] [0]
			);
		}
		chn._filter_bank.reset (sample_freq, max_buf_len);
	}
	

	_param_change_flag_misc.set ();
	for (auto &cflag : _param_change_flag_notch_arr)
	{
		 cflag.set ();
	}
	_param_change_flag_band.set ();

	update_param (true);

	_state = State_ACTIVE;

	return Err_OK;
}



void	NoiseBleach::do_process_block (ProcInfo &proc)
{
	// Channels
	const int      nbr_chn_src = proc._nbr_chn_arr [Dir_IN ];
	const int      nbr_chn_dst = proc._nbr_chn_arr [Dir_OUT];
	assert (nbr_chn_src <= nbr_chn_dst);
	const int      nbr_chn_proc = std::min (nbr_chn_src, nbr_chn_dst);

	// Events
	for (int evt_cnt = 0; evt_cnt < proc._nbr_evt; ++evt_cnt)
	{
		const piapi::EventTs &  evt = *(proc._evt_arr [evt_cnt]);
		if (evt._type == piapi::EventType_PARAM)
		{
			const piapi::EventParam &  evtp = evt._evt._param;
			assert (evtp._categ == piapi::ParamCateg_GLOBAL);
			_state_set.set_val (evtp._index, evtp._val);
		}
	}

	const int      nbr_spl  = proc._nbr_spl;

	// Parameters
	_state_set.process_block (nbr_spl);
	update_param (false);

	// Signal
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn     = _chn_arr [chn_index];

		const float *  src_ptr = proc._src_arr [chn_index];
		float *        dst_ptr = proc._dst_arr [chn_index];
		for (int n_idx = 0; n_idx < Cst::_nbr_notches; ++n_idx)
		{
			chn._notch_arr [n_idx].process_block (dst_ptr, src_ptr, nbr_spl);
			src_ptr = dst_ptr;
		}

		chn._filter_bank.process_block (dst_ptr, src_ptr, nbr_spl);
	}

	// Duplicates the remaining output channels
	for (int chn_index = 0; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	NoiseBleach::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_misc (true) || force_flag)
		{
			_lvl_base = float (_state_set.get_val_end_nat (Param_LVL));
			update_all_levels ();
		}

		// Notches
		for (int index = 0; index < Cst::_nbr_notches; ++index)
		{
			if (_param_change_flag_notch_arr [index] (true) || force_flag)
			{
				const int      base  = NoiseBleachDesc::get_base_notch (index);

				const float    freq = float (
					_state_set.get_val_end_nat (base + ParamNotch_FREQ)
				);
				const float    q    = float (
					_state_set.get_val_end_nat (base + ParamNotch_Q)
				);
				_lvl_notch_arr [index] = float (
					_state_set.get_val_end_nat (base + ParamNotch_LVL)
				);

				for (auto &chn : _chn_arr)
				{
					Notch &        notch = chn._notch_arr [index];
					notch.set_freq (freq);
					notch.set_q (q);
					notch.set_lvl (_lvl_notch_arr [index] * _lvl_base);
				}
			}
		}

		// Bands
		if (_param_change_flag_band (true) || force_flag)
		{
			for (int index = 0; index < Cst::_nbr_bands; ++index)
			{
				const int      base  = NoiseBleachDesc::get_base_band (index);
				_lvl_band_arr [index] = float (
					_state_set.get_val_end_nat (base + ParamBand_LVL)
				);
				
				const float    lvl_final = _lvl_band_arr [index] * _lvl_base;
				for (auto &chn : _chn_arr)
				{
					chn._filter_bank.set_level (index, lvl_final);
				}
				check_band_activity ();
			}
		}
	}
}



void	NoiseBleach::update_all_levels ()
{
	for (int index = 0; index < Cst::_nbr_notches; ++index)
	{
		const float    lvl_final = _lvl_notch_arr [index] * _lvl_base;
		for (auto &chn : _chn_arr)
		{
			chn._notch_arr [index].set_lvl (lvl_final);
		}
	}
	for (int index = 0; index < Cst::_nbr_bands; ++index)
	{
		const float    lvl_final = _lvl_band_arr [index] * _lvl_base;
		for (auto &chn : _chn_arr)
		{
			chn._filter_bank.set_level (index, lvl_final);
		}
	}
	check_band_activity ();
}



void	NoiseBleach::check_band_activity ()
{
	bool           active_flag = false;
	for (int index = 0; index < Cst::_nbr_bands && ! active_flag; ++index)
	{
		active_flag |= (_lvl_band_arr [index] > 1e-9f);
	}
	_band_active_flag = active_flag;
}



}  // namespace nzbl
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
