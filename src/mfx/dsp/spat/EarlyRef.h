/*****************************************************************************

        EarlyRef.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_EarlyRef_HEADER_INCLUDED)
#define mfx_dsp_spat_EarlyRef_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/dly/DelaySimple.h"

#include <array>
#include <utility>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace spat
{



template <typename T>
class EarlyRef
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _max_nbr_taps = 64;

	void           reset (double sample_freq, double max_predelay_time, double max_duration);

	void           generate_taps (uint32_t seed, int nbr_taps, float duration, float lvl_end, float gain);
	void           set_predelay (float delay);

	fstb_FORCEINLINE std::pair <T, T>
	               process_sample (T x);
	void           process_block (T dly_ptr [], T erf_ptr [], const T src_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _max_blk_size = 64;

	class Tap
	{
	public:
		int            _pos  = 0;
		T              _gain = 0;
	};
	typedef std::array <Tap, _max_nbr_taps> TapArray;

	typedef dly::DelaySimple <T> DelayType;

	double            _sample_freq  = 0; // Hz, > 0. 0 = not set
	double            _max_predelay_time = 0; // s, >= 0
	double            _max_duration = 0; // s, >= 0. 0 = not set
	TapArray          _tap_arr;
	int               _nbr_taps     = 0; // [0 ; _max_nbr_taps].
	DelayType         _delay;
	int               _predelay_spl = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const EarlyRef &other) const = delete;
	bool           operator != (const EarlyRef &other) const = delete;

}; // class EarlyRef



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/EarlyRef.hpp"



#endif   // mfx_dsp_spat_EarlyRef_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
