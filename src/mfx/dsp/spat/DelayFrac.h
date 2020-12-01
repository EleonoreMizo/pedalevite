/*****************************************************************************

        DelayFrac.h
        Author: Laurent de Soras, 2020

Simple fractional delay without feedback using 4-point FIR interpolation

Call first set_max_len()
Then set_delay_flt() or set_delay_fix()

Template parameters:

- T : Stored data type. Should have:
	T::T ();
	T::T (const T &);
	T::T (float);
	T::~T ();
	T operator * (T, T);
	T operator + (T, T);

- NPL2 : log2 of the number of phases for the interpolator. Sets the
	resolution of the fractional delay. 6 (64) should be fine for a generic
	use.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_DelayFrac_HEADER_INCLUDED)
#define mfx_dsp_spat_DelayFrac_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>
#include <vector>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <typename T, int NPL2>
class DelayFrac
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _nbr_phases_l2 = NPL2;
	static constexpr int _nbr_phases    = 1 << NPL2;
	static constexpr int _delay_min     = 1; // Samples

	typedef T DataType;

	void           set_max_len (int len);

	fstb_FORCEINLINE void
	               set_delay_flt (float len_spl);
	fstb_FORCEINLINE void
	               set_delay_fix (int len_fixp);
	fstb_FORCEINLINE int
	               get_delay_len_int () const;

	fstb_FORCEINLINE T
	               read () const;
	fstb_FORCEINLINE T
	               read_at (int delay) const;
	fstb_FORCEINLINE void
	               write (T x);
	fstb_FORCEINLINE void
	               step ();
	fstb_FORCEINLINE T
	               process_sample (T x);

	fstb_FORCEINLINE int
	               get_max_block_len () const;
	fstb_FORCEINLINE void
	               read_block (T dst_ptr [], int len) const;
	fstb_FORCEINLINE void
	               read_block_var_dly (T dst_ptr [], const int32_t dly_fix_ptr [], int len);
	fstb_FORCEINLINE void
	               read_block_at (T dst_ptr [], int delay, int len) const;
	fstb_FORCEINLINE void
	               write_block (const T src_ptr [], int len);
	fstb_FORCEINLINE void
	               step_block (int len);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _phase_len = 4;
	static constexpr int _ph_pre    = 2;
	static constexpr int _ph_post   = _phase_len - _ph_pre - 1;
	static constexpr int _phase_msk = _nbr_phases - 1;

	typedef std::array <float, _phase_len> Phase;
	typedef std::array <Phase, _nbr_phases> PhaseArray;

	fstb_FORCEINLINE T
	               read_safe (int pos_read, const Phase &phase) const;
	fstb_FORCEINLINE T
	               read_nocheck (int pos_read, const Phase &phase) const;
	fstb_FORCEINLINE void
	               find_phase_and_delay (const Phase * &phase_ptr, int &delay_int, int &delay_frc, int len_fixp) const;

	static void    init_interpolator ();

	std::vector <T>
	               _buffer;
	int            _buf_msk   = 0;
	int            _delay_len = 0; // Maximum length in samples
	int            _delay_int = 0; // Current delay, integer part (samples)
	int            _delay_frc = 0; // Current delay, fractional part (phase)
	int            _pos_write = 0;
	const Phase *  _phase_ptr = nullptr;

	static bool    _interp_init_flag;

	// The array index corresponds to a fractional delay, not a fractional
	// position.
	// _phase_arr [            0] corresponds to position 0
	// _phase_arr [_nbr_phases-1] corresponds to position 1/_nbr_phases - 1
	static PhaseArray
	               _phase_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayFrac &other) const = delete;
	bool           operator != (const DelayFrac &other) const = delete;

}; // class DelayFrac



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/DelayFrac.hpp"



#endif   // mfx_dsp_spat_ltc_DelayFrac_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
