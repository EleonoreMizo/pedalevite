/*****************************************************************************

        AdsrRc.cpp
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

#include "fstb/fnc.h"
#include "mfx/dsp/ctrl/env/AdsrRc.h"
#include "mfx/dsp/mix/Generic.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/




const float	AdsrRc::_infinite_sus = 1999.f;



AdsrRc::AdsrRc () noexcept
{
	mix::Generic::setup ();
	set_sample_freq (44100);
}



void	AdsrRc::set_sample_freq (float fs) noexcept
{
	assert (fs > 0);

	_sample_freq = fs;

	set_atk_time (_atk_time);
	set_dcy_time (_dcy_time);
	set_sus_time (_sus_time);
	set_sus_lvl (_sus_lvl);
	set_rls_time (_rls_time);
}



void	AdsrRc::set_atk_time (float at) noexcept
{
	assert (at > 0);

	_atk_time = at;
	update_atk_seg ();
}



void	AdsrRc::set_atk_virt_lvl (float atk_lvl) noexcept
{
	assert (atk_lvl > 1);

	_atk_virt_lvl = atk_lvl;
	update_atk_seg ();
}



void	AdsrRc::set_dcy_time (float dt) noexcept
{
	assert (dt > 0);

	_dcy_time = dt;
	update_dcy_seg ();
}



void	AdsrRc::set_sus_lvl (float sl) noexcept
{
	_sus_lvl = sl;
	if (_seg == AdsrSeg_SUS && _sus_time >= _infinite_sus)
	{
		_seg_sus.set_val (_sus_lvl);

		_seg         = AdsrSeg_DCY;
		_cur_seg_ptr = &_seg_dcy;
		_seg_dcy.set_val (_cur_val);
		check_and_set_next_seg ();
	}
	update_dcy_seg ();
}



void	AdsrRc::set_sus_time (float st) noexcept
{
	assert (st > 0);

	_sus_time = st;

	const float    ratio = 0.01f;
	if (_sus_time >= _infinite_sus)
	{
		_seg_sus.setup (0, 1, ratio);
	}
	else
	{
		const float    nbr_spl = _sus_time * _sample_freq;
		const float    mult    = float (exp (log (ratio) / nbr_spl));

		_seg_sus.setup (0, mult, ratio);	// ratio ~= end_threshold
	}

	check_and_set_next_seg ();
}



void	AdsrRc::set_rls_time (float rt) noexcept
{
	assert (rt > 0);

	_rls_time = rt;

	const float    nbr_spl = _rls_time * _sample_freq;
	const float    ratio   = 0.01f;
	const float    mult    = float (exp (log (ratio) / nbr_spl));

	_seg_rls.setup (0, mult, ratio);	// ratio ~= end_threshold
	check_and_set_next_seg ();
}



void	AdsrRc::note_on () noexcept
{
	_seg         = AdsrSeg_ATK;
	_cur_seg_ptr = &_seg_atk;
	_seg_atk.set_val (_cur_val);
	check_and_set_next_seg ();
}



void	AdsrRc::note_off () noexcept
{
	if (   _seg != AdsrSeg_NONE
	    && _seg != AdsrSeg_RLS)
	{
		_seg         = AdsrSeg_RLS;
		_cur_seg_ptr = &_seg_rls;
		_seg_rls.set_val (_cur_val);
		check_and_set_next_seg ();
	}
}



void	AdsrRc::clear_buffers () noexcept
{
	_seg = AdsrSeg_NONE;
	_cur_val = 0;
}



void	AdsrRc::kill_ramps () noexcept
{
	assert (   _seg == AdsrSeg_NONE
	        || _seg == AdsrSeg_ATK);

	_cur_val = 0;
	if (_seg == AdsrSeg_ATK)
	{
		_seg_atk.set_val (_cur_val);
		check_and_set_next_seg ();
	}
}



float	AdsrRc::process_sample () noexcept
{
	if (_seg == AdsrSeg_NONE)
	{
		_cur_val = 0;
	}
	else
	{
		assert (_cur_seg_ptr != nullptr);
		_cur_val = _cur_seg_ptr->process_sample ();
		check_and_set_next_seg ();
	}

	return _cur_val;
}



void	AdsrRc::process_block (float data [], int nbr_spl) noexcept
{
	int            pos = 0;
	do
	{
		if (_seg == AdsrSeg_NONE)
		{
			_cur_val = 0;
			dsp::mix::Generic::clear (&data [pos], nbr_spl - pos);
			pos      = nbr_spl;
		}
		else
		{
			assert (_cur_seg_ptr != nullptr);
			const int      work_len = std::min (
				nbr_spl - pos,
				_cur_seg_ptr->get_nbr_rem_spl ()
			);
			_cur_seg_ptr->process_block (&data [pos], work_len);
			pos     += work_len;
			_cur_val = _cur_seg_ptr->get_val ();
			check_and_set_next_seg ();
		}
	}
	while (pos < nbr_spl);
}



void	AdsrRc::skip_block (int nbr_spl) noexcept
{
	int            pos = 0;
	do
	{
		if (_seg == AdsrSeg_NONE)
		{
			_cur_val = 0;
			pos      = nbr_spl;
		}
		else
		{
			assert (_cur_seg_ptr != nullptr);
			const int      work_len = std::min (
				nbr_spl - pos,
				_cur_seg_ptr->get_nbr_rem_spl ()
			);
			_cur_seg_ptr->skip_block (work_len);
			pos     += work_len;
			_cur_val = _cur_seg_ptr->get_val ();
			check_and_set_next_seg ();
		}
	}
	while (pos < nbr_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	AdsrRc::update_atk_seg () noexcept
{
	assert (_atk_virt_lvl > 1);

	const float    nbr_spl = _atk_time * _sample_freq;
	const float    ratio   = (_atk_virt_lvl - 1) / _atk_virt_lvl;
	const float    mult    = float (exp (log (ratio) / nbr_spl));

	_seg_atk.setup (1, mult, _atk_virt_lvl - 1);
	check_and_set_next_seg ();
}



void	AdsrRc::update_dcy_seg () noexcept
{
	const float    nbr_spl = _dcy_time * _sample_freq;
	const float    ratio   = 0.01f;
	const float    mult    = float (exp (log (ratio) / nbr_spl));

	_seg_dcy.setup (
		_sus_lvl,
		mult,
		ratio,	// ratio ~= end_threshold
		fstb::round_int (nbr_spl)
	);
	check_and_set_next_seg ();
}



void	AdsrRc::check_and_set_next_seg () noexcept
{
	while (   _cur_seg_ptr != nullptr
	       && _cur_seg_ptr->get_nbr_rem_spl () <= 0)
	{
		_seg         = _next_seg [_seg];
		_cur_seg_ptr = _seg_ptr_arr [_seg];
		if (_cur_seg_ptr != nullptr)
		{
			_cur_seg_ptr->set_val (_cur_val);
		}
	}
}



const std::array <AdsrSeg, AdsrSeg_NBR_ELT>	AdsrRc::_next_seg =
{{
	AdsrSeg_ATK,   // AdsrSeg_NONE
	AdsrSeg_DCY,   // AdsrSeg_ATK
	AdsrSeg_SUS,   // AdsrSeg_DCY
	AdsrSeg_RLS,   // AdsrSeg_SUS
	AdsrSeg_NONE   // AdsrSeg_RLS
}};



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
