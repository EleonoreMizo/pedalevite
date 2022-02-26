/*****************************************************************************

        AllMeters.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_AllMeters_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_AllMeters_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ana/r128/ChnMerger.h"
#include "mfx/dsp/ana/r128/HistoLu.h"
#include "mfx/dsp/ana/r128/PeakDetectStd.h"
#include "mfx/dsp/ana/r128/SegmentHelper.h"

#include <array>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class AllMeters
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_nbr_chn (int nbr_chn);
	void           set_chn_weights (const float weight_arr []) noexcept;
	void           set_chn_buffers (const float *src_ptr_arr []) noexcept;

	void           analyse_sample (int pos) noexcept;
	void           analyse_block (int pos_beg, int pos_end) noexcept;

	float          get_loudness_m () const noexcept;
	float          get_loudness_s () const noexcept;
	float          compute_loudness_r () const noexcept;
	float          compute_loudness_i () const noexcept;
	float          get_peak (int chn_idx) const noexcept;
	void           clear_peaks () noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Default finite value for silence, LUFS
	static constexpr float  _clear_peak_val = -999;

	enum class Win
	{
		M = 0, // Momentary (400 ms) and loudness integrated (same window param)
		S,     // Short term (3 s) and loudness range (same window parameters)

		NBR_ELT
	};

	class Meter
	{
	public:
		ChnMerger   _merger;
		HistoLu     _histo;
	};
	typedef std::array <Meter, int (Win::NBR_ELT)> MeterArray;

	class Channel
	{
	public:
		const float *  _src_ptr = nullptr;
		PeakDetectStd  _peak_detect;
	};
	typedef std::vector <Channel> ChannelArray;

	void           update_sample_freq ();
	inline float   get_loudness (Win type) const noexcept;

	double         _sample_freq = 0; // Sample frequency, Hz, > 0. 0 = invalid
	ChannelArray   _chn_arr;
	MeterArray     _meter_arr;
	SegmentHelper  _seg;
	float          _loud_m      = _clear_peak_val; // Latest momentary loudness, LUFS



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AllMeters &other) const = delete;
	bool           operator != (const AllMeters &other) const = delete;

}; // class AllMeters



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/AllMeters.hpp"



#endif   // mfx_dsp_ana_r128_AllMeters_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
