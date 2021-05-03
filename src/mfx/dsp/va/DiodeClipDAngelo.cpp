/*****************************************************************************

        DiodeClipDAngelo.cpp
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

#include "fstb/Approx.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/va/DiodeClipDAngelo.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace va
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipDAngelo::set_sample_freq (double sample_freq) noexcept
{
	assert (sample_freq > 0);

	_sample_freq = float (sample_freq);
	update_internal_coef_fs ();
}



void	DiodeClipDAngelo::set_d1_is (float is) noexcept
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is1        = is;
	_dirty_flag = true;
}



void	DiodeClipDAngelo::set_d2_is (float is) noexcept
{
	assert (is >= 1e-20f);
	assert (is <= 0.1f);

	_is2        = is;
	_dirty_flag = true;
}



void	DiodeClipDAngelo::set_d1_n (float n) noexcept
{
	assert (n > 0);

	_n1         = n;
	_dirty_flag = true;
}



void	DiodeClipDAngelo::set_d2_n (float n) noexcept
{
	assert (n > 0);

	_n2         = n;
	_dirty_flag = true;
}



// It could be interesting to make c proportional to the input gain.
void	DiodeClipDAngelo::set_capa (float c) noexcept
{
	assert (c > 0);

	_c          = c;
	_dirty_flag = true;
}



// Same as setting C, but with a meaningful value.
void	DiodeClipDAngelo::set_cutoff_freq (float f) noexcept
{
	assert (f > 0);

	set_capa (1.0f / (float (2 * fstb::PI) * _r * f));
}



void	DiodeClipDAngelo::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	if (_dirty_flag)
	{
		update_internal_coef ();
	}

	float          mem_p = _mem_p;
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample_internal (src_ptr [pos], mem_p);
	}
	_mem_p = mem_p;
}



void	DiodeClipDAngelo::clear_buffers () noexcept
{
	_mem_p = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	DiodeClipDAngelo::update_internal_coef_fs () noexcept
{
	// Trapezoidal rule
	// v[n] = v[n-1] + (T/2) * (v'[n] + v'[n-1])
	// v'[n] = (2/T) * v[n] - (2/T) * v[n-1] - v'[n-1]
	_b0 = _sample_freq * 2;
	_b1 = -_b0;
	_a1 = 1;

	_k6 = _b1 - _a1 * _b0;
	_dirty_flag = true;
}



void	DiodeClipDAngelo::update_internal_coef () noexcept
{
	const float    cr        = _c * _r;
	const float    invb0crp1 = 1 / (_b0 * cr + 1);
	_k1  = 1 / cr;
	_k2  = cr * invb0crp1;
	_k31 =  _is1 * _r * invb0crp1;
	_k32 = -_is2 * _r * invb0crp1;
	_k41 =  1 / (_vt * _n1);
	_k42 = -1 / (_vt * _n2);
	_k51 = fstb::Approx::log2 (_k31 * _k41) * float (fstb::LN2);
	_k52 = fstb::Approx::log2 (_k32 * _k42) * float (fstb::LN2);
	_k71 =  _vt * _n1;
	_k72 = -_vt * _n2;

	_dirty_flag = false;
}



}  // namespace va
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
