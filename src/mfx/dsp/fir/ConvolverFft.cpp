/*****************************************************************************

        ConvolverFft.cpp
        Author: Laurent de Soras, 2019

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
#include "mfx/dsp/fir/ConvolverFft.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ConvolverFft::ConvolverFft (int nbr_spl, const DataType impulse_ptr [])
:	_fft_sptr ()
,	_impulse_freq ()
,	_input_buf ()
,	_temp_buf ()
,	_output_buf ()
,	_impulse_len (0)
,	_fft_len (0)
,	_block_len (0)
,	_rem_len (0)
{
	set_impulse (nbr_spl, impulse_ptr);
}



bool	ConvolverFft::is_valid () const
{
	return (_fft_len > 0);
}



/*
==============================================================================
Name : set_impulse
Description :
	Set a new impulse. Object reallocations are done only if the FFT length
	had to change since the last time. Buffer is not cleared if FFT size
	doesn't change.
	Buffers addresses should be invalidated by client application.
Input parameters :
	- nbr_spl : Impulse length, in samples. > 1.
	- impulse_ptr : Pointer on the impulse. Impulse data is copied into the
		class data.
Throws : std::vector exception, only if FFT length changed.
==============================================================================
*/

void	ConvolverFft::set_impulse (int nbr_spl, const DataType impulse_ptr [])
{
	assert (nbr_spl > 1);
	assert (impulse_ptr != nullptr);

	_impulse_len = nbr_spl;
	const int      half_len_p2 = fstb::get_next_pow_2 (_impulse_len - 1);
	const int      old_fft_len = _fft_len;
	_fft_len = 2L << half_len_p2;
	assert (_fft_len > _impulse_len);
	assert (_fft_len >= 2 * (_impulse_len - 1));

	// Reallocations, if required
	if (_fft_len != old_fft_len)
	{
		_fft_sptr = std::make_shared <FftType> (_fft_len);
		_impulse_freq.resize (_fft_len);
		_input_buf.resize (_fft_len);
		_temp_buf.resize (_fft_len);
		_output_buf.resize (_fft_len);

		_block_len = _fft_len - _impulse_len + 1;
		_rem_len   = _fft_len - _block_len;

		clear_buffers ();
	}

	// Copy impulse and pad it with 0 to the right
	memcpy (
		&_temp_buf [0],
		impulse_ptr,
		_impulse_len * sizeof (_temp_buf [0])
	);
	memset (
		&_temp_buf [_impulse_len],
		0,
		(_fft_len - _impulse_len) * sizeof (_temp_buf [0])
	);

	// Frequency domain
	_fft_sptr->do_fft (&_impulse_freq [0], &_temp_buf [0]);
	_fft_sptr->rescale (&_impulse_freq [0]);

	assert (is_valid ());
}



int	ConvolverFft::get_block_len () const
{
	assert (is_valid ());

	return _block_len;
}



/*
==============================================================================
Name : get_input_buffer
Description :
	Once the impulse set, get the input buffer for data. The address remains
	constant between to impulse changes.
Returns: Address of the buffer.
Throws : Nothing
==============================================================================
*/

ConvolverFft::DataType *	ConvolverFft::get_input_buffer () const
{
	assert (is_valid ());

	return const_cast <DataType *> (&_input_buf [_rem_len]);
}



/*
==============================================================================
Name : get_output_buffer
Description :
	Once the impulse set, get the output buffer for data. The address remains
	constant between to impulse changes.
Returns: Address of the buffer.
Throws : Nothing
==============================================================================
*/

const ConvolverFft::DataType *	ConvolverFft::get_output_buffer () const
{
	assert (is_valid ());

	return &_output_buf [_rem_len];
}



/*
==============================================================================
Name : process
Description :
	First, user should fill the public input buffer with block_len samples.
	In return, block_len samples are calculated in the output buffer.
Throws : Nothing
==============================================================================
*/

void	ConvolverFft::process ()
{
	assert (is_valid ());

	// Data in frequency domain
	_fft_sptr->do_fft (&_temp_buf [0], &_input_buf [0]);

	// Save old data
	memcpy (
		&_input_buf [0],
		&_input_buf [_block_len],
		_rem_len * sizeof (_input_buf [0])
	);

	// Multiply signals in frequency domain
	const int      half_fft_len = _fft_len >> 1;
	for (int bin = 1; bin < half_fft_len; ++bin)
	{
		const int      bin_imag = bin + half_fft_len;

		const float    h_real = _impulse_freq [bin];
		const float    h_imag = _impulse_freq [bin_imag];

		const float    x_real = _temp_buf [bin];
		const float    x_imag = _temp_buf [bin_imag];

		const float    y_real = x_real * h_real - x_imag * h_imag;
		const float    y_imag = x_imag * h_real + x_real * h_imag;

		_temp_buf [bin] = y_real;
		_temp_buf [bin_imag] = y_imag;
	}
	_temp_buf [0] *= _impulse_freq [0];
	_temp_buf [half_fft_len] *= _impulse_freq [half_fft_len];

	// Back to time domain
	_fft_sptr->do_ifft (&_temp_buf [0], &_output_buf [0]);
}



void	ConvolverFft::clear_buffers ()
{
	const size_t   len = _input_buf.size ();
	memset (&_input_buf [0], 0, len * sizeof (_input_buf [0]));
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
