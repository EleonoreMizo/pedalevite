/*****************************************************************************

        DelayLineReadInterface.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLineReadInterface_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineReadInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dly
{



class DelayLineReadInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DelayLineReadInterface ()                        = default;
	               DelayLineReadInterface (const DelayLineReadInterface &other) = default;
	               DelayLineReadInterface (DelayLineReadInterface &&other)      = default;

	virtual        ~DelayLineReadInterface ()                       = default;

	virtual DelayLineReadInterface &
	               operator = (const DelayLineReadInterface &other) = default;
	virtual DelayLineReadInterface &
	               operator = (DelayLineReadInterface &&other)      = default;

	double         get_sample_freq () const;
	int            get_ovrspl_l2 () const;
	double         get_min_delay_time () const;
	double         get_max_delay_time () const;
	int            estimate_max_one_shot_proc_w_feedback (double min_delay_time) const;
	void           read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const;
	float          read_sample (float delay) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual double do_get_sample_freq () const = 0;
	virtual int    do_get_ovrspl_l2 () const = 0;
	virtual double do_get_min_delay_time () const = 0;
	virtual double do_get_max_delay_time () const = 0;
	virtual int    do_estimate_max_one_shot_proc_w_feedback (double min_dly_time) const = 0;
	virtual void   do_read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const = 0;
	virtual float  do_read_sample (float dly) const = 0;



}; // class DelayLineReadInterface



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/dly/DelayLineReadInterface.hpp"



#endif   // mfx_dsp_dly_DelayLineReadInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
