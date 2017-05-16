/*****************************************************************************

        BbdLineReadable.cpp
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

#include "mfx/dsp/dly/BbdLineReadable.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	BbdLineReadable::init (int max_bbd_size, double sample_freq, rspl::InterpolatorInterface &interp, int ovrspl_l2)
{
	_sample_freq = float (sample_freq);
	_bbd.init (max_bbd_size, interp, ovrspl_l2);
}



const rspl::InterpolatorInterface &	BbdLineReadable::use_interpolator () const
{
	return _bbd.use_interpolator ();
}



void	BbdLineReadable::set_bbd_size (int bbd_size)
{
	_bbd.set_bbd_size (bbd_size);

	if (_speed > 0)
	{
		_max_dly_time = _bbd.get_bbd_size () / (_sample_freq * _speed);
	}
}



int	BbdLineReadable::get_bbd_size () const
{
	return _bbd.get_bbd_size ();
}



void	BbdLineReadable::set_speed (float speed)
{
	_bbd.set_speed (speed);
	_speed        = speed;

	const float    div = 1.0f / (_sample_freq * _speed);
	_min_dly_time = _bbd.compute_min_delay () * div;
	_max_dly_time = _bbd.get_bbd_size ()      * div;
}



void	BbdLineReadable::push_block (const float src_ptr [], int nbr_spl)
{
	_bbd.push_block (src_ptr, nbr_spl);
}



void	BbdLineReadable::push_sample (float x)
{
	_bbd.push_sample (x);
}



void	BbdLineReadable::clear_buffers ()
{
	_bbd.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



double	BbdLineReadable::do_get_sample_freq () const
{
	return _sample_freq;
}



int	BbdLineReadable::do_get_ovrspl_l2 () const
{
	return _bbd.get_ovrspl_l2 ();
}



double	BbdLineReadable::do_get_min_delay_time () const
{
	assert (_min_dly_time > 0);

	return _min_dly_time;
}



double	BbdLineReadable::do_get_max_delay_time () const
{
	assert (_max_dly_time > 0);

	return _max_dly_time;
}



int	BbdLineReadable::do_estimate_max_one_shot_proc_w_feedback (double min_dly_time) const
{
	const float    min_dly_bbd = float (min_dly_time) * _sample_freq * _speed;

	return _bbd.estimate_max_one_shot_proc_w_feedback (min_dly_bbd);
}



void	BbdLineReadable::do_read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const
{
	const float    mult        = _sample_freq * _speed;
	const float    dly_beg_bbd = float (dly_beg) * mult;
	const float    dly_end_bbd = float (dly_end) * mult;

	_bbd.read_block (dst_ptr, nbr_spl, dly_beg_bbd, dly_end_bbd, pos_in_block);
}



float	BbdLineReadable::do_read_sample (float dly) const
{
	const float    dly_bbd = dly * _sample_freq * _speed;

	return _bbd.read_sample (dly_bbd);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
