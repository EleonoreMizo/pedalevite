/*****************************************************************************

        OscWavetable.h
        Author: Laurent de Soras, 2019

This oscillator is meant to be oversampled. Indeed, the top octave of the
spectrum may not be filled with all the harmonics, depending on the pitch.
It will also contain aliasing caused by the slow rolloff (and first sidelobes)
of the interpolator frequency response. Usually, oversampling it twice is
enough.

When generating the wavetable with DT as an integer type, make sure that there
is enough headroom for the interpolation overshoots. Limiting the waveform
amplitude to half the maximum absolute value of the type is safe.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscWavetable_HEADER_INCLUDED)
#define mfx_dsp_osc_OscWavetable_HEADER_INCLUDED

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
class OscWavetable
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef OscWavetable <IF, MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST> ThisType;
	typedef IF InterpFtor;
	typedef WavetableData <MAXSL2, MINSL2, OVRL2, DT, UPRE, UPOST> WavetableDataType;
	typedef typename WavetableDataType::DataType DataType;

	static const int  PITCH_FRAC_BITS = 16;

	               OscWavetable ()                          = default;
	               OscWavetable (const OscWavetable &other) = default;
	virtual        ~OscWavetable ()                         = default;
	OscWavetable & operator = (const OscWavetable &other)   = default;

	void           set_wavetable (const WavetableDataType &wavetable);
	const WavetableDataType &
	               use_wavetable () const;

	inline void    set_base_pitch (int32_t pitch);
	inline int32_t get_base_pitch () const;
	fstb_FORCEINLINE void
	               set_pitch (int32_t pitch);
	inline int32_t get_pitch () const;
	fstb_FORCEINLINE void
	               set_pitch_no_table_update (int32_t pitch);

	void           reset_phase ();
	void           set_phase_flt (float phase);
	fstb_FORCEINLINE float
	               get_phase_flt () const;
	void           set_phase_int (uint32_t phase);
	fstb_FORCEINLINE uint32_t
	               get_phase_int () const;

	DataType       get_sample_at_phase_flt (float phase) const;
	DataType       get_sample_at_phase_int (uint32_t phase) const;
	fstb_FORCEINLINE DataType
	               get_cur_sample () const;
	fstb_FORCEINLINE DataType
	               process_sample ();
	void           process_block (DataType dest_ptr [], int nbr_spl);
	void           process_block_mix (DataType dest_ptr [], int nbr_spl);
	inline void    skip_block (int nbr_spl);

	// Convenience function
	inline int32_t conv_freq_to_pitch (float freq, float fs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Pitch value for Nyquist frequency.
	int32_t        _base_pitch         = 0;

	// Current pitch. 0x10000 = +1 octave.
	int32_t        _pitch              = 0;

	// Selected table, depend on the pitch
	int            _cur_table          = 0;

	// Log base 2 of the length of selected table
	int            _cur_table_len_log2 = 0;

	// Length of selected table. 0 = not initialized
	int            _cur_table_len      = 1;

	int            _cur_table_mask     = 0;

	fstb::FixedPoint
	               _pos                = fstb::FixedPoint (0);
	fstb::FixedPoint
	               _step               = fstb::FixedPoint (0);
	const WavetableDataType *  // 0 = not initialized
	               _wavetable_ptr      = 0;
	InterpFtor     _interpolator;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscWavetable &other) const = delete;
	bool           operator != (const OscWavetable &other) const = delete;

}; // class OscWavetable



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscWavetable.hpp"



#endif   // mfx_dsp_osc_OscWavetable_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
