/*****************************************************************************

        FreqYin.h
        Author: Laurent de Soras, 2021

Template parameters:

- PP: post-processing object, ValSmooth for example. Requires:
	PP::PP ();
	PP::~PP ();
	void PP::proc_val (float f);
		0 indicates that the frequency detection failed for the current frame.
	float PP::get_val () const;
	void PP::clear_buffers ();

Based on:
Alain de Chevigne, Hideki Kawahara
YIN, a fundamental frequency estimator for speech and music,
Acoustical Society of America, 2002
Implemented up to step 5.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_FreqYin_HEADER_INCLUDED)
#define mfx_dsp_ana_FreqYin_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/Vf32.h"

#include <array>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace ana
{



template <class PP>
class FreqYin
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef PP PreProcessor;

	void           set_sample_freq (double sample_freq);
	void           set_freq_bot (float f) noexcept;
	void           set_freq_top (float f) noexcept;
	PP &           use_postproc () noexcept;
	const PP &     use_postproc () const noexcept;
	void           set_analysis_period (int per) noexcept;
	void           clear_buffers () noexcept;
	float          process_block (const float spl_ptr [], int nbr_spl) noexcept;
	float          process_sample (float x) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _vec_size_l2  = 2;
	static constexpr int _vec_size     = 1 << _vec_size_l2;
	static constexpr int _vec_mask     = _vec_size - 1;
	static constexpr int _max_blk_size = 64;

	class Delta
	{
	public:
		typedef std::array <float, _vec_size> VectFlt32;
		// Difference function = _sum_u + _sum_d
		alignas (16) VectFlt32 _sum_u {{ 0, 0, 0, 0 }};
		alignas (16) VectFlt32 _sum_d {{ 0, 0, 0, 0 }};
		alignas (16) VectFlt32 _cmndf {{ 0, 0, 0, 0 }};
	};
	typedef std::vector <Delta, fstb::AllocAlign <Delta, 16> > DeltaArray;

	void           update_freq_bot_param () noexcept;
	void           update_freq_top_param () noexcept;
	void           update_difference_functions () noexcept;
	void           update_difference_functions_block (int nbr_spl) noexcept;
	void           check_sum_position () noexcept;
	void           check_ana_position () noexcept;
	void           analyse () noexcept;
	float          get_cmndf (int delta) const noexcept;

	const float    _min_freq    = 20.0f; // Hz
	const float    _threshold   = 0.1f; // From step 4: absolute theshold

	std::vector <float>
	               _buffer;
	int            _buf_len     = 0;
	int            _buf_mask    = 0;
	int            _buf_pos_w   = 0;    // Writing position within the buffer
	float          _sample_freq = 0;    // Hz. 0 = not set
	int            _max_delta   = 0;    // Highest tested offset for the autocor. Samples, > _min_delta
	int            _min_delta   = 0;    // Lowest tested offset for the autocor. Samples, >= 2
	int            _win_len     = 0;    // Window length for the autocorrelation. Samples, generally equal to _max_delta
	int            _ana_per     = 64;   // Period between two analysis, in samples. >= 1
	int            _ana_pos     = 0;    // Position within the analysis period, [0 ; _ana_per[
	int            _sum_pos     = 0;    // Counter for the running sums. [0 ; _win_len[

	float          _freq_bot    = _min_freq; // Hz, > 0
	float          _freq_top    = 1000; // Hz, > _freq_bot
	PP             _postproc;
	DeltaArray     _delta_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FreqYin &other) const = delete;
	bool           operator != (const FreqYin &other) const = delete;

}; // class FreqYin



}  // namespace ana
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ana/FreqYin.hpp"



#endif   // mfx_dsp_ana_FreqYin_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
