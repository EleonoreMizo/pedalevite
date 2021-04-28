/*****************************************************************************

        DesignPhaseMin.cpp
        Author: Laurent de Soras, 2021

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

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/fir/DesignPhaseMin.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_fft_len
Description:
Input parameters:
	- fft_len: 
Throws: ?
==============================================================================
*/

void	DesignPhaseMin::set_fft_len (int fft_len)
{
	assert (fft_len > 0);
	assert (fstb::is_pow_2 (fft_len));

	if (fft_len != get_fft_len ())
	{
		release_buffers ();

		_fft_uptr = std::make_unique <FftType> (fft_len);

		for (auto &buffer : _buf_list)
		{
			buffer.resize (fft_len);
		}
	}
}



/*
==============================================================================
Name: release_buffers
Description:
Throws: ?
==============================================================================
*/

void	DesignPhaseMin::release_buffers ()
{
	for (auto &buffer : _buf_list)
	{
		Buffer {}.swap (buffer);
	}
	_fft_uptr.reset ();
}



/*
==============================================================================
Name: compute_optimal_fft_length
Description:
	set_fft_len() must have been called.
	Based on article formulas (28) to (32)
Input parameters:
	- f_stop: normalised stopband frequency, ]0 ; 0.5[
	- n2: number of zeros in the stopband, >= 2
	- epsilon: cepstral error, > 0
Returns: the FFT length, power of 2 > 0.
Throws: Nothing
==============================================================================
*/

int	DesignPhaseMin::compute_optimal_fft_length (double f_stop, int n2, double epsilon) noexcept
{
	assert (f_stop > 0);
	assert (f_stop < 1);
	assert (n2 >= 2);
	assert (epsilon > 0);

	const int      l    = (n2 - 1) / 2;
	const double   mult = (1 - 2 * f_stop) / (n2 - 1);
	double         sum  = 0;
	for (int i = 0; i <= l; ++i)
	{
		const double   fi = f_stop + i * mult;
		const double   c  = cos (fi * (2 * fstb::PI));
		sum += c;
	}
	const double   m     = 2 + log (fabs (sum) / epsilon) / fstb::LN2;
	assert (m < 31); // The FFT would be too large
	const int      m_int = fstb::limit (fstb::ceil_int (m), 2, 30);

	return 1 << m_int;
}



/*
==============================================================================
Name: minimize_phase
Description:
	Turns a linear-phase filter into a minimum-phase filter.
	The resulting filter length is halved, rounded up.
	set_fft_len() must have been called.
Input parameters:
	- lin_ptr: pointer on the array containing the original linear-phase filter
		of length len.
	- len: length of the linear-phase filter, [1 ; fft_length]
Output parameters:
	- min_ptr: pointer on an array containing the resulting minimum-phase
		filter coefficients, whose length is (len + 1) / 2.
Throws: Nothing
==============================================================================
*/

void	DesignPhaseMin::minimize_phase (float min_ptr [], const float lin_ptr [], int len) noexcept
{
	assert (_fft_uptr.get () != nullptr);
	assert (min_ptr != nullptr);
	assert (lin_ptr != nullptr);
	assert (len > 0);
	assert (len <= get_fft_len ());

	const int      fft_len = get_fft_len ();

	// H=fft(b,N); 
	Buffer &       buf_b = _buf_list [_buf_b];
	std::copy (lin_ptr, lin_ptr + len, buf_b.begin ());
	std::fill (buf_b.begin () + len, buf_b.end (), DataType (0));

	Buffer &       buf_h = _buf_list [_buf_h];
	_fft_uptr->do_fft (buf_h.data (), buf_b.data ());

	// w1=linspace(0,2*pi,N);
	// H1=real(H.*exp(j*w1*324));
	Buffer &       buf_h123r  = _buf_list [_buf_h123r];
	const int      len_h      = (len + 1) >> 1;
	const double   angle_step = (fstb::PI * 2) * (len_h - 1) / (fft_len - 1);
	const int      fft_len_h  = fft_len >> 1;
	buf_h123r [0] = buf_h [0];
	for (int pos = 1; pos < fft_len_h; ++pos)
	{
		const DataType h_r  = buf_h [pos            ];
		const DataType h_i  = buf_h [pos + fft_len_h];

		const double   a    = pos * angle_step;
		const DataType h1_r = h_r * cos (a) + h_i * sin (a); // + = -- for FFTReal
		buf_h123r [pos          ] = h1_r;

		const double   a_inv    = (fft_len - pos) * angle_step;
		const DataType h1_r_inv = h_r * cos (a_inv) - h_i * sin (a_inv); // - for FFTReal
		buf_h123r [fft_len - pos] = h1_r_inv;
	}
	buf_h123r [fft_len_h] = buf_h [fft_len_h] * cos (fft_len_h * angle_step);

	// d1=max(H1)-1;
	// d2=0-min(H1);
	DataType       d1 = 0;
	DataType       d2 = 1;
	for (int pos = 0; pos < fft_len; ++pos)
	{
		const DataType val = buf_h123r [pos];
		d1 = std::max (val, d1);
		d2 = std::min (val, d2);
	}
	d1 -= DataType (1);
	d2  = -d2;

	// S=4/(sqrt(1+d1+d2)+sqrt(1-d1+d2))^2;
	const double   s_sqrt = 2 / (sqrt (d1 + d2 + 1) + sqrt (d2 - d1 + 1));
	const DataType s      = DataType (s_sqrt * s_sqrt);

	// H2=H1+d2;
	// H3=H2*S;
	// HR=sqrt(H3)+1e-10;
	for (int pos = 0; pos < fft_len; ++pos)
	{
		DataType       val = buf_h123r [pos];
		val += d2;
		val *= s;
		val  = DataType (sqrt (val) + 1e-10);
		buf_h123r [pos] = val;
	}

	// y=dhtm(HR',N,325);
	// y1=real(y);
	Buffer &       buf_y = _buf_list [_buf_y];
	compute_dht (buf_y.data (), buf_h123r.data (), len_h);
	for (int pos = 0; pos < len_h; ++pos)
	{
		min_ptr [pos] = float (buf_y [pos]);
	}
}



/*
==============================================================================
Name: compute_linear_spec
Description:
	Computes the design spec for the linear-phase filter given the desired
	spec for the minimum-phase filter.
Input parameters:
	- min_phase_spec: a Ripple structure filled with the desired passband and
		stopband ripples of the minimum-phase filter, in dB.
Returns: the Ripple structure for the linear-phase filter, in dB.
Throws: Nothing
==============================================================================
*/

DesignPhaseMin::Ripple	DesignPhaseMin::compute_linear_spec (const Ripple &min_phase_spec) noexcept
{
	assert (min_phase_spec._passband > 0);
	assert (min_phase_spec._passband < 20 * log10 (2));
	assert (min_phase_spec._stopband > min_phase_spec._passband);

	const double   m_pb = pow (10,  min_phase_spec._passband / 20) - 1;
	const double   m_sb = pow (10, -min_phase_spec._stopband / 20);

	// Eq. (10) and (11)
	const double   mult = 1 / (2 + 2 * m_pb * m_pb - m_sb * m_sb);
	const double   l_pb = mult * 4 * m_pb;
	const double   l_sb = mult * m_sb * m_sb;

	Ripple         r;
	r._passband =  20 * log10 (l_pb + 1);
	r._stopband = -20 * log10 (l_sb    );

	return r;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DesignPhaseMin::get_fft_len () const noexcept
{
	return int (_buf_list.front ().size ());
}



/*
Algorithm (Matlab):

sig = i * [0 ones(1,N/2-1) 0 -ones(1,N/2-1)];

mag_log = log (abs (mag));

in = fft (mag_log);
in_sig = sig .* in;
ph = ifft (in_sig);

rec = mag .* exp (i * ph);

rec_inv = [rec(1) rec(length(rec):-1:2)];
rec_s = 0.5 * (rec + rec_inv);
rec_a = 0.5 * (rec - rec_inv);
rec_tmp = real (rec_s) + i * imag (rec_a);
recu = ifft (rec_tmp);

y = recu (1:s);

Validation test:

fft length = 8
mag [8] = { 9, 8, 7, 6, 2, 3, 5, 3 };
recu [8] ==
{
	4.7185f,  2.0023f, -0.7167f, 0.9918f,
	0.4609f, -0.3244f, -0.0502f, 0.0070f
};
*/

void	DesignPhaseMin::compute_dht (DataType recu_ptr [], const DataType mag_ptr [], int trunc_len) noexcept
{
	assert (_fft_uptr.get () != nullptr);
	assert (recu_ptr != nullptr);
	assert (mag_ptr != nullptr);
	assert (trunc_len > 0);
	assert (trunc_len <= get_fft_len ());

	// mag_log = log (abs (mag));
	Buffer &       buf_mag_log = _buf_list [_buf_maglog];
	assert (buf_mag_log.data () != mag_ptr);
	const int      len         = get_fft_len ();
	for (int pos = 0; pos < len; ++pos)
	{
		const DataType    val     = mag_ptr [pos];

		const DataType    val_abs = DataType (fabs (val));
		assert (val_abs > 0);
		const DataType    val_log = DataType (log (val_abs));

		buf_mag_log [pos] = val_log;
	}

	// in = fft (mag_log);
	Buffer &       buf_in = _buf_list [_buf_in];
	assert (buf_in.data () != mag_ptr);
	_fft_uptr->do_fft (buf_in.data (), buf_mag_log.data ());

	// in_sig = sig .* in
	const int      len_h = len >> 1;
	buf_in [0] = 0;
	for (int pos = 1; pos < len_h; ++pos)
	{
		const DataType    tmp = buf_in [pos        ];
		buf_in [pos        ]  = buf_in [pos + len_h];
		buf_in [pos + len_h]  = -tmp;
	}
	buf_in [len_h] = 0;

   // ph = ifft (in_sig);
   Buffer &       buf_ph = _buf_list [_buf_ph];
	assert (buf_ph.data () != mag_ptr);
   _fft_uptr->do_ifft (buf_in.data (), buf_ph.data ());
   _fft_uptr->rescale (buf_ph.data ());

	// rec = mag .* exp (i * ph);
	// rec_inv = [rec(1) rec(length(rec):-1:2)];
	// rec_s = 0.5 * (rec + rec_inv);
	// rec_a = 0.5 * (rec - rec_inv);
	// rec_tmp = real (rec_s) + i * imag (rec_a);
	Buffer &       buf_rec = _buf_list [_buf_rec];
	assert (buf_rec.data () != mag_ptr);
	buf_rec [0] = DataType (mag_ptr [0] * cos (buf_ph [0]));
	for (int pos = 1; pos < len_h; ++pos)
	{
		const DataType ph        = buf_ph [      pos];
		const DataType ph_inv    = buf_ph [len - pos];
		const DataType mag       = mag_ptr [      pos];
		const DataType mag_inv   = mag_ptr [len - pos];
		const DataType rec_r     = DataType (mag     * cos (ph    ));
		const DataType rec_i     = DataType (mag     * sin (ph    ));
		const DataType rec_inv_r = DataType (mag_inv * cos (ph_inv));
		const DataType rec_inv_i = DataType (mag_inv * sin (ph_inv));
		const DataType rec_r_s   =  0.5f * (rec_r + rec_inv_r); // real, symetric
		const DataType rec_i_a   = -0.5f * (rec_i - rec_inv_i); // imag, antisymetric, - for FFTReal
		buf_rec [pos        ]    = rec_r_s;
		buf_rec [pos + len_h]    = rec_i_a;
	}
	buf_rec [len_h] = DataType (mag_ptr [len_h] * cos (buf_ph [len_h]));

   // recu = ifft (rec_tmp);
   Buffer &       buf_recu = _buf_list [_buf_recu];
   _fft_uptr->do_ifft (buf_rec.data (), buf_recu.data ());
   const DataType scale = DataType (1.0 / len);

   // y = recu (1:s);
   for (int pos = 0; pos < trunc_len; ++pos)
   {
      recu_ptr [pos] = buf_recu [pos] * scale;
   }
}



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
