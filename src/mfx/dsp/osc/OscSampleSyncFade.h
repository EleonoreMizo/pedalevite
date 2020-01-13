/*****************************************************************************

        OscSampleSyncFade.h
        Author: Laurent de Soras, 2019

The pre/post-unroll size may be used to other purpose than interpolation
security, such as sample loop unrolling. Therefore bound checks are loosy.

Template parameters:

- OSC: Sample oscillator. Something like OscSample.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSampleSyncFade_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSampleSyncFade_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"

#include <array>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <typename OSC>
class OscSampleSyncFade
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef OscSampleSyncFade <OSC> ThisType;
	typedef OSC Oscillator;
	typedef typename OSC::SampleTable SampleTable;
	typedef typename OSC::CalcDataType CalcDataType;
	typedef typename OSC::SampleTable::DataType StorageDataType;

	static const int  PITCH_FRAC_BITS = Oscillator::PITCH_FRAC_BITS;

	void           set_sample_data (const SampleTable &sample_data);
	const SampleTable &
	               use_sample_data () const;

	void           set_bandlimit (int32_t limit);
	fstb_FORCEINLINE int32_t
	               get_bandlimit () const;

	void           set_base_pitch (int32_t pitch);
	fstb_FORCEINLINE int32_t
	               get_base_pitch () const;

	fstb_FORCEINLINE void
	               set_master_pitch (int32_t pitch);
	fstb_FORCEINLINE int32_t
	               get_master_pitch () const;

	fstb_FORCEINLINE void
	               set_slave_pitch (int32_t pitch);
	fstb_FORCEINLINE int32_t
	               get_slave_pitch () const;

	void           set_sync_pos (const fstb::FixedPoint &pos);
	void           set_sync_speed (const fstb::FixedPoint &spd);
	int            get_wave_length () const;

	fstb_FORCEINLINE void
	               get_slave_pos (fstb::FixedPoint &pos) const;
	fstb_FORCEINLINE void
	               set_slave_pos (const fstb::FixedPoint &pos);

	void           clear_buffers ();

	fstb_FORCEINLINE CalcDataType
	               process_sample ();
	void           process_block (CalcDataType data_ptr [], int nbr_spl);

	void           wrap_osc_pos ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  PITCH_FRAC_RANGE = 1 << PITCH_FRAC_BITS;
	static const int  PITCH_FRAC_MASK  = PITCH_FRAC_RANGE - 1;

	static const int  BUF_SIZE_L2      = 8;
	static const int  BUF_SIZE         = 1 << BUF_SIZE_L2;
	static const int  BUF_MASK         = BUF_SIZE - 1;

	static const int  NBR_OSC          = 2;

	typedef std::array <CalcDataType, BUF_SIZE> Buffer;
	typedef std::array <Oscillator  , NBR_OSC > OscArray;
	typedef std::array <CalcDataType, NBR_OSC > FadeValArray;

	void           update_master_pitch ();
	fstb_FORCEINLINE void
	               update_sync_pos ();
	fstb_FORCEINLINE void
	               update_sync_speed ();
	fstb_FORCEINLINE void
	               generate_block (CalcDataType data_ptr [], int nbr_spl);
	fstb_FORCEINLINE CalcDataType
	               generate_sample ();
	fstb_FORCEINLINE void
	               check_and_handle_sync_point ();

	OscArray       _osc_arr;
	int            _cur_osc { 0 };      // [0 ; NBR_OSC-1]

	// Reference pitch in octaves for Nyquist frequency, 32:16.
	int32_t        _base_pitch { 0 };

	// Main sync pitch in octaves, 32:16. Used in conjunction with _base_pitch.
	int32_t        _master_pitch { 0 };

	int            _cur_table_len { 0 };

	// Sample distance to the next sync point, relative to the current buffer
	// r/w pos. >= 0
	fstb::FixedPoint
	               _spl_to_next_sync { 0 };

	// Time in sample between two sync. >= 2
	// We set the default to a quite big number because of the
	// _spl_to_next_sync adjustment in update_master_pitch()
	fstb::FixedPoint
						_period { 10000 };

	// Waveform frequency, samples, relative to the current table.
	fstb::FixedPoint
						_wave_freq_spl { 0 };

	// Where the sync occurs in the sample
	fstb::FixedPoint
						_sync_pos { 0 };

	// Where the sync occurs in the sample, relative to the current table
	fstb::FixedPoint
						_sync_pos_table { 0 };

	fstb::FixedPoint
						_sync_speed { 0 };
	fstb::FixedPoint
						_sync_speed_table { 0 };

	// Buffer for fading out waveform generation
	Buffer         _buffer;

	// For each active oscillator, [0 ; 1] if float, [0 ; 0x4000] if integer
	FadeValArray   _fade_val_arr { 0, 0 };

	// Fade increment per destination sample. Depends on the master pitch.
	CalcDataType   _fade_step { 0 };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSampleSyncFade &other) const = delete;
	bool           operator != (const OscSampleSyncFade &other) const = delete;

}; // class OscSampleSyncFade



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSampleSyncFade.hpp"



#endif   // mfx_dsp_osc_OscSampleSyncFade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
