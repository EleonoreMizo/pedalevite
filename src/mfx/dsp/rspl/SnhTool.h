/*****************************************************************************

        SnhTool.h
        Author: Laurent de Soras, 2016

This class is intended to improve resampling speed in presence of oversampled
processing (rendering at a high sampling rate then downsampling to the target
sampling rate). The idea is to generate less samples than required (in other
words, at a lower rate) and to let the object hold the values to fill the
blanks. Sample & Hold is preferred here over zero-filling because it greatly
simplifies rate changes.

Set first the oversampling and the stream's number of channels.

To resample a block of data, follow these three steps:

1.	Call compute_snh_data().

2.	Resamples data. Use the information returned by the previous call to
	optimise your resampling. You have only to generates one sample every
	hold_time sample.

	Pseudo code:

	for (pos_dest = 0; pos_dest < nbr_spl; ++pos_dest)
	{
		if (rep_index == 0)
		{
			// Generate resampled data at pos_dest
		}
		// Increment cursors, rate and stuffs

		++ rep_index;
		rep_index %= hold_time;
	}

	Possibly, call before adjust_rate_param() to convert rates and increments,
	adapting to these new values. New pseudo code:

	for ( ; pos_dest < nbr_spl; pos_dest += hold_time)
	{
		// Generate resampled data at pos_dest
		// Increment cursors, rates and stuffs
	}
	// Update real rate and cursors

3.	After having resampled the block, call process_data() to finalise the
	buffers. Data is ready to be downsampled (or mixed before, or whatever).

This object is stateful, keep one separate copy per audio stream to be
processed.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_SnhTool_HEADER_INCLUDED)
#define	mfx_dsp_rspl_SnhTool_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/FixedPoint.h"
#include "mfx/dsp/rspl/Cst.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace rspl
{



class SnhTool
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SnhTool ();
	virtual        ~SnhTool () {}

	void           set_ovrspl (int ovrspl_l2);
	void           set_nbr_chn (int nbr_chn);
	int            get_nbr_chn () const;

	void           compute_snh_data (int &hold_time, int &rep_index, int max_nbr_spl, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step) const;
	bool           compute_snh_data_sample (const fstb::FixedPoint &rate) const;
	void           process_data (float * const data_ptr_arr [], int nbr_spl, const fstb::FixedPoint &rate, const fstb::FixedPoint &rate_step);

	void           clear_buffers ();

	static void    adjust_rate_param (int &pos_dest, fstb::FixedPoint &pos_src, fstb::FixedPoint &rate, fstb::FixedPoint &rate_step, int hold_time, int rep_index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  FADE_LEN = 256;   // Transition duration (number of samples) between two _hold_time.

	class ChnState
	{
	public:
		float          _hold_val;     // During transitions, it corresponds to the shorter blocks (_hold_time)
		float          _hold_val_max; // Only used in transitions. Corresponds to _nbr_sub * _hold_time.
	};

	typedef	std::array <ChnState, Cst::MAX_NBR_CHN>	ChnStateArray;

	void           process_data_steady_state (float * const data_ptr_arr [], int pos_beg, int pos_end);
	void           process_data_steady_state_naive (float * const data_ptr_arr [], int pos_beg, int pos_end);
	void           process_data_steady_state_block (float * const data_ptr_arr [], int pos_beg, int pos_end);

	void           process_data_interpolate (float * const data_ptr_arr [], int pos_beg, int pos_end);
	void           process_data_interpolate_naive (float * const data_ptr_arr [], int pos_beg, int pos_end);
	void           process_data_interpolate_block (float * const data_ptr_arr [], int pos_beg, int pos_end);

	static inline int
	               compute_hold_time (const fstb::FixedPoint &rate, int ovrspl_l2);

	ChnStateArray  _chn_state_arr;
	int            _nbr_chn;
	int            _ovrspl_l2;

	int            _hold_time;    // Current hold time
	int            _rep_index;    // [0 ; _hold_time[

	// Fade, interpolation between two hold times
	float          _interp_val;   // 0 = maximum, 1 = minimum
	float          _interp_step;
	int            _rem_spl;      // > 0 if fade is active

	int            _sub_index;    // Actually "sub" is the longest block
	int            _nbr_sub;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SnhTool (const SnhTool &other);
	SnhTool &      operator = (const SnhTool &other);
	bool           operator == (const SnhTool &other);
	bool           operator != (const SnhTool &other);

};	// class SnhTool



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



//#include "mfx/dsp/rspl/SnhTool.hpp"



#endif	// mfx_dsp_rspl_SnhTool_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
