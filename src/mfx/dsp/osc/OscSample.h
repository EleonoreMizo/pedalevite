/*****************************************************************************

        OscSample.h
        Author: Laurent de Soras, 2019

Oscillator using a mip-mapped sample data.

The pre/post-unroll size may be used to other purpose than interpolation
security, such as sample loop unrolling. Therefore bound checks are loosy.

Template parameters:

- SD: SampleData object. It must be initialized before using the oscillator.
- IF: interpolator functor
- CDT: calculation and result data type. It may differ from the storage data
	type

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_OscSample_HEADER_INCLUDED)
#define mfx_dsp_osc_OscSample_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <typename SD, typename IF, typename CDT>
class OscSample
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  PITCH_FRAC_BITS = 16;

	typedef OscSample <SD, IF, CDT> ThisType;
	typedef SD SampleTable;
	typedef IF InterpFtor;
	typedef CDT CalcDataType;
	typedef typename SD::DataType StorageDataType;

	               OscSample ()                        = default;
	               OscSample (const OscSample &other)  = default;
	virtual        ~OscSample ()                       = default;

	OscSample &    operator = (const OscSample &other) = default;

	void           set_sample_data (const SampleTable &sample_data);
	const SampleTable &
						use_sample_data () const;
	SampleTable &  use_sample_data ();

	void           set_bandlimit (int32_t limit);
	fstb_FORCEINLINE int32_t
	               get_bandlimit () const;

	fstb_FORCEINLINE void
	               set_pitch (int32_t pitch);
	fstb_FORCEINLINE int32_t
	               get_pitch () const;
	fstb_FORCEINLINE void
	               get_table_rate (fstb::FixedPoint &rate) const;

	fstb_FORCEINLINE int
	               get_cur_table () const;
	fstb_FORCEINLINE void
	               set_playback_pos (const fstb::FixedPoint &pos);
	fstb_FORCEINLINE void
	               get_playback_pos (fstb::FixedPoint &pos) const;
	fstb_FORCEINLINE void
	               set_playback_pos_in_cur_table (const fstb::FixedPoint &pos);
	fstb_FORCEINLINE void
	               get_playback_pos_in_cur_table (fstb::FixedPoint &pos) const;

	fstb_FORCEINLINE CalcDataType
	               get_sample_in_cur_table (const fstb::FixedPoint &pos) const;
	fstb_FORCEINLINE CalcDataType
	               process_sample ();
	void           process_block (CalcDataType data_ptr [], int nbr_spl);
	void           process_block_mix (CalcDataType data_ptr [], int nbr_spl);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  PITCH_FRAC_RANGE = 1 << PITCH_FRAC_BITS;
	static const int  PITCH_FRAC_MASK  = PITCH_FRAC_RANGE - 1;

	SampleTable    _sample_data;
	const StorageDataType *          // Current table, updated each time pitch
	               _table_ptr = 0;   // changes.

	// Upper harmonic frequency range, relative to Nyquist. <= 0.
	// Set the pitch for table-switching. 0 = maximum 1-fold aliasing.
	// In octaves, 32:16.
	int32_t        _bandlimit = 0;

	// Playback pitch in octaves, 32:16. 0 = original sample rate.
	// + for higher pitch, - for lower pitch.
	int32_t        _pitch     = 0;

	int            _table     = 0;   // [0 ; N[
	fstb::FixedPoint                 // Relative to the current table
	               _pos       = fstb::FixedPoint (0);
	fstb::FixedPoint                 // Relative to the current table
	               _step      = fstb::FixedPoint (1);
	InterpFtor     _interpolator;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSample &other) const = delete;
	bool           operator != (const OscSample &other) const = delete;

}; // class OscSample



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/OscSample.hpp"



#endif   // mfx_dsp_osc_OscSample_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
