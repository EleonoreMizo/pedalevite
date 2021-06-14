/*****************************************************************************

        ReverbSC.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_ReverbSC_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_ReverbSC_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// void fnc (P &, int);
template <typename T, typename P>
template <typename F>
void	ReverbSC <T, P>::config_fdbk_proc (F fnc)
{
	for (int dly_idx = 0; dly_idx < _nbr_delays; ++dly_idx)
	{
		fnc (_delay [dly_idx]._fdbk_proc, dly_idx);
	}
}



template <typename T, typename P>
void	ReverbSC <T, P>::set_sample_freq (double sample_freq)
{
	assert (sample_freq > 0);

	static constexpr std::array <ParamSet, _nbr_delays> params
	{{
		//    delay        drift  randfreq  seed
		{ 2473 / 44100.f, 1.0e-3f, 3.100f,  1966 },
		{ 2767 / 44100.f, 1.1e-3f, 3.500f, 29491 },
		{ 3217 / 44100.f, 1.7e-3f, 1.110f, 22937 },
		{ 3557 / 44100.f, 0.6e-3f, 3.973f,  9830 },
		{ 3907 / 44100.f, 1.0e-3f, 2.341f, 20643 },
		{ 4127 / 44100.f, 1.1e-3f, 1.897f, 22937 },
		{ 2143 / 44100.f, 1.7e-3f, 0.891f, 29491 },
		{ 1933 / 44100.f, 0.6e-3f, 3.221f, 14417 }
	}};

	_sr = float (sample_freq);
	for (int i = 0; i < _nbr_delays; ++i)
	{
		const int      sz = params [i].get_delay_size (_sr);
		_delay [i].init (params [i], sz, _sr);
	}
}



template <typename T, typename P>
void	ReverbSC <T, P>::set_size (float size) noexcept
{
	assert (_sr > 0);
	assert (_size >= 0);
	assert (_size <= 1);

	_size = size;
}



template <typename T, typename P>
void	ReverbSC <T, P>::set_cutoff (float cutoff) noexcept
{
	assert (_sr > 0);
	assert (cutoff > 0);
	assert (cutoff < _sr * 0.5f);

	_cutoff = cutoff;
}



template <typename T, typename P>
void	ReverbSC <T, P>::process_sample (T *outL, T *outR, T inL, T inR) noexcept
{
	assert (_sr > 0);

	T lsum = 0;
	T rsum = 0;

	// Calculates tone filter coefficient if frequency changed
	if (_pcutoff != _cutoff)
	{
		_pcutoff = _cutoff;
		_filt  = 2.f - cosf (_pcutoff * float (2 * fstb::PI) / _sr);
		_filt -= sqrtf (_filt * _filt - 1.f);
	}

	// Calculates "resultant junction pressure" and mix to input signals
	T jp = 0;
	for (int i = 0; i < _nbr_delays; ++i)
	{
		jp += _delay [i]._y;
	}

	jp *= _jp_scale;
	inL += jp;
	inR += jp;

	// Loops through all delay lines
	for (int i = 0; i < _nbr_delays; ++i)
	{
		// Mix to output
		if (i & 1)
		{
			rsum += _delay [i].compute (inR, _size, _filt, _sr);
		}
		else
		{
			lsum += _delay [i].compute (inL, _size, _filt, _sr);
		}
	}

	*outL = lsum * _output_gain;
	*outR = rsum * _output_gain;
}



template <typename T, typename P>
void	ReverbSC <T, P>::clear_buffers () noexcept
{
	for (auto &delay : _delay)
	{
		delay.clear_buffers ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Calculates the length of the delay line
template <typename T, typename P>
int	ReverbSC <T, P>::ParamSet::get_delay_size (float sr) const noexcept
{
	float          sz = _delay + _drift * 1.125f;

	return 16 + fstb::floor_int (sz * sr);
}



template <typename T, typename P>
void	ReverbSC <T, P>::Delay::init (const ParamSet &p, int sz, float sr)
{
	_buf.resize (sz);
	_sz       = sz;

	_drift    = p._drift;
	_seed     = p._seed;
	_dels     = p._delay;

	float          readpos = _dels;
	readpos  += calculate_drift ();
	readpos   = float (sz) - readpos * sr;

	_irpos    = fstb::floor_int (readpos);
	_frpos    = fstb::floor_int ((readpos - float (_irpos)) * float (FRACSCALE));
	_inc      = 0;
	_counter  = 0;
	_maxcount = fstb::round_int (sr / p._randfreq);

	generate_next_line (sr);

	clear_buffers ();
}



template <typename T, typename P>
T	ReverbSC <T, P>::Delay::compute (T in, T fdbk, T filt, float sr) noexcept
{
	// Sends input signal and feedback to delay line
	_buf [_wpos] = in - _y;
	++ _wpos;
	if (_wpos >= _sz)
	{
		_wpos -= _sz;
	}

	// Reads from delay line with cubic interpolation
	if (_frpos >= FRACSCALE)
	{
		_irpos += _frpos >> FRACNBITS;
		_frpos &= FRACMASK;
	}
	if (_irpos >= _sz)
	{
		_irpos -= _sz;
	}
	T frac_norm = T (_frpos) * (T (1) / T (FRACSCALE));

	// Calculates interpolation coefficients
	T tmp [2];
	T d = ((frac_norm * frac_norm) - 1) / T (6);
	tmp [0] = ((frac_norm + T (1)) * T (0.5));
	tmp [1] = T (3) * d;
	T a = tmp [0] - T (1) - d;
	T c = tmp [0] - tmp [1];
	T b = tmp [1] - frac_norm;

	int n = _irpos;

	// Reads four samples for interpolation
	T s [4];
	if (n > 0 && n < (_sz - 2))
	{
		s [0] = _buf [n - 1];
		s [1] = _buf [n    ];
		s [2] = _buf [n + 1];
		s [3] = _buf [n + 2];
	}
	else
	{
		// At buffer wrap-around, need to check index
		-- n;
		if (n < 0)
		{
			n += _sz;
		}
		s [0] = _buf [n];
		for (int k = 0; k < 3; ++k)
		{
			++ n;
			if (n >= _sz)
			{
				n -= _sz;
			}
			s [k + 1] = _buf [n];
		}
	}

	T out = (a * s [0] + b * s [1] + c * s [2] + d * s [3]) * frac_norm + s [1];

	// Updates buffer read position
	_frpos += _inc;

	// Additional processing
	out = _fdbk_proc.process_sample (out);

	// Applies feedback gain and lowpass filter
	out *= fdbk;
	out += (_y - out) * filt;
	_y = out;

	// Starts next random line segment if current one has reached endpoint
	-- _counter;
	if (_counter <= 0)
	{
		generate_next_line (sr);
	}
	return out;
}



template <typename T, typename P>
void	ReverbSC <T, P>::Delay::generate_next_line (float sr) noexcept
{
	// Updates random seed
	constexpr int  mul = fstb::ipowpc <6> (5); // 5^6;
	_rng &= _rng_mask;
	_rng *= mul;
	_rng += 1;
	_rng &= _rng_mask;
	if (_rng >= _rng_scale)
	{
		_rng -= _rng_range;
	}

	_counter = _maxcount;
	float          curdel =
		float (_wpos) - (float (_irpos) + float (_frpos) / float (FRACSCALE));
	while (curdel < 0)
	{
		curdel += float (_sz);
	}
	curdel /= sr;

	// Next delay time in seconds
	float          nxtdel = _dels + calculate_drift ();

	// Calculate phase increment per sample
	float          inc    = ((curdel - nxtdel) / float (_counter)) * sr;
	inc += 1;
	_inc = fstb::floor_int (inc * FRACSCALE);
}



template <typename T, typename P>
float	ReverbSC <T, P>::Delay::calculate_drift () const noexcept
{
	return _drift * float (_rng) / float (_rng_scale);
}



template <typename T, typename P>
void	ReverbSC <T, P>::Delay::clear_buffers () noexcept
{
	std::fill (_buf.begin (), _buf.end (), T (0));
	_rng = _seed;
	_y = 0;
}



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_ReverbSC_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
