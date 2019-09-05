/*****************************************************************************

        OscWavetableSyncHard.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscWavetableSyncHard_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetableSyncHard_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Scale.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::OscWavetableSyncHard ()
{
	clear_buffers ();
}



/*
==============================================================================
Name: use_osc
Description:
	Gives access to the main oscillator, for extra-configuration if required.
Returns: the oscillator object.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::Oscillator &	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::use_osc ()
{
	return _osc;
}



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
const typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::Oscillator &	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::use_osc () const
{
	return _osc;
}



/*
==============================================================================
Name: set_wavetable
Description:
	Links the oscillator to a wavetable. The wavetable should be filled by
	the caller. This can be done before or after the call to this function, but
	before any call to sample-generating functions.
	This is a mandatory call before using the oscillator.
Input parameters:
	- wavetable: the wavetable.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_wavetable (const WavetableDataType &wavetable)
{
	_osc.set_wavetable (wavetable);
}



/*
==============================================================================
Name: use_wavetable
Description:
	Returns the attached wavetable. A wavetable must have been set previously.
Returns: A reference on the wavetable.
Throws: Nothing
==============================================================================
*/


template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
const typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::WavetableDataType &	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::use_wavetable () const
{
	return _osc.use_wavetable ();
}



/*
==============================================================================
Name: set_steptable
Description:
	Links an antialiased step table to the oscillator. The steptable should
	be filled by the caller at any time before calling any sample-generating
	functions.
	This is a mandatory call before using the oscillator.
Input parameters:
	- steptable: A reference on the steptable.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_steptable (const AntialiasedStep &steptable)
{
	_step_ptr = &steptable;
}



/*
==============================================================================
Name: use_steptable
Description:
	Access the antialiased step table. A steptable must have been set
	previously.
Returns: The step table.
Throws: ?
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
const typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::AntialiasedStep &	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::use_steptable () const
{
	assert (_step_ptr != 0);

	return *_step_ptr;
}



/*
==============================================================================
Name: set_base_pitch
Description:
	Sets the reference pitch value for the highest possible output frequency
	(= Nyquist). Pitch is logarithmic: each 1 << PITCH_FRAC_BITS more is one
	octave up.
	The current pitch nor generation parameter are not updated, so the
	oscillator may be in an invalid state after a call to this function.
	The caller must ensure that the current pitch is set to a legal value after
	having set the reference and before calling any	other function.
	Default pitch is 0.
Input parameters:
	- pitch: reference pitch. There is almost no constraint on its value, as
		it is just a reference.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_base_pitch (int32_t pitch)
{
	_base_pitch = pitch;
	_osc.set_base_pitch (pitch);
}



/*
==============================================================================
Name: get_base_pitch
Description:
	Returns the reference pitch value. It is defaulted to 0 if it has never
	been set previously.
Returns: The reference pitch value
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
int32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::get_base_pitch () const
{
	return _base_pitch;
}



/*
==============================================================================
Name: set_pitch
Description:
	Sets the current oscillator pitch. The reference pitch value is the Nyquist
	frequency. When 1 << PITCH_FRAC_BITS is subtracted to the pitch, the
	corresponding frequency is decreased by one octave.
	Here, the pitch affects the rate at which the slave oscillator is
	retrigged.
Input parameters:
	- pitch: pitch to set. Should be less than the reference pitch.
Throws: Nothing
Returns:
	The fractional part of the frequency (2 ^ pitch_frac), on a 32-bit scale
	(0x80000000 to 0xFFFFFFFF).
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_pitch (int32_t pitch)
{
	// freq/F_s = 2 ^ (rel_pitch / (1 << PITCH_FRAC_BITS) - 1)
	// period_spl = 1 / (freq/F_s)
	//            = 2 ^ (1 - rel_pitch / (1 << PITCH_FRAC_BITS))

	assert (pitch < _base_pitch);

	_pitch = pitch;

	const int      rel_pitch_neg = _base_pitch - pitch;

	static_assert (PITCH_FRAC_BITS == 16, "");
	const uint32_t per_frac (
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch_neg)
	);

	// +2 instead of +1 because the fractional part is 2 ^ (x - 1)
	const int      per_l2_int =
		(rel_pitch_neg >> Oscillator::PITCH_FRAC_BITS) + 2;

	const fstb::FixedPoint  period_old (_period);
	_period.set_val_int64 (int64_t (per_frac) << per_l2_int);

	// Fixes the next sync point
	_spl_to_next_sync += _period;
	_spl_to_next_sync -= period_old;
	if (_spl_to_next_sync.get_val_int64 () < -int64_t (0xFFFFFFFFU))
	{
		_spl_to_next_sync.clear ();
	}

}



/*
==============================================================================
Name: get_pitch
Description:
	Returns the current pitch.
Returns: The pitch (slave retrig rate)
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
int32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::get_pitch () const
{
	return _pitch;
}



/*
==============================================================================
Name: set_pitch_slave
Description:
	Sets the pitch of the internal oscillator.
Input parameters:
	- pitch: pitch to set. Should be less than the reference pitch.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_pitch_slave (int32_t pitch)
{
	// freq/F_s = 2 ^ (rel_pitch / (1 << PITCH_FRAC_BITS) - 1)

	assert (pitch < _base_pitch);

	const uint32_t freq_frac   = _osc.set_pitch (pitch);
	const int      rel_pitch   = pitch - _base_pitch;
	const int      freq_l2_int = rel_pitch >> Oscillator::PITCH_FRAC_BITS;
	_freq_slave.set_val_int64 (freq_frac >> -freq_l2_int);
}



/*
==============================================================================
Name: get_pitch_slave
Description:
	Retrieve the current slave oscillator pitch.
Returns: The slave pitch.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
int32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::get_pitch_slave () const
{
	return _osc.get_pitch ();
}



/*
==============================================================================
Name: reset_phase
Description:
	Sets the current phase to 0, meaning that the slave oscillator phase is
	immediately reset.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::reset_phase ()
{
	_spl_to_next_sync.clear ();
}



/*
==============================================================================
Name: set_phase
Description:
	Sets the phase of the master oscillator.
Input parameters:
	- phase: New phase, in range [0 ; (1 << 32) - 1]
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_phase (uint32_t phase)
{
	const int64_t  per20 = _period.get_val_int64 () >> 12;
	const uint16_t ph12  = (-phase) >> (32 - 12);
	_spl_to_next_sync.set_val_int64 (per20 * ph12);
}



/*
==============================================================================
Name: get_phase
Description:
	Gets the current phase.
Returns: Current phase, ranging in [0 ; 0xFFFFFFFF].
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
uint32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::get_phase () const
{
	const int64_t  per20 = _period.get_val_int64 () >> 12;
	const int64_t  pos32 = _spl_to_next_sync.get_val_int64 ();
	const uint32_t ph12  = uint32_t (pos32 / per20);

	return -ph12 << (32 - 12);
}



/*
==============================================================================
Name: set_sync_pos
Description:
	Sets the internal phase at which the slave oscillator is reset when a
	synchronisation happens. This phase is defaulted to 0 if not set manually.
Input parameters:
	- pos: Slave phase on a 1 << 32 scale.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::set_sync_pos (uint32_t pos)
{
	_sync_pos = pos;
}



/*
==============================================================================
Name: get_sync_pos
Description:
	Rertrieve the current reset value for the slave oscillator phase.
Returns: The slave reset phase, on a 1 << 32 scale.
Throws: Noting
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
uint32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::get_sync_pos () const
{
	return _sync_pos;
}



/*
==============================================================================
Name: clear_buffers
Description:
	Resets the oscillator state.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::clear_buffers ()
{
	_spl_to_next_sync.clear ();
	_buffer.fill (DataType (0));
	_buf_pos = 0;
}



/*
==============================================================================
Name: process_sample
Description:
	Outputs a sample and updates the oscillator state.
Returns: the oscillator output
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::DataType	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::process_sample ()
{
	assert (_spl_to_next_sync.get_ceil () >= 0);

	check_and_handle_sync_point ();

	return generate_sample ();
}



/*
==============================================================================
Name: process_block
Description:
	Outputs a block of samples and updates the oscillator state.
Input parameters:
	- nbr_spl: Number of samples to output. > 0.
Output parameters:
	- dest_ptr: Pointer on the output buffer. Not null.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::process_block (DataType dst_ptr [], int nbr_spl)
{
	assert (_spl_to_next_sync.get_ceil () >= 0);
	assert (_step_ptr != 0);
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		// Generates slave oscillator output until the next sync point.
		const int      distance = _spl_to_next_sync.get_ceil ();
		const int      gen_len  = std::min (distance, nbr_spl - pos);
		if (gen_len > 0)
		{
			generate_block (&dst_ptr [pos], gen_len);
			pos += gen_len;
		}

		// Sync point?
		check_and_handle_sync_point ();
	}
	while (pos < nbr_spl);
}



/*
==============================================================================
Name: conv_freq_to_pitch
Description:
	Utility function converting a frequency and a sampling rate into a pitch
	value usable in set_pitch().
Input parameters:
	- freq: Oscillator frequency, Hz, ]0 ; Fs / 2[
	- fs: Sampling rate, Hz, > 0.
Returns: The computed pitch
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
int32_t	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::conv_freq_to_pitch (float freq, float fs) const
{
	return _osc.conv_freq_to_pitch (freq, fs);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
typename OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::DataType	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::generate_sample ()
{
	const DataType val_osc = _osc.process_sample ();

	const int      read_pos  = (_buf_pos - HALF_PHASE_LEN) & BUF_MASK;
	const int      clean_pos = (_buf_pos + HALF_BUF_SIZE ) & BUF_MASK;

	const DataType val_out = _buffer [read_pos];
	_buffer [_buf_pos ] += val_osc;
	_buffer [clean_pos]  = DataType (0);

	++_buf_pos;
	_buf_pos &= BUF_MASK;
	_spl_to_next_sync -= 1;

	return val_out;
}



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::generate_block (DataType dst_ptr [], int nbr_spl)
{
	assert (dst_ptr != 0);
	assert (nbr_spl > 0);

	const int      BUF_SEL_BIT = BUF_MASK - HALF_BUF_MASK;

	dly::RingBufVectorizer	rbv (HALF_BUF_SIZE);
	int            half_buf_write = _buf_pos & BUF_SEL_BIT;
	const int      beg_read_pos   = (_buf_pos - HALF_PHASE_LEN) & BUF_MASK;
	int            half_buf_read  = beg_read_pos & BUF_SEL_BIT;
	int            pos            = 0;

	rbv.start (nbr_spl, _buf_pos & HALF_BUF_MASK, beg_read_pos & HALF_BUF_MASK);
	for ( ; rbv.end (); rbv.next ())
	{
		const int      len       = rbv.get_seg_len ();
		const int      write_pos = rbv.get_curs_pos (0);
		const int      read_pos  = rbv.get_curs_pos (1);

		const int      byte_len  = len * sizeof (_buffer [0]);

		// Generate on one half-buffer
		_osc.process_block_mix (&_buffer [write_pos + half_buf_write], len);
		const int      pos_hbr = read_pos + half_buf_read;
		memcpy (&dst_ptr [pos], &_buffer [pos_hbr], byte_len);

		// Clean on the other one
		const int      half_buf_clean = half_buf_write ^ BUF_SEL_BIT;
		const int      pos_hbc        = write_pos + half_buf_clean;
		memset (&_buffer [pos_hbc], 0, byte_len);

		// Swap buffers
		if (write_pos + len == HALF_BUF_SIZE)
		{
			half_buf_write ^= BUF_SEL_BIT;
		}
		if (read_pos  + len == HALF_BUF_SIZE)
		{
			half_buf_read ^= BUF_SEL_BIT;
		}

		pos += len;
	}

	_buf_pos = (_buf_pos + nbr_spl) & BUF_MASK;

	_spl_to_next_sync -= nbr_spl;
}



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::check_and_handle_sync_point ()
{
	if (_spl_to_next_sync.get_val_int64 () <= 0)
	{
		assert (_spl_to_next_sync.get_int_val () >= -1);
		generate_step ();
		_spl_to_next_sync += _period;
	}
}



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::generate_step ()
{
	// Calculates the oscillator phase for the next generated sample
	// _spl_to_next_sync * _freq_slave
	// _spl_to_next_sync is in ]-1; 0  ] 0xFFFFFFFFxxxxxxxx or 0x000...000
	// _freq_slave       is in [ 0; 0.5] 0x00000000xxxxxxxx
	const uint32_t stns_neg = -int32_t (_spl_to_next_sync.get_frac_val ());
	const uint32_t f_slave  = _freq_slave.get_frac_val ();
	const uint32_t phase_inc_neg = fstb::Scale <32>::mul (f_slave, stns_neg);

	const uint32_t phase_syn = _sync_pos;

	// Advances the oscillator to the sampled sync point
	const uint32_t phase_cur = _osc.get_phase ();
	const uint32_t phase_end = phase_cur - phase_inc_neg;
	const uint32_t phase_new = phase_syn + phase_inc_neg;

	// Gets the step values
	const DataType step_beg  = _osc.get_sample_at_phase (phase_end);
	_osc.set_phase (phase_new);
	const DataType step_end  = _osc.get_sample_at_phase (phase_syn);

	// Generates the antialiased step
	const DataType step_amp  = step_end - step_beg;
	add_step (step_amp, stns_neg);
}



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
void	OscWavetableSyncHard <
	OSC, STPPLEN, STPNPL2, STPLVL2
>::add_step (DataType step_amp, uint32_t stns_neg)
{
	const int      phase_idx    = stns_neg >> (32 - NBR_PHASES_L2);
	const DataType *  phase_ptr = _step_ptr->use_table (phase_idx);
	const int      buf_pos_base = _buf_pos - HALF_PHASE_LEN;
	for (int pos = 0; pos < PHASE_LEN; ++pos)
	{
		const DataType base = phase_ptr [pos];
		const DataType val  = fstb::Scale <PHASE_LEVEL_L2>::mul (base, step_amp);
		_buffer [(buf_pos_base + pos) & BUF_MASK] += val;
	}
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscWavetableSyncHard_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
