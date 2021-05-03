/*****************************************************************************

        FilterComb.h
        Author: Laurent de Soras, 2018

This comb filter has a sub-sample precision.

Processing is described as:

y[k] = OP (x [k] + FP (f * y [k - d]))

There is no other function called. Depending on IM, FP and OP, caller may
have to call additional functions to keep the consitency (setting the sampling
rate for example).

Template parameters:

- IM is a class generating the 4 FIR coefficients for an interpolator.
It must have the following functions:

	IM::IM ();
	void IM::make_interpolator (float fir_ptr [], float pos) noexcept;
	int IM::get_length () noexcept;
	int IM::get_delay () noexcept;

- FP is an optional processor for feedback path, put after the feedback gain.
It must have the following functions:

	FP::FP ();
	float FP::process_sample (float sample) noexcept;

- OP is an optional processor for delay line output. It must have the following
functions:

	OP::OP ();
	float OP::process_sample (float sample) noexcept;

- AL is a standard allocator for the internal buffer.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_FilterComb_HEADER_INCLUDED)
#define mfx_dsp_iir_FilterComb_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/dly/DelayLineFracFir4Base.h"
#include	"mfx/dsp/rspl/InterpFirMakerLagrange4.h"
#include	"mfx/dsp/FilterBypass.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace iir
{



template <typename IM = rspl::InterpFirMakerLagrange4 <float>, typename FP = FilterBypass, typename OP = FilterBypass, typename AL = std::allocator <float> >
class FilterComb
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef FP FeedbackProcessor;
	typedef OP OutputProcessor;
	typedef IM InterpolatorMaker;
	typedef AL AllocatorType;

	explicit       FilterComb (const AllocatorType &al = AllocatorType ());
	               FilterComb (const FilterComb &other) = default;
	               FilterComb (FilterComb &&other)      = default;

	               ~FilterComb ()                       = default;

	FilterComb &   operator = (const FilterComb &other) = default;
	FilterComb &   operator = (FilterComb &&other)      = default;

	void           set_sample_freq (float sample_freq);
	inline float   get_sample_freq () const noexcept;
	void           set_min_freq (float freq);
	inline float   get_min_freq () const noexcept;
	inline float   get_max_freq () const noexcept;
	void           set_freq (float freq) noexcept;
	inline float   get_freq () const noexcept;
	void           set_feedback (float feedback) noexcept;
	inline float   get_feedback () const noexcept;
	inline FeedbackProcessor &
	               use_feedback_proc () noexcept;
	inline const FeedbackProcessor &
	               use_feedback_proc () const noexcept;
	inline OutputProcessor &
	               use_output_proc () noexcept;
	inline const OutputProcessor &
	               use_output_proc () const noexcept;
	inline InterpolatorMaker &
	               use_interpolator_maker () noexcept;
	inline const InterpolatorMaker &
	               use_interpolator_maker () const noexcept;

	inline float   process_sample (float sample) noexcept;
	void           process_block (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_mix (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_vff (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_freq, float final_feedback) noexcept;
	void           process_block_vff_mix (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_freq, float final_feedback) noexcept;

	void           clear_buffers () noexcept;
	void           clear_buffers_fast () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class VariableFreqFeedbackData
	{
	public:
		float          _cur_feedback;
		float          _feedback_step;

		float          _cur_period;
		float          _period_step;
	};

	void           update_buffer ();
	void           update_interpolator () noexcept;
	void           update_interpolator_base_only (float d) noexcept;
	void           update_feedback () noexcept;
	inline float   read_sample_without_feedback () const noexcept;
	inline float   read_sample_with_feedback () const noexcept;
	inline float   write_in_delay_line (float sample) noexcept;
	inline void 	init_variable_freq_feedback (VariableFreqFeedbackData &data, int nbr_spl, float final_freq, float final_feedback) noexcept;
	inline float   iterate_variable_freq_feedback (VariableFreqFeedbackData &data, float sample) noexcept;

	std::array <float, 4>         // Base
	               _interp_base;
	std::array <float, 4>         // Pre-multiplied by the feedback
	               _interp_final;
	dly::DelayLineFracFir4Base <float, AllocatorType>
	               _buf;
	float          _sample_freq;  // Hz, > 0
	float          _freq;         // Hz
	float          _period;       // In samples, plus the interpolator delay, >= 4 - interp.delay
	float          _min_freq;
	float          _feedback;     // [-1; 1]
	int            _period_int;
	FeedbackProcessor
						_feedback_proc;
	OutputProcessor
						_output_proc;
	InterpolatorMaker
						_interpolator_maker;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FilterComb &other) const = delete;
	bool           operator != (const FilterComb &other) const = delete;

}; // class FilterComb



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/FilterComb.hpp"



#endif   // mfx_dsp_iir_FilterComb_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
