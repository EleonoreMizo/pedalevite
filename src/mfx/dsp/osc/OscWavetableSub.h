/*****************************************************************************

        OscWavetableSub.h
        Author: Laurent de Soras, 2019

Oscillator with a "subtraction" option. Two waveforms with a variable phase
shift are subtracted from each other. For example, with identical saw
waveforms, one can obtain a rectangle waveform with PWM (pulse width
modulation). The oscillator works more or less the same as OscWavetable.

This oscillator is meant to be oversampled. Indeed, the top octave of the
spectrum may not be filled with all the harmonics, depending on the pitch.
It will also contain aliasing caused by the slow rolloff (and first sidelobes)
of the interpolator frequency response. Usually, oversampling it twice is
enough.

When generating the wavetable with DT as an integer type, make sure that there
is at least 6 dB of headroom for the subraction, in addition to the headroom
required for the interpolation overshoots. Limiting the waveform amplitude to
the quarter of the maximum absolute value of the type is safe.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscWavetableSub_HEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetableSub_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"
#include "mfx/dsp/osc/WavetableData.h"

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <typename IF, int MAXSL2, int MINSL2, int OVRL2, typename DT = float, int UPRE = 1, int UPOST = 3>
class OscWavetableSub
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef OscWavetableSub <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST> ThisType;
	typedef IF InterpFtor;
	typedef WavetableData <MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST> WavetableDataType;
	typedef typename WavetableDataType::DataType DataType;

	static const int  PITCH_FRAC_BITS = 16;

	               OscWavetableSub ()                             = default;
	               OscWavetableSub (const OscWavetableSub &other) = default;
	virtual        ~OscWavetableSub ()                            = default;
	OscWavetableSub &
	               operator = (const OscWavetableSub &other)      = default;

	void           set_wavetables (const WavetableDataType &wavetable_pos, const WavetableDataType &wavetable_neg);
	const WavetableDataType &
	               use_wavetable (int number) const;

	inline void    set_base_pitch (int32_t pitch);
	inline int32_t get_base_pitch () const;
	fstb_FORCEINLINE void
	               set_pitch (int32_t pitch);
	fstb_FORCEINLINE void
	               set_pitch (int32_t pitch, uint32_t pre_step, int table);
	inline int32_t get_pitch () const;

	void           reset_phase ();
	fstb_FORCEINLINE void
	               set_phase_int (uint32_t phase);
	fstb_FORCEINLINE uint32_t
	               get_phase_int () const;

	fstb_FORCEINLINE void
	               set_rel_phase_int (uint32_t rel_phase, DataType dc_fixer);
	fstb_FORCEINLINE uint32_t
	               get_rel_phase_int () const;

	fstb_FORCEINLINE void
	               get_sample_at_phase (DataType &ref_data, DataType &sub_data, uint32_t phase) const;
	fstb_FORCEINLINE void
	               process_sample (DataType &ref_data, DataType &sub_data);
	void           process_sample_fm (DataType &ref_data, DataType &sub_data, int32_t new_pitch);
	void           process_block (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl);
	void           process_block_mix (DataType ref_data_ptr [], DataType sub_data_ptr [], int nbr_spl);

	// Convenience function
	int32_t        conv_freq_to_pitch (float freq, float fs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  PITCH_FRAC_MASK = (1 << PITCH_FRAC_BITS) - 1;

	fstb_FORCEINLINE void
	               sync_sub_phase ();
	fstb_FORCEINLINE void
	               sync_sub_phase (const fstb::FixedPoint &position_pos, fstb::FixedPoint &position_neg) const;
	fstb_FORCEINLINE void
	               generate_sample (DataType &ref_data, DataType &sub_data, const fstb::FixedPoint &position_pos, const fstb::FixedPoint &position_neg, const DataType * const src_pos_ptr, const DataType * const src_neg_ptr) const;
	fstb_FORCEINLINE void
	               step_one_sample ();

	// Pitch value for Nyquist frequency.
	int32_t        _base_pitch         = 0;

	// Current pitch. 0x10000 = +1 octave.
	int32_t        _pitch              = 0;

	// Phase between positive and negative components
	int32_t        _rel_phase          = 0x80000000;

	// Compensates DC offset after subtraction
	DataType       _dc_fixer           = 0;

	// Selected table, depend on the pitch
	int            _cur_table          = 0;

	// Log base 2 of the length of selected table
	int            _cur_table_len_log2 = 0;

	// Length of selected table. 0 = not initialized
	int            _cur_table_len      = 1;

	int            _cur_table_mask     = 0;

	fstb::FixedPoint
	               _position_pos       = fstb::FixedPoint (0);
	fstb::FixedPoint
	               _position_neg       = fstb::FixedPoint (0);
	fstb::FixedPoint
	               _step               = fstb::FixedPoint (0);

	const WavetableDataType *  // 0 = not initialized
	               _wavetable_pos_ptr  = 0;
	const WavetableDataType *  // 0 = not initialized
	               _wavetable_neg_ptr  = 0;

	InterpFtor     _interpolator;


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscWavetableSub &other) const = delete;
	bool           operator != (const OscWavetableSub &other) const = delete;

}; // class OscWavetableSub



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscWavetableSub.hpp"



#endif   // mfx_dsp_osc_OscWavetableSub_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
