/*****************************************************************************

        PhaseGenChaos.cpp
        Author: Laurent de Soras, 2016

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
#include "mfx/dsp/ctrl/lfo/PhaseGenChaos.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PhaseGenChaos::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_phase_gen.set_sample_freq (sample_freq);
	update_retrig_len ();
}



void	PhaseGenChaos::set_period (double per) noexcept
{
	assert (per > 0);

	_phase_gen.set_period (per);
	update_retrig_len ();
}



void	PhaseGenChaos::set_phase (double phase) noexcept
{
	assert (phase >= 0);
	assert (phase < 1);

	_phase_gen.set_phase (phase);
}



void	PhaseGenChaos::set_chaos (double chaos) noexcept
{
	assert (chaos >= 0);
	assert (chaos <= 1);

	_chaos = fstb::floor_int (chaos * 256);
	if (_chaos <= 0)
	{
		_ramp_flag   = false;
		_ramp_pos    = 0;
		_phase_shift = 0;
	}
}



void	PhaseGenChaos::clear_buffers () noexcept
{
	_ramp_flag   = false;
	_speed       = 0;
	_ramp_pos    = 0;
	_phase_shift = 0;
	_retrig_pos  = 0;
	_rand_gen.seed (_rand_gen.default_seed);
}



void	PhaseGenChaos::tick (int nbr_spl) noexcept
{
	_phase_gen.tick (nbr_spl);

	if (_chaos > 0)
	{
		// Start a ramp ?
		int            pos = 0;
		while (! _ramp_flag && pos < nbr_spl)
		{
			static const float   rnd_mul =
				1.0f / float (_rand_gen.max () - _rand_gen.min ());

			const int      work_len =
				std::min (nbr_spl - pos, _retrig_len - _retrig_pos);
			_retrig_pos += work_len;
			if (_retrig_pos >= _retrig_len)
			{
				_retrig_pos = 0;
				_ramp_flag  = true;

				const float    val        = float (_rand_gen ()) * rnd_mul;
				const float    nbr_cycles =   // Rough approx of 2 ^ (val * 4 - 2)
					((35.f/16 * val + 5.f/4) * val + 5.f/16) * val + 1.f/4;
				const float    lfo_spl_per_cycles = float (
					_phase_gen.get_period () * _phase_gen.get_sample_freq ()
				);
				const float    ramp_len_spl = nbr_cycles * lfo_spl_per_cycles;
				_speed    = 2.0f / ramp_len_spl;
				_ramp_pos = -1.0f;

				_dest     =
					(int (_rand_gen () & 0xFF) < _chaos) ? Dest_GO : Dest_STAY;
				if (_dest == Dest_STAY)
				{
					float				val_2 = float (_rand_gen ()) * rnd_mul;
					_amp = val_2 * val_2;
				}

				if (nbr_cycles < 1 && _dest == Dest_GO)
				{
					_dir = Dir_POS;
				}
				else
				{
					static_assert (Dir_NBR_ELT == 2, "Modulo value");
					_dir = static_cast <Dir> (_rand_gen () & 1);
				}
			}
		
			if (! _ramp_flag)
			{
				pos += work_len;
			}
		}

		// Next pos for the ramp
		_ramp_pos += _speed * (nbr_spl - pos);
		if (_ramp_pos > 1)
		{
			assert (_ramp_flag);
			_ramp_flag   = false;
			_speed       = 0;
			_ramp_pos    = 0;
			_phase_shift = 0;
			_retrig_pos  = 0;
		}

		// Computes the phase shift
		if (_ramp_flag)
		{
			const float		rp2 = float (_ramp_pos * _ramp_pos);
			switch (_dest)
			{
			case	Dest_STAY:
				{
					const float		temp = 1 - rp2;
					_phase_shift = temp * temp * _amp;
				}
				break;

			case	Dest_GO:
				_phase_shift = 0.5f + (3 - rp2) * (_ramp_pos * 0.25f);
				break;
			
			case Dest_NBR_ELT:
			default:
				assert (false);
				break;
			}

			if (_dir == Dir_NEG)
			{
				_phase_shift = -_phase_shift;
			}
		}
	}
}



double	PhaseGenChaos::get_phase () const noexcept
{
	double			pos = _phase_gen.get_phase ();
	if (_chaos > 0)
	{
		pos += _phase_shift + 1;
		while (pos >= 1)
		{
			pos -= 1;
		}
	}

	return pos;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	PhaseGenChaos::update_retrig_len () noexcept
{
	// Update every 2/3-cycles
	const float		per = float (_phase_gen.get_period ());
	_retrig_len = fstb::round_int (
		_phase_gen.get_sample_freq () * per * (2.0f / 3)
	);
	_retrig_pos = 0;
}



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
