/*****************************************************************************

        OscSampleSyncFade.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_OscSampleSyncFade_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_OscSampleSyncFade_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/Approx.h"
#include "fstb/fnc.h"

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

template <typename OSC>
void	OscSampleSyncFade <OSC>::set_sample_data (const SampleTable &sample_data) noexcept
{
	_osc_arr [0].set_sample_data (sample_data);
	_osc_arr [1].set_sample_data (sample_data);
}



/*
==============================================================================
Name: use_sample_data
Description:
	Use the current sample data, for direct access. There is no guaranty that
	it is valid. However client should ensure it is valid before using the
	"working" member functions.
	There is no non-const function equivalent because the two oscillators must
	have their data synchronized. There is set_sample_data() to modify samples.
Returns: Reference on the data.
Throws: Nothing
==============================================================================
*/

template <typename OSC>
const typename OscSampleSyncFade <OSC>::SampleTable &	OscSampleSyncFade <OSC>::use_sample_data () const noexcept
{
	return _osc_arr [0].use_sample_data ();
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

template <typename OSC>
void	OscSampleSyncFade <OSC>::set_bandlimit (int32_t limit) noexcept
{
	_osc_arr [0].set_bandlimit (limit);
	_osc_arr [1].set_bandlimit (limit);
}



template <typename OSC>
int32_t	OscSampleSyncFade <OSC>::get_bandlimit () const noexcept
{
	return _osc_arr [0].get_bandlimit ();
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_base_pitch (int32_t pitch) noexcept
{
	_base_pitch = pitch;
}



template <typename OSC>
int32_t	OscSampleSyncFade <OSC>::get_base_pitch () const noexcept
{
	return _base_pitch;
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_master_pitch (int32_t pitch) noexcept
{
	assert (pitch < get_base_pitch ());

	if (pitch != _master_pitch)
	{
		_master_pitch = pitch;
		update_master_pitch ();
	}
}



template <typename OSC>
int32_t	OscSampleSyncFade <OSC>::get_master_pitch () const noexcept
{
	return _master_pitch;
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_slave_pitch (int32_t pitch) noexcept
{
	_osc_arr [0].set_pitch (pitch);
	_osc_arr [1].set_pitch (pitch);
	_osc_arr [0].get_table_rate (_wave_freq_spl);
	update_sync_pos ();

	const int      table = _osc_arr [0].get_cur_table ();
	const SampleTable &	table_data = use_sample_data ();
	_cur_table_len = table_data.get_table_len (table);
}



template <typename OSC>
int32_t	OscSampleSyncFade <OSC>::get_slave_pitch () const noexcept
{
	return _osc_arr [0].get_pitch ();
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_sync_pos (const fstb::FixedPoint &pos) noexcept
{
	assert (pos.get_int_val () >= -SampleTable::UNROLL_PRE);
	assert (pos.get_int_val () < _osc_arr [0].use_sample_data ().get_table_len (0) + SampleTable::UNROLL_POST);

	_sync_pos = pos;
	update_sync_pos ();
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_sync_speed (const fstb::FixedPoint &spd) noexcept
{
	_sync_speed = spd;
	update_sync_speed ();
}



/*
==============================================================================
Name: get_wave_length
Description:
	Gives the length of a master cycle within the original sample.
	The data actually used are doubled, because of the fadeout.
Returns: Length in sample, rounded to +oo
Throws: Nothing
==============================================================================
*/

template <typename OSC>
int	OscSampleSyncFade <OSC>::get_wave_length () const noexcept
{
	fstb::FixedPoint  rate;

	_osc_arr [0].get_table_rate (rate);
	rate.shift_left (_osc_arr [0].get_cur_table ());

	const float    period = rate.get_val_flt () * _period.get_val_flt ();

	return (fstb::ceil_int (period));
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::get_slave_pos (fstb::FixedPoint &pos) const noexcept
{
	_osc_arr [_cur_osc].get_playback_pos (pos);
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::set_slave_pos (const fstb::FixedPoint &pos) noexcept
{
	_osc_arr [_cur_osc].set_playback_pos (pos);
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::clear_buffers () noexcept
{
	for (int osc = 0; osc < NBR_OSC; ++osc)
	{
		_fade_val_arr [osc] = 0;
	}
	_spl_to_next_sync.set_val (0, 0);
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

template <typename OSC>
typename OscSampleSyncFade <OSC>::CalcDataType	OscSampleSyncFade <OSC>::process_sample () noexcept
{
	assert (_period.get_val_dbl () > 0);
	assert (_wave_freq_spl.get_val_int64 () > 0);
	assert (_spl_to_next_sync.get_ceil () >= 0);

	check_and_handle_sync_point ();
	const CalcDataType	val = generate_sample ();
	_spl_to_next_sync -= 1;
	_sync_pos         += _sync_speed;
	_sync_pos_table   += _sync_speed_table;

	return val;
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::process_block (CalcDataType data_ptr [], int nbr_spl) noexcept
{
	assert (_period.get_val_dbl () > 0);
	assert (_wave_freq_spl.get_val_int64 () > 0);

	assert (data_ptr != nullptr);
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
		}

		// Sync point ?
		check_and_handle_sync_point ();
	}
	while (pos < nbr_spl);

	// Resync sync pos
	fstb::FixedPoint	full_step (_sync_speed);
	full_step.mul_int (nbr_spl);
	_sync_pos += full_step;
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::wrap_osc_pos () noexcept
{
	const int      old_osc = NBR_OSC-1 - _cur_osc;
	fstb::FixedPoint  pos;
	_osc_arr [old_osc].get_playback_pos_in_cur_table (pos);
	int            pos_int = pos.get_int_val ();

	const int      hpre  = -(SampleTable::UNROLL_PRE  + 1) >> 1;
	const int      hpost =  (SampleTable::UNROLL_POST + 1) >> 1;
	if (pos_int < hpre)
	{
		pos_int %= _cur_table_len;
		if (pos_int < 0)
		{
			pos_int += _cur_table_len;
		}
		pos.set_int_val (pos_int);
		_osc_arr [old_osc].set_playback_pos_in_cur_table (pos);
	}
	else if (pos_int >= _cur_table_len + hpost)
	{
		pos_int %= _cur_table_len;
		pos.set_int_val (pos_int);
		_osc_arr [old_osc].set_playback_pos_in_cur_table (pos);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

template <typename T>
class OscSampleSyncFade_ScaleHelper
{
public:
	static fstb_FORCEINLINE constexpr T  get_scale_val () noexcept;
	static fstb_FORCEINLINE constexpr T  scale (T data, T scale) noexcept;
	static fstb_FORCEINLINE T  invert_float_and_scale (float val) noexcept;
	static fstb_FORCEINLINE T  scale_float (T data, float scale) noexcept;
};

template <typename T>
constexpr T	OscSampleSyncFade_ScaleHelper <T>::get_scale_val () noexcept
{
	return 1;
}



template <>
inline constexpr int32_t	OscSampleSyncFade_ScaleHelper <int32_t>::get_scale_val () noexcept
{
	return 1 << 14;
}



template <typename T>
constexpr T	OscSampleSyncFade_ScaleHelper <T>::scale (T data, T scale) noexcept
{
	return data * scale;
}



// data = N:N, scale = 16:14, output = N:N
// N should be less than 18
// Mutiplication intermediate result is stored on 32 bit
template <>
inline constexpr int32_t	OscSampleSyncFade_ScaleHelper <int32_t>::scale (int32_t data, int32_t scale) noexcept
{
	return (data * scale) >> 14;
}



template <typename T>
T	OscSampleSyncFade_ScaleHelper <T>::invert_float_and_scale (float val) noexcept
{
	return 1 / val;
}



template <>
inline int32_t	OscSampleSyncFade_ScaleHelper <int32_t>::invert_float_and_scale (float val) noexcept
{
	return fstb::conv_int_fast ((1 << 14) / val);
}



template <typename T>
T	OscSampleSyncFade_ScaleHelper <T>::scale_float (T data, float scale) noexcept
{
	return data * scale;
}



template <>
inline int32_t	OscSampleSyncFade_ScaleHelper <int32_t>::scale_float (int32_t data, float scale) noexcept
{
	return fstb::conv_int_fast (data * scale);
}



// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -



template <typename OSC>
void	OscSampleSyncFade <OSC>::update_master_pitch () noexcept
{
	const int      rel_pitch = _base_pitch - _master_pitch;

	// Convert pitch into linear period
	static_assert (Oscillator::PITCH_FRAC_BITS == 16, "");
	uint32_t       frac_part =
		fstb::Approx::fast_partial_exp2_int_16_to_int_32_4th (rel_pitch);

	const int      log_2_int = (rel_pitch >> Oscillator::PITCH_FRAC_BITS) + 2;
	const float    old_period_flt = _period.get_val_flt ();
	_period.set_val_int64 (int64_t (frac_part) << log_2_int);

	// Compute the new step
	const float    period_flt = _period.get_val_flt ();
	_fade_step = OscSampleSyncFade_ScaleHelper <
		CalcDataType
	>::invert_float_and_scale (period_flt);

	// Adjust the number of remaining samples before next sync
	assert (old_period_flt > 0);
	const float    ratio = period_flt / old_period_flt;
	assert (ratio > 0);
	_spl_to_next_sync.mul_flt (ratio);
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::update_sync_pos () noexcept
{
	SampleTable::convert_position (
		_sync_pos_table,
		_sync_pos,
		_osc_arr [0].get_cur_table ()
	);
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::update_sync_speed () noexcept
{
	SampleTable::convert_position (
		_sync_speed_table,
		_sync_speed,
		_osc_arr [0].get_cur_table ()
	);
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::generate_block (CalcDataType data_ptr [], int nbr_spl) noexcept
{
	assert (nbr_spl > 0);
	assert (NBR_OSC == 2);

	const int      old_osc = NBR_OSC-1 - _cur_osc;
	FadeValArray   fade_val_arr;
	fade_val_arr [0] = _fade_val_arr [_cur_osc];
	fade_val_arr [1] = _fade_val_arr [old_osc];
	CalcDataType   fade_step = _fade_step;

	int            block_pos = 0;
	do
	{
		const int      block_len =
			std::min (nbr_spl - block_pos, int (BUF_SIZE));

		// Generate waveforms
		_osc_arr [_cur_osc].process_block (data_ptr, block_len);
		_osc_arr [ old_osc].process_block (&_buffer [0], block_len);

		// Fade
		int            pos = 0;
		do
		{
			const auto     s0 =
				OscSampleSyncFade_ScaleHelper <CalcDataType>::scale (
					data_ptr [pos], fade_val_arr [0]
				);
			const auto     s1 =
				OscSampleSyncFade_ScaleHelper <CalcDataType>::scale (
					_buffer  [pos], fade_val_arr [1]
				);
			data_ptr [pos]    = s0 + s1;

			fade_val_arr [0] += fade_step;
			fade_val_arr [1] -= fade_step;
			++pos;
		}
		while (pos < block_len);

		block_pos += block_len;
		data_ptr  += block_len;
	}
	while (block_pos < nbr_spl);

	_fade_val_arr [_cur_osc] = fade_val_arr [0];
	_fade_val_arr [ old_osc] = fade_val_arr [1];
}



template <typename OSC>
typename OscSampleSyncFade <OSC>::CalcDataType	OscSampleSyncFade <OSC>::generate_sample () noexcept
{
	const int      old_osc = NBR_OSC-1 - _cur_osc;

	// Generate waveforms
	const CalcDataType   temp_0 = _osc_arr [_cur_osc].process_sample ();
	const CalcDataType   temp_1 = _osc_arr [ old_osc].process_sample ();

	// Fade
	const CalcDataType   val    =
		  OscSampleSyncFade_ScaleHelper <CalcDataType>::scale (temp_0, _fade_val_arr [_cur_osc])
		+ OscSampleSyncFade_ScaleHelper <CalcDataType>::scale (temp_1, _fade_val_arr [ old_osc]);

	_fade_val_arr [_cur_osc] += _fade_step;
	_fade_val_arr [ old_osc] -= _fade_step;

	return val;
}



template <typename OSC>
void	OscSampleSyncFade <OSC>::check_and_handle_sync_point () noexcept
{
	if (_spl_to_next_sync.get_ceil () == 0)
	{
		// Change the current oscillator
		const int      old_osc = _cur_osc;
		_cur_osc = NBR_OSC-1 - old_osc;

		// We need that >> keeps the sign
		static_assert ((int64_t (-2) >> 1) == -1, "");

		// Calculate oscillator phase for the next generated sample
		const float    phase_inc_flt =
			_spl_to_next_sync.get_val_flt () * _wave_freq_spl.get_val_flt ();
		const fstb::FixedPoint	phase_inc (phase_inc_flt);

		// Setup the fade values
		const CalcDataType   offset =
			OscSampleSyncFade_ScaleHelper <CalcDataType>::scale_float (_fade_step, -phase_inc_flt);
		_fade_val_arr [_cur_osc] = offset;
		_fade_val_arr [ old_osc] =
			OscSampleSyncFade_ScaleHelper <CalcDataType>::get_scale_val () - offset;

		// Advance the oscillator to the sampled sync point
		fstb::FixedPoint  new_phase (_sync_pos_table - phase_inc);
		_osc_arr [_cur_osc].set_playback_pos_in_cur_table (new_phase);

		// Compute next sync point
		_spl_to_next_sync += _period;
		assert (_spl_to_next_sync.get_int_val () >= 0);
	}
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_OscSampleSyncFade_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
