/*****************************************************************************

        Wah2.cpp
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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/iir/TransSZBilin.h"
#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/wah2/Param.h"
#include "mfx/pi/wah2/Wah2.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/Err.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/EventType.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace pi
{
namespace wah2
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Wah2::Wah2 ()
:	_state (State_CREATED)
,	_desc ()
,	_state_set ()
,	_sample_freq (0)
,	_inv_fs (0)
,	_param_change_flag ()
,	_param_change_flag_freq ()
,	_param_change_flag_type ()
,	_chn_arr ()
,	_model (0)
,	_bz ({{ 1, 0, 0 }})
,	_az ({{ 1, 0, 0 }})
,	_az_base ({{ 1, 0, 0 }})
,	_az_delta ({{ 1, 0, 0 }})
,	_ax (1)
{
	dsp::mix::Align::setup ();

	const ParamDescSet & desc_set = _desc.use_desc_set ();
	_state_set.init (piapi::ParamCateg_GLOBAL, desc_set);

	_state_set.set_val_nat (desc_set, Param_POS , 0.5);
	_state_set.set_val_nat (desc_set, Param_TYPE, 0);

	_state_set.add_observer (Param_POS , _param_change_flag_freq);
	_state_set.add_observer (Param_TYPE, _param_change_flag_type);

	_param_change_flag_freq.add_observer (_param_change_flag);
	_param_change_flag_type.add_observer (_param_change_flag);

	_state_set.set_ramp_time (Param_POS, 0.010);

	for (auto &chn : _chn_arr)
	{
		chn._hpf.neutralise ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



piapi::PluginInterface::State	Wah2::do_get_state () const
{
	return _state;
}



double	Wah2::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	assert (categ == piapi::ParamCateg_GLOBAL);

	return _state_set.use_state (index).get_val_tgt ();
}



int	Wah2::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	latency = 0;

	_sample_freq = float (    sample_freq);
	_inv_fs      = float (1 / sample_freq);

	_state_set.set_sample_freq (sample_freq);
	_state_set.clear_buffers ();

	_param_change_flag_freq.set ();
	_param_change_flag_type.set ();

	update_param (true);

	clear_buffers ();

	_state = State_ACTIVE;

	return piapi::Err_OK;
}



void	Wah2::do_clean_quick ()
{
	clear_buffers ();
}



void	Wah2::do_process_block (piapi::ProcInfo &proc)
{
	const int      nbr_chn_src = proc._dir_arr [piapi::Dir_IN ]._nbr_chn;
	const int      nbr_chn_dst = proc._dir_arr [piapi::Dir_OUT]._nbr_chn;
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

	// Parameters
	_state_set.process_block (proc._nbr_spl);
	update_param ();

	// Signal processing
	const int      nbr_spl = proc._nbr_spl;
	for (int chn_index = 0; chn_index < nbr_chn_proc; ++chn_index)
	{
		Channel &      chn = _chn_arr [chn_index];

		chn._hpf.process_block (
			proc._dst_arr [chn_index],
			proc._src_arr [chn_index],
			nbr_spl
		);

		process_reso (chn, proc._dst_arr [chn_index], nbr_spl);
	}

	// Duplicates the remaining output channels
	for (int chn_index = nbr_chn_proc; chn_index < nbr_chn_dst; ++chn_index)
	{
		dsp::mix::Align::copy_1_1 (
			proc._dst_arr [chn_index],
			proc._dst_arr [0],
			proc._nbr_spl
		);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Wah2::Channel::clear_buffers ()
{
	_hpf.clear_buffers ();
	for (auto &x : _mem_x) { x = 0; }
	for (auto &x : _mem_y) { x = 0; }
}



void	Wah2::clear_buffers ()
{
	for (auto &chn : _chn_arr)
	{
		chn.clear_buffers ();
	}
}



void	Wah2::update_param (bool force_flag)
{
	if (_param_change_flag (true) || force_flag)
	{
		if (_param_change_flag_type (true) || force_flag)
		{
			_model = fstb::round_int (_state_set.get_val_tgt_nat (Param_TYPE));
			update_model ();
		}

		if (_param_change_flag_freq (true) || force_flag)
		{
			const float    p   = float (_state_set.get_val_end_nat (Param_POS));
			const float    f   = 1 - p;
			const float    gw  = f * (1.15f + f * (-0.261f + f * 0.0342f));
			_az [0] = _az_base [0] + gw * _az_delta [0];
			_az [1] = _az_base [1] + gw * _az_delta [1];
			_az [2] = _az_base [2] + gw * _az_delta [2];
			_ax = 1.0f / _az [0];
		}
	}
}



void	Wah2::update_model ()
{
	const Spec &   spec = _spec_arr [_model];

	const float    ro = para (spec._rc, spec._rpot, spec._rbias);
	const float    ic = 3.7f / spec._rc;   // Typical bias current
	const float    re = 0.025f / ic;       // BJT gain stage equivalent internal emitter resistance
	                                       // gm = Ic/Vt, re = 1/gm
	const float    req = spec._re + re;
	const float    gf  = -ro / req;
	const float    re2 = spec._beta * req;
	float          rp  = spec._rp_main;
	switch (spec._rp_par)
	{
	case Spec::RpPar_RE:
		rp = para (rp, re2);
		break;
	case Spec::RpPar_RI:
		rp = para (rp, spec._ri);
		break;
	case Spec::RpPar_ALONE:
		// Nothing
		break;
	default:
		assert (false);
		break;
	}

	// High-pass filter
	const float    f_hp = 0.5f / (float (fstb::PI) * spec._ri * spec._ci);

	// Resonant filter
	const float    rpri = para (rp, spec._ri);
	const float    f0   = 0.5f / (float (fstb::PI) * sqrt (spec._lp * spec._cf));
	const float    q    = rpri * sqrt (spec._cf / spec._lp);
	const float    gi   = spec._rs / (spec._ri + spec._rs);
	const float    gbpf = 0.5f / (float (fstb::PI) * f0 * spec._ri * spec._cf);

	update_model (f_hp, f0, q, gf, gi, gbpf);
}



void	Wah2::update_model (float f_hp, float f0, float q, float gf, float gi, float gbpf)
{
	// High-pass filter
	// BJT forward gain worked in here to save extra multiplications in
	// updating biquad coefficients
	const float    bs2 [2] = { 0, gf };
	const float    as2 [2] = { 1,  1 };
	const float    khpf =
		dsp::iir::TransSZBilin::compute_k_approx (f_hp * _inv_fs);
	float          bz2 [2];
	float          az2 [2];
	dsp::iir::TransSZBilin::map_s_to_z_one_pole_approx (
		bz2, az2, bs2, as2, khpf
	);
	for (auto & chn : _chn_arr)
	{
		chn._hpf.set_z_eq (bz2, az2);
	}

	// Resonant filter
	const float    w0   = float (fstb::PI) * 2 * f0 * _inv_fs;
	const float    c    = float (cos (w0));
	const float    s    = float (sin (w0));
	const float    a    = s / (q * 2);

	const float    bzh [3] =
	{
		 (c + 1) * 0.5f,
		-(c + 1),
		 (c + 1) * 0.5f
	};
	const float    bzb [3] =
	{
		 q * a,
		 0,
		-q * a
	};
	const float    azb [3] =
	{
		 1 + a,
		-2 * c,
		 1 - a
	};

	// Distill all down to final biquad coefficients
	_bz [0]       = gbpf * bzb [0] + gi * azb [0];
	_bz [1]       = gbpf * bzb [1] + gi * azb [1];
	_bz [2]       = gbpf * bzb [2] + gi * azb [2];

	_az_base [0]  =        azb [0];
	_az_base [1]  =        azb [1];
	_az_base [2]  =        azb [2];

	_az_delta [0] = -gf  * bzh [0];
	_az_delta [1] = -gf  * bzh [1];
	_az_delta [2] = -gf  * bzh [2];

	_param_change_flag_freq.set ();
}



void	Wah2::process_reso (Channel &chn, float spl_ptr [], int nbr_spl) const
{
	float          x1 = chn._mem_x [0];
	float          x2 = chn._mem_x [1];
	float          y1 = chn._mem_y [0];
	float          y2 = chn._mem_y [1];

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		const float    x  = spl_ptr [pos];

		float          y0 =
			  _bz [0] * x
			+ _bz [1] * x1
			+ _bz [2] * x2
			- _az [1] * y1
			- _az [2] * y2;
		y0 *= _ax;

		const float    y  = shaper (y0);

		// Let a little harmonic distortion feed back into the filter loop
		y0 += 0.05f * (y - y0);

		spl_ptr [pos] = y;

		x2 = x1;
		x1 = x;
		y2 = y1;
		y1 = y0;
	}

	chn._mem_x [0] = x1;
	chn._mem_x [1] = x2;
	chn._mem_y [0] = y1;
	chn._mem_y [1] = y2;
}



float	Wah2::para (float a, float b)
{
	return (a * b) / (a + b);
}



float	Wah2::para (float a, float b, float c)
{
	return para (para (a, b), c);
}



float	Wah2::shaper (float x)
{
	x = fstb::limit (x, -1.12f, 1.2f);

	const float    thr_p =  0.8f;
	const float    thr_n = -0.72f;
	const float    f     =  1.25f;
	if (x > thr_p)
	{
		const float    u = x - thr_p;
		x -= f * u * u;
	}
	else if (x < thr_n)
	{
		const float    u = x - thr_n;
		x += f * u * u;
	}

	return x;
}



const Wah2::Spec	Wah2::_spec_arr [6] =
{
	{ 22e3f, 100e3f, 470e3f, 390,  250, 10e-9f, 10e-9f,  68e3f,  33e3f, Spec::RpPar_RE   , 0.50f, 1.5e3f },
	{ 22e3f, 100e3f, 470e3f, 510,  650, 10e-9f, 10e-9f,  68e3f,  33e3f, Spec::RpPar_RI   , 0.50f, 1.5e3f },
	{ 22e3f, 100e3f, 470e3f, 470,  250, 10e-9f, 10e-9f,  68e3f,  33e3f, Spec::RpPar_RE   , 0.66f, 1.5e3f },
	{ 22e3f, 100e3f, 470e3f, 470,  250, 10e-9f, 10e-9f,  68e3f, 100e3f, Spec::RpPar_RE   , 0.50f, 1.5e3f },
	{ 22e3f, 100e3f, 470e3f, 510,  250, 15e-9f,  8e-9f,  68e3f,  47e3f, Spec::RpPar_RE   , 0.50f, 800    },
	{ 22e3f, 100e3f, 470e3f, 150, 1200, 10e-9f, 47e-9f, 220e3f, 150e3f, Spec::RpPar_ALONE, 0.50f, 100    }
};



}  // namespace wah2
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
