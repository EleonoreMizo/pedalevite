/*****************************************************************************

        OscWavetableSyncHard.h
        Author: Laurent de Soras, 2019

This oscillator use another oscillator (called slave) and make it reset its
phase at a rate corresponding to its own pitch. The slave pitch is independent
and can be varied to shape the final tone.

The oscillator has a delay corresponding to the antialiased step group delay
(and exact step position), generally half the steptable size.

Wavetables and antialiased step data should be filled by client.

Antialiased step formula f(t):

f(t) = Integral (sinc (t), -oo...t) - Step (t);
t = time in samples, generally not integer.

Step (t) = 0 if t < 0
Step (t) = 1 if t >= 0
Integral (sinc (t), -oo...t) is supposed to be 0 for t = -oo and 1 for t = +oo
f should be antisymmetric.

The antialiased step table covers the range [-a ; +a[ for t.

Template parameters:

- OSC: main oscillator class, OscWavetable or OscWavetableSub
- STPPLEN: Size of the table for antialiased step (integrated sinc).
	Should be even and <= 1024
- STPNPL2: Log2 of the number of phases for the antialiased step
- STPLVL2: Log2 of the nominal level of the antialiased step.
	Ignored with floating point types.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscWavetableSyncHard_HEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetableSyncHard_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/FixedPoint.h"
#include "mfx/dsp/osc/WavetablePolyphaseData.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <typename OSC, int STPPLEN, int STPNPL2, int STPLVL2>
class OscWavetableSyncHard
{
	static_assert (STPPLEN > 0, "");
	static_assert (STPPLEN <= 1024, "");
	static_assert ((STPPLEN & 1) == 0, "");
	static_assert (STPNPL2 >= 0, "");
	static_assert (STPLVL2 >= 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  NBR_PHASES_L2  = STPNPL2;
	static const int  NBR_PHASES     = 1 << NBR_PHASES_L2;
	static const int  PHASE_LEN      = STPPLEN;
	static const int  PHASE_LEVEL_L2 = STPLVL2;
	static const int  PHASE_LEVEL    = 1 << PHASE_LEVEL_L2;

	typedef OSC Oscillator;
	typedef typename Oscillator::WavetableDataType WavetableDataType;
	typedef typename Oscillator::DataType DataType;
	typedef WavetablePolyphaseData <STPPLEN, STPNPL2, DataType> AntialiasedStep;

	static const int  PITCH_FRAC_BITS = Oscillator::PITCH_FRAC_BITS;

	Oscillator &   use_osc ();
	const Oscillator &
	               use_osc () const;

	void           set_wavetable (const WavetableDataType &wavetable);
	const WavetableDataType &
	               use_wavetable () const;

	void           set_steptable (const AntialiasedStep &steptable);
	const AntialiasedStep &
	               use_steptable () const;

	inline void    set_base_pitch (int32_t pitch);
	inline int32_t get_base_pitch () const;
	fstb_FORCEINLINE void
	               set_pitch (int32_t pitch);
	inline int32_t get_pitch () const;
	fstb_FORCEINLINE void
	               set_pitch_slave (int32_t pitch);
	inline int32_t get_pitch_slave () const;

	void           reset_phase ();
	void           set_phase (uint32_t phase);
	fstb_FORCEINLINE uint32_t
	               get_phase () const;

	inline void    set_sync_pos (uint32_t pos);
	fstb_FORCEINLINE uint32_t
	               get_sync_pos () const;

	void           clear_buffers ();
	DataType       process_sample ();
	void           process_block (DataType dst_ptr [], int nbr_spl);

	// Convenience function
	inline int32_t conv_freq_to_pitch (float freq, float fs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  PHASE_MASK     = NBR_PHASES - 1;
	static const int  HALF_PHASE_LEN = PHASE_LEN / 2;  // Step output delay (= ceil (group_delay))
	static const int  HALF_BUF_SIZE  = ((PHASE_LEN > 63) ? ((PHASE_LEN > 255) ? 1024 : 256) : 64 );
	static const int  HALF_BUF_MASK  = HALF_BUF_SIZE - 1;
	static const int  BUF_SIZE       = HALF_BUF_SIZE * 2;
	static const int  BUF_MASK       = BUF_SIZE - 1;

	typedef std::array <DataType, BUF_SIZE> Buffer;

	DataType       generate_sample ();
	void           generate_block (DataType dst_ptr [], int nbr_spl);
	void           check_and_handle_sync_point ();
	void           generate_step ();
	void           add_step (DataType step_amp, uint32_t stns_neg);

	// Waveform oscillator
	Oscillator     _osc;

	// Antialiased step, symmetric data. 0 = not set
	const AntialiasedStep *
	               _step_ptr           = nullptr;

	// Pitch value for Nyquist frequency.
	int32_t        _base_pitch         = 0;

	// Current pitch. 0x10000 = +1 octave.
	int32_t        _pitch              = 0;

	// Time in output samples between two sync (real oscillator period). >= 2.
	fstb::FixedPoint
	               _period             = fstb::FixedPoint (64);

	// Sample distance to the next sync point, relative to the current buffer
	// r/w pos (output scale). > -1. When _spl_to_next_sync is in ]-1; 0], it
	// means that a phase discontinuity occured between the previous sample
	// and the current one, and should be corrected with an antialiased step
	// right now.
	fstb::FixedPoint
	               _spl_to_next_sync   = fstb::FixedPoint (0);

	// Slave osc frequency, in 1/samples (= freq_Hz / fs_Hz).
	// Integer part is most likely 0.
	fstb::FixedPoint
	               _freq_slave         = fstb::FixedPoint (0);

	// Buffers for past and new samples.
	Buffer         _buffer             {};

	// Write position within the buffer
	int            _buf_pos            = 0;

	// Reset position for the phase
	uint32_t       _sync_pos           = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscWavetableSyncHard &other) const = delete;
	bool           operator != (const OscWavetableSyncHard &other) const = delete;

}; // class OscWavetableSyncHard



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscWavetableSyncHard.hpp"



#endif   // mfx_dsp_osc_OscWavetableSyncHard_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
