/*****************************************************************************

        OscSampleSyncHard.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSampleSyncHard_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSampleSyncHard_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"
#include "mfx/dsp/dly/RingBufVectorizer.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename OSC, int STPPLEN, int STPNPL2>
OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::OscSampleSyncHard ()
:	_osc ()
,	_step_ptr (0)
,	_base_pitch (0)
,	_master_pitch (0)
,	_spl_to_next_sync (0)
,	_period (0)
,	_wave_freq_spl (0)
,	_sync_pos (0)
,	_sync_pos_table (0)
,	_sync_speed (0)
,	_sync_speed_table (0)
,	_buffer ()
,	_buf_pos (0)
{
	assert (PHASE_LEN <= 1024);
	assert ((PHASE_LEN & 1) == 0);

	clear_buffers ();
}



/*
==============================================================================
Name: set_sample_data
Description:
	Set the oscillator sample data. Data should be valid to be set.
Input parameters:
	- sample_data: reference on the data. It is copied from the original,
		therefore the original can be discarded after the call.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_sample_data (const SampleTable &sample_data)
{
	_osc.set_sample_data (sample_data);
}



/*
==============================================================================
Name: use_sample_data
Description:
	Use the current sample data, for direct access. There is no guaranty that
	it is valid. However client should ensure it is valid before using the
	"working" member functions.
Returns: Reference on the data.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
const typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::SampleTable &	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::use_sample_data () const
{
	return _osc.use_sample_data ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::SampleTable &	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::use_sample_data ()
{
	return _osc.use_sample_data ();
}



/*
==============================================================================
Name: set_steptable
Description:
	Set the antialiased step data for sync operations. Unlike the sample data,
	it takes only the reference on the AntialiasedObject (which contains the
	real data), therefore client should keep the parameter object as long as
	it is used by the OscSampleSyncHard object.
	Antialiased step must be set prior any "working" function call.
Input/output parameters:
	- steptable: table with antialiased step.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_steptable (AntialiasedStep &steptable)
{
	assert (&steptable != 0);

	_step_ptr = &steptable;
}



/*
==============================================================================
Name: use_steptable
Description:
	Get a reference on the antialiased steptable. It should be called only
	after at least one steptable set, because the object is created without
	steptable.
Returns: The steptable reference.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
const typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::AntialiasedStep &	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::use_steptable () const
{
	assert (_step_ptr != 0);

	return *_step_ptr;
}



template <typename OSC, int STPPLEN, int STPNPL2>
typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::AntialiasedStep &	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::use_steptable ()
{
	assert (_step_ptr != 0);

	return *_step_ptr;
}



/*
==============================================================================
Name: set_bandlimit
Description:
	Set the frequency range for the upper harmonic. For example:
	-0 * 0x10000 -> [Nyquist   ; Nyquist*2]. Gives 1-fold aliasing
	-1 * 0x10000 -> [Nyquist/2 ; Nyquist  ]. No aliasing, but halved bandwidth.
	This value is well-suited for x2 oversampling, but something a bit higher
	could be better to avoid table-switching glitches.
Input parameters:
	- limit: Limit in octaves, relative to Nyquist, 32:16. <= 0.
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_bandlimit (int32_t limit)
{
	_osc.set_bandlimit (limit);
}



template <typename OSC, int STPPLEN, int STPNPL2>
int32_t	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_bandlimit () const
{
	return _osc.get_bandlimit ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_base_pitch (int32_t pitch)
{
	_base_pitch = pitch;
}



template <typename OSC, int STPPLEN, int STPNPL2>
int32_t	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_base_pitch () const
{
	return _base_pitch;
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_master_pitch (int32_t pitch)
{
	assert (pitch < get_base_pitch ());

	_master_pitch = pitch;
	update_master_pitch ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
int32_t	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_master_pitch () const
{
	return _master_pitch;
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_slave_pitch (int32_t pitch)
{
	_osc.set_pitch (pitch);
	_osc.get_table_rate (_wave_freq_spl);
	update_sync_pos ();
	update_sync_speed ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
int32_t	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_slave_pitch () const
{
	return _osc.get_pitch ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_sync_pos (const fstb::FixedPoint &pos)
{
	assert (pos.get_int_val () >= -SampleTable::UNROLL_PRE);
	assert (pos.get_int_val () < _osc.use_sample_data ().get_table_len (0) + SampleTable::UNROLL_POST);

	_sync_pos = pos;
	update_sync_pos ();
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_sync_speed (const fstb::FixedPoint &spd)
{
	_sync_speed = spd;
	update_sync_speed ();
}



/*
==============================================================================
Name: get_wave_length
Description:
	Gives the length of a master cycle within the original sample.
Returns: Length in sample, rounded to +oo
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
int	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_wave_length () const
{
	fstb::FixedPoint  rate;

	_osc.get_table_rate (rate);
	rate.shift_left (_osc.get_cur_table ());

	const float    period = rate.get_val_flt () * _period.get_val_flt ();

	return fstb::ceil_int (period);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::get_slave_pos (fstb::FixedPoint &pos) const
{
	_osc.get_playback_pos (pos);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::set_slave_pos (const fstb::FixedPoint &pos)
{
	_osc.set_playback_pos (pos);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::clear_buffers ()
{
	memset (
		&_buffer [0],
		0,
		_buffer.size () * sizeof (_buffer [0])
	);

	_buf_pos = 0;
	_spl_to_next_sync.set_val (0, 0x00000001);
}



/*
==============================================================================
Name: process_sample
Description:
	Generate a sample and move sample index according to the sample speed.
	Bound checks are loosy, so client must ensure that the position is really
	correct.
Returns: Generated data
Throws: Nothing
==============================================================================
*/

template <typename OSC, int STPPLEN, int STPNPL2>
typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::CalcDataType	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::process_sample ()
{
	assert (_step_ptr != 0);
	assert (_period.get_val_dbl () > 0);
	assert (_wave_freq_spl.get_val_int64 () > 0);
	assert (_spl_to_next_sync.get_ceil () >= 0);

	check_and_handle_sync_point ();
	const CalcDataType   val = generate_sample ();
	_spl_to_next_sync -= 1;
	_sync_pos         += _sync_speed;
	_sync_pos_table   += _sync_speed_table;

	return val;
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::process_block (CalcDataType data_ptr [], int nbr_spl)
{
	assert (_step_ptr != 0);
	assert (_period.get_val_dbl () > 0);
	assert (_wave_freq_spl.get_val_int64 () > 0);

	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	int            pos = 0;
	do
	{
		// Generate waves until the next sync point.
		const int      distance = _spl_to_next_sync.get_ceil ();
		const int      gen_len = std::min (distance, nbr_spl - pos);
		if (gen_len > 0)
		{
			generate_block (&data_ptr [pos], gen_len);
			pos += gen_len;
			_spl_to_next_sync -= gen_len;

			fstb::FixedPoint	full_step (_sync_speed_table);
			full_step.mul_int (gen_len);
			_sync_pos_table  += full_step;

			// We can easily get out of the table if the sync speed is high,
			// so we have to fix the sync position if we don't want to crash.
			fix_sync_pos_table ();
		}

		// Sync point ?
		check_and_handle_sync_point ();
	}
	while (pos < nbr_spl);

	// Resync sync pos
	fstb::FixedPoint  full_step (_sync_speed);
	full_step.mul_int (nbr_spl);
	_sync_pos += full_step;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
static fstb_FORCEINLINE T	OscSampleSync_scale (T data, T scale)
{
	return data * scale;
}



// data = N:N, scale = 16:14, output = N:N
// N should be less than 18
// Mutiplication intermediate result is stored on 32 bit
template <>
fstb_FORCEINLINE int32_t	OscSampleSync_scale (int32_t data, int32_t scale)
{
	return (data * scale) >> (16-2);
}



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::update_master_pitch ()
{
	const int      rel_pitch = _base_pitch - _master_pitch;

	assert (Oscillator::PITCH_FRAC_BITS == 16);
	uint32_t       frac_part =
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch);

	const int      log_2_int =
		(rel_pitch >> Oscillator::PITCH_FRAC_BITS) + 2;

	const fstb::FixedPoint  old_period (_period);
	_period.set_val_int64 (int64_t (frac_part) << log_2_int);

	fstb::FixedPoint  period_dif (_period);
	period_dif -= old_period;
	_spl_to_next_sync += period_dif;
	if (_spl_to_next_sync.get_int_val () < 0)
	{
		_spl_to_next_sync.set_val (0, 0x00000001);
	}
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::update_sync_pos ()
{
	SampleTable::convert_position (
		_sync_pos_table,
		_sync_pos,
		_osc.get_cur_table ()
	);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::update_sync_speed ()
{
	SampleTable::convert_position (
		_sync_speed_table,
		_sync_speed,
		_osc.get_cur_table ()
	);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::generate_block (CalcDataType data_ptr [], int nbr_spl)
{
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
		memcpy (&data_ptr [pos], &_buffer [pos_hbr], byte_len);

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
}



template <typename OSC, int STPPLEN, int STPNPL2>
typename OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::CalcDataType	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::generate_sample ()
{
	const CalcDataType   temp = _osc.process_sample ();

	const int      read_pos  = (_buf_pos - HALF_PHASE_LEN) & BUF_MASK;
	const int      clean_pos = (_buf_pos + HALF_BUF_SIZE ) & BUF_MASK;

	const CalcDataType   val = _buffer [read_pos];
	_buffer [_buf_pos ] += temp;
	_buffer [clean_pos]  = 0;

	++_buf_pos;
	_buf_pos &= BUF_MASK;

	return val;
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::check_and_handle_sync_point ()
{
	if (_spl_to_next_sync.get_ceil () == 0)
	{
		// We need that >> keeps the sign
		assert ((int64_t (-2) >> 1) == -1);

		// Calculate oscillator phase for the next generated sample
		const float    phase_inc_flt =
			_spl_to_next_sync.get_val_flt () * _wave_freq_spl.get_val_flt ();
		const fstb::FixedPoint  phase_inc (phase_inc_flt);

		// Advance the oscillator to the sampled sync point and get step values
		fstb::FixedPoint	end_phase (fstb::FixedPoint::NoInit::NO);
		_osc.get_playback_pos_in_cur_table (end_phase);
		end_phase += phase_inc;
		CalcDataType   step_beg = _osc.get_sample_in_cur_table (end_phase);

		fstb::FixedPoint	new_phase (_sync_pos_table - phase_inc);
		_osc.set_playback_pos_in_cur_table (new_phase);
		CalcDataType   step_end = _osc.get_sample_in_cur_table (_sync_pos_table);

		// Generate the antialiased step
		const CalcDataType   step_amplitude = step_end - step_beg;
		add_step (step_amplitude);

		// Compute next sync point
		_spl_to_next_sync += _period;
		assert (_spl_to_next_sync.get_int_val () >= 0);
	}
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::add_step (CalcDataType step_amplitude)
{
	assert (_step_ptr != 0);
	assert (_spl_to_next_sync.get_ceil () == 0);

	const int      shift_int32_to_phase = 32 - AntialiasedStep::NBR_PHASES_LOG2;
	const int      phase_32 = -int32_t (_spl_to_next_sync.get_frac_val ());	// Cast removes a warning. We only manipulate the 32 lowest bits here.	
	const int      phase = (phase_32 >> shift_int32_to_phase) & PHASE_MASK;
	int            write_pos = _buf_pos - HALF_PHASE_LEN;
	int            pos       = 0;
	do
	{
		const CalcDataType   val        = _step_ptr->get_sample (phase, pos);
		const CalcDataType   scaled_val =
			OscSampleSync_scale (val, step_amplitude);
		const int      wrapped_pos = (write_pos + pos) & BUF_MASK;
		_buffer [wrapped_pos] += scaled_val;

		++pos;
	}
	while (pos < PHASE_LEN);
}



template <typename OSC, int STPPLEN, int STPNPL2>
void	OscSampleSyncHard <OSC, STPPLEN, STPNPL2>::fix_sync_pos_table ()
{
	int            spt_int     = _sync_pos_table.get_int_val ();
	const int      table_index = _osc.get_cur_table ();
	const SampleTable &  spl_table = _osc.use_sample_data ();
	const int      table_len   = spl_table.get_table_len (table_index);

	if (spt_int < 0)
	{
		spt_int %= table_len;
		spt_int += table_len;
		_sync_pos_table.set_int_val (spt_int);
	}
	else if (spt_int >= table_len)
	{
		spt_int %= table_len;
		_sync_pos_table.set_int_val (spt_int);
	}
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSampleSyncHard_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
