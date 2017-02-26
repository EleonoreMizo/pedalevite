/*****************************************************************************

        SegmentRc.cpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ctrl/env/SegmentRc.h"

#include <algorithm>

#include <cassert>
#include <cmath>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: setup
Description:

* Current value
 \
  \
   \_
	  \__              Final val
--------\====================== ^
             \________          | end threshold
______________________\________ v
0 of the exponential

Input parameters:
	- final_val: This is the value reached after an infinite amount of time.
		It is not the final exponential value. If the duration is intended to
      be infinite (mult == 1), current value should be over the end threshold,
      otherwise get_nbr_rem_spl() will return 0.
	- mult: Recursive multiplier applied at each sample, for exponential
		calculation. Should be in ]-1 ; 1], 1 is for infinite duration.
	- end_threshold: Level between the exponential 0 and the final value.
		Must be > 0 because true 0 can't be reached.
Throws: Nothing
==============================================================================
*/

void	SegmentRc::setup (float final_val, float mult, float end_thr)
{
	setup_and_set_val (final_val, mult, end_thr, get_val ());
}



// Duration: force duration (samples) of the section. Useful when start and
// target values are close and prone to numeric roundoff errors.
void	SegmentRc::setup (float final_val, float mult, float end_thr, long duration)
{
   assert (duration >= 0);

   const float    val = get_val ();
	setup_partial (final_val, mult, end_thr);
	set_val_direct (val);
   _nbr_rem_spl = duration;
}



void	SegmentRc::setup_and_set_val (float final_val, float mult, float end_thr, float val)
{
	setup_partial (final_val, mult, end_thr);
	set_val (val);
}



void	SegmentRc::set_val (float val)
{
   set_val_direct (val);
	compute_nbr_rem_spl ();
}



void	SegmentRc::process_block (float data_ptr [], int nbr_spl)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	const float    offset   = _offset;
	const float    mult     = _mult;
	double         val      = _raw_val;

	const int      work_len = std::min (nbr_spl, _nbr_rem_spl);
	for (int pos = 0; pos < work_len; ++pos)
	{
		data_ptr [pos] = float (val + offset);
		val           *= mult;
	}

	_raw_val      = float (val);
	_nbr_rem_spl -= work_len;

	// Fill with a constant if the section is finished
	const float    fill_val = get_val ();
	for (int pos = work_len; pos < nbr_spl; ++pos)
	{
		data_ptr [pos] = fill_val;
	}
}



void	SegmentRc::skip_block (int nbr_spl)
{
	assert (nbr_spl > 0);

	if (nbr_spl >= _nbr_rem_spl)
	{
		_raw_val     = _final_val - _offset;
		_nbr_rem_spl = 0;
	}
	else
	{
		if (_mult != 1)
		{
			_raw_val *= float (fstb::ipowp (double (_mult), nbr_spl));
		}
		_nbr_rem_spl -= nbr_spl;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SegmentRc::setup_partial (float final_val, float mult, float end_thr)
{
	assert (fabs (mult) <= 1);
	assert (end_thr >= 0);

	_final_val = final_val;
	_end_thr   = end_thr;
	_mult      = mult;
}



void	SegmentRc::set_val_direct (float val)
{
	_offset = _final_val;
   const float    dif = val - _final_val;

   // Constant case: we already reached the value
   const float    dif_abs = fabs (dif);
   if (   dif_abs <= 0.05f
       && dif_abs <= _end_thr)
   {
      _raw_val = 0;
   }

   // Normal cases
   else
   {
      if (dif > 0)
	   {
		   _offset -= _end_thr;
	   }
	   else
	   {
		   _offset += _end_thr;
	   }
	   _raw_val = val - _offset;
   }
}



void	SegmentRc::compute_nbr_rem_spl ()
{
	if (_mult >= 1)
	{
		_nbr_rem_spl = LONG_MAX;
	}
	else if (_raw_val == 0)
   {
      _nbr_rem_spl = 0;
   }
   else
	{
	   const double   ratio = double (_end_thr) / fabs (double (_raw_val));
		_nbr_rem_spl = fstb::ceil_int (log (ratio) / log (_mult));
	}
}



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
