/*****************************************************************************

        OscSampleSyncHard.h
        Author: Laurent de Soras, 2019

The pre/post-unroll size may be used to other purpose than interpolation
security, such as sample loop unrolling. Therefore bound checks are loosy.

Antialiased step formula f(t):

f(t) = Integral (sinc (t), -oo...t) - Step (t);
t = time in samples, generally not integer.

Step (t) = 0 if t < 0
Step (t) = 1 if t >= 0
Integral (sinc (t), -oo...t) is supposed to be 0 for t = -oo and 1 for t = +oo
f should be antisymmetric.

Template parameters:

- OSC: Sample oscillator. Something like OscSample.
- STPPLEN: Size of the table for antialiased step (integrated sinc). Should
	be even and <= 1024
- STPNPL2: Log2 of the number of phase for the antialiased step.


--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSampleSyncHard_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSampleSyncHard_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"
#include "mfx/dsp/osc/WavetablePolyphaseData.h"

#include <array>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <typename OSC, int STPPLEN, int STPNPL2>
class OscSampleSyncHard
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef OscSampleSyncHard <OSC, STPPLEN, STPNPL2> ThisType;
	typedef OSC Oscillator;
	typedef typename OSC::SampleTable SampleTable;
	typedef typename OSC::CalcDataType CalcDataType;
	typedef typename OSC::SampleTable::DataType StorageDataType;
	typedef WavetablePolyphaseData <STPPLEN, STPNPL2, StorageDataType> AntialiasedStep;

	static const int  PITCH_FRAC_BITS = Oscillator::PITCH_FRAC_BITS;

	void           set_sample_data (const SampleTable &sample_data) noexcept;
	const SampleTable &
	               use_sample_data () const noexcept;
	SampleTable &	use_sample_data () noexcept;

	void           set_steptable (AntialiasedStep &steptable) noexcept;
	const AntialiasedStep &
	               use_steptable () const noexcept;
	AntialiasedStep &
	               use_steptable () noexcept;

	void           set_bandlimit (int32_t limit) noexcept;
	fstb_FORCEINLINE int32_t
	               get_bandlimit () const noexcept;

	void           set_base_pitch (int32_t pitch) noexcept;
	fstb_FORCEINLINE int32_t
	               get_base_pitch () const noexcept;

	fstb_FORCEINLINE void
	               set_master_pitch (int32_t pitch) noexcept;
	fstb_FORCEINLINE int32_t
	               get_master_pitch () const noexcept;

	fstb_FORCEINLINE void
	               set_slave_pitch (int32_t pitch) noexcept;
	fstb_FORCEINLINE int32_t
	               get_slave_pitch () const noexcept;

	void           set_sync_pos (const fstb::FixedPoint &pos) noexcept;
	void           set_sync_speed (const fstb::FixedPoint &spd) noexcept;
	int            get_wave_length () const noexcept;

	fstb_FORCEINLINE void
	               get_slave_pos (fstb::FixedPoint &pos) const noexcept;
	fstb_FORCEINLINE void
	               set_slave_pos (const fstb::FixedPoint &pos) noexcept;

	void           clear_buffers () noexcept;

	fstb_FORCEINLINE CalcDataType
	               process_sample () noexcept;
	void           process_block (CalcDataType data_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  PITCH_FRAC_RANGE = 1 << PITCH_FRAC_BITS;
	static const int  PITCH_FRAC_MASK  = PITCH_FRAC_RANGE - 1;

	static const int  PHASE_LEN      = STPPLEN;
	static const int  HALF_PHASE_LEN = PHASE_LEN / 2;
	static const int  HALF_BUF_SIZE  = ((PHASE_LEN > 63) ? ((PHASE_LEN > 255) ? 1024 : 256) : 64 );
	static const int  HALF_BUF_MASK  = HALF_BUF_SIZE - 1;
	static const int  BUF_SIZE       = HALF_BUF_SIZE * 2;
	static const int  BUF_MASK       = BUF_SIZE - 1;
	static const int  PHASE_MASK     = AntialiasedStep::NBR_PHASES - 1;

	static_assert (PHASE_LEN <= 1024, "");
	static_assert ((PHASE_LEN & 1) == 0, "");

	typedef std::array <CalcDataType, BUF_SIZE> Buffer;

	void           update_master_pitch () noexcept;
	fstb_FORCEINLINE void
	               update_sync_pos () noexcept;
	fstb_FORCEINLINE void
	               update_sync_speed () noexcept;
	fstb_FORCEINLINE void
	               generate_block (CalcDataType data_ptr [], int nbr_spl) noexcept;
	fstb_FORCEINLINE CalcDataType
	               generate_sample () noexcept;
	fstb_FORCEINLINE void
	               check_and_handle_sync_point () noexcept;
	fstb_FORCEINLINE void
	               add_step (CalcDataType step_amplitude) noexcept;
	fstb_FORCEINLINE void
	               fix_sync_pos_table () noexcept;

	Oscillator     _osc;
	AntialiasedStep *	         // Antialiased step, symetric data
	               _step_ptr { nullptr };

	// Reference pitch in octaves for Nyquist frequency, 32:16.
	int32_t        _base_pitch { 0 };

	// Main sync pitch in octaves, 32:16. Used in conjunction with _base_pitch.
	int32_t        _master_pitch { 0 };

	// Sample distance to the next sync point, relative to the current buffer
	// r/w pos. >= 0
	fstb::FixedPoint					
	               _spl_to_next_sync { 0 };

	// Time in sample between two sync. >= 2
	fstb::FixedPoint
	               _period { 0 };

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

	Buffer         _buffer {};       // Buffer for past and new samples
	int            _buf_pos { 0 };   // Writing position



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSampleSyncHard &other) const = delete;
	bool           operator != (const OscSampleSyncHard &other) const = delete;

}; // class OscSampleSyncHard



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSampleSyncHard.hpp"



#endif   // mfx_dsp_osc_OscSampleSyncHard_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
