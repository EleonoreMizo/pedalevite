/*****************************************************************************

        BbdLineReadable.h
        Author: Laurent de Soras, 2017

Template parameters:

- BBD: type of the BbdLine to wrap

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_BbdLineReadable_HEADER_INCLUDED)
#define mfx_dsp_dly_BbdLineReadable_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dly/BbdLine.h"
#include "mfx/dsp/dly/DelayLineReadInterface.h"



namespace mfx
{
namespace dsp
{
namespace dly
{



template <class BBD = BbdLine <> >
class BbdLineReadable final
:	public DelayLineReadInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef BBD BbdType;

	static constexpr int  _min_speed_l2 = BbdType::_min_speed_l2;

	void           init (int max_bbd_size, double sample_freq, rspl::InterpolatorInterface &interp, int ovrspl_l2);
	const rspl::InterpolatorInterface &
	               use_interpolator () const;
	void           set_bbd_size (int bbd_size);
	int            get_bbd_size () const;

	void           set_speed (float speed);
	void           push_block (const float src_ptr [], int nbr_spl);
	void           push_sample (float x);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// DelayLineReadInterface
	double         do_get_sample_freq () const final;
	int            do_get_ovrspl_l2 () const final;
	double         do_get_min_delay_time () const final;
	double         do_get_max_delay_time () const final;
	int            do_estimate_max_one_shot_proc_w_feedback (double min_delay_time) const final;
	void           do_read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const final;
	float          do_read_sample (float delay) const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _sample_freq    = 0; // Hz, > 0. 0 = not initialised
	float          _speed          = 0; // 0 = not initialised
	BbdType        _bbd;
	float          _min_dly_time   = -1;
	float          _max_dly_time   = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const BbdLineReadable <BBD> &other) const = delete;
	bool           operator != (const BbdLineReadable <BBD> &other) const = delete;

}; // class BbdLineReadable



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/BbdLineReadable.hpp"



#endif   // mfx_dsp_dly_BbdLineReadable_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
