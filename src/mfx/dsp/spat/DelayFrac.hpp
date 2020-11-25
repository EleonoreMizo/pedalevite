/*****************************************************************************

        DelayFrac.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED)
#define mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/rspl/InterpFtor.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace spat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// len = samples
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_max_len (int len)
{
	assert (len >= _delay_min);

   if (! _interp_init_flag)
   {
		init_interpolator ();
   }

	const int      buf_len_l2 = fstb::get_next_pow_2 (len + _nbr_phases - 1);
	const int      buf_len    = 1 << buf_len_l2;
	_delay_len = len;
	_buf_msk   = buf_len - 1;
	_buffer.resize (buf_len);

	clear_buffers ();
}



// ones.
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_delay_flt (float len_spl)
{
	set_delay_fix (fstb::conv_int_fast (len_spl * _nbr_phases));
}



// len_fixp = samples * _nbr_phases
template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::set_delay_fix (int len_fixp)
{
	assert (len_fixp >= _delay_min * _nbr_phases);

	_delay_frc = len_fixp & _phase_msk;
	_phase_ptr = &_phase_arr [_delay_frc];
	_delay_int = len_fixp >> _nbr_phases_l2;
	assert (_delay_int <= _delay_len);
}



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::read () const
{
	assert (_phase_ptr != nullptr);

	const int      pos_read = _pos_write - _delay_int;
	const T        v0 { _buffer [(pos_read - 2) & _buf_msk] };
	const T        v1 { _buffer [(pos_read - 1) & _buf_msk] };
	const T        v2 { _buffer [(pos_read    ) & _buf_msk] };
	const T        v3 { _buffer [(pos_read + 1) & _buf_msk] };

#if 1

	// FIR interpolation
	const Phase &  phase = *_phase_ptr;
	const T        i0 { phase [0] };
	const T        i1 { phase [1] };
	const T        i2 { phase [2] };
	const T        i3 { phase [3] };

	return (v0 * i0 + v1 * i1) + (v2 * i2 + v3 * i3);

#else

	// Simple linear interpolation, for testing
	return v2 + _delay_frc * (v1 - v2) * T (1.f / _nbr_phases);

#endif
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::write (T x)
{
	assert (_phase_ptr != nullptr);

	_buffer [_pos_write] = x;
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::step ()
{
	assert (_phase_ptr != nullptr);

	_pos_write = (_pos_write + 1) & _buf_msk;
}



template <typename T, int NPL2>
T	DelayFrac <T, NPL2>::process_sample (T x)
{
	assert (_phase_ptr != nullptr);

	write (x);
	const T        val { read () };
	step ();

	return val;
}



template <typename T, int NPL2>
void	DelayFrac <T, NPL2>::clear_buffers ()
{
	std::fill (_buffer.begin (), _buffer.end (), T (0.f));
	_pos_write = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, int NPL2>
bool	DelayFrac <T, NPL2>::_interp_init_flag = false;

template <typename T, int NPL2>
typename DelayFrac <T, NPL2>::PhaseArray	DelayFrac <T, NPL2>::_phase_arr;



template <typename T, int NPL2>
void  DelayFrac <T, NPL2>::init_interpolator ()
{
	// We use a cubic hermite interpolator to build the FIR.
	// Probably not optimal but close enough to rock'n'roll.
	rspl::InterpFtor::CubicHermite   interp;

	constexpr float   frac_mul = 1.f / _nbr_phases;
	for (int ph_idx = 0; ph_idx < _nbr_phases; ++ph_idx)
	{
		Phase &        phase = _phase_arr [ph_idx];
		const float    frac  = 1 - ph_idx * frac_mul;
		for (int pos = 0; pos < _phase_len; ++pos)
		{
			std::array <float, _phase_len> imp {};
			imp [pos] = 1;
			phase [pos] = interp (frac, imp.data () + 1);
		}
	}

	_interp_init_flag = true;
}



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_spat_DelayFrac_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
