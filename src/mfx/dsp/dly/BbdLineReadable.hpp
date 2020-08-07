/*****************************************************************************

        BbdLineReadable.hpp
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_BbdLineReadable_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_BbdLineReadable_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class BBD>
void	BbdLineReadable <BBD>::init (int max_bbd_size, double sample_freq, rspl::InterpolatorInterface &interp, int ovrspl_l2)
{
	_sample_freq = float (sample_freq);
	_bbd.init (max_bbd_size, interp, ovrspl_l2);
}



template <class BBD>
const rspl::InterpolatorInterface &	BbdLineReadable <BBD>::use_interpolator () const
{
	return _bbd.use_interpolator ();
}



template <class BBD>
void	BbdLineReadable <BBD>::set_bbd_size (int bbd_size)
{
	_bbd.set_bbd_size (bbd_size);

	if (_speed > 0)
	{
		_max_dly_time = _bbd.get_bbd_size () / (_sample_freq * _speed);
	}
}



template <class BBD>
int	BbdLineReadable <BBD>::get_bbd_size () const
{
	return _bbd.get_bbd_size ();
}



template <class BBD>
void	BbdLineReadable <BBD>::set_speed (float speed)
{
	_bbd.set_speed (speed);
	_speed        = speed;

	const float    div = 1.0f / (_sample_freq * _speed);
	_min_dly_time = _bbd.compute_min_delay () * div;
	_max_dly_time = _bbd.get_bbd_size ()      * div;
}



template <class BBD>
void	BbdLineReadable <BBD>::push_block (const float src_ptr [], int nbr_spl)
{
	_bbd.push_block (src_ptr, nbr_spl);
}



template <class BBD>
void	BbdLineReadable <BBD>::push_sample (float x)
{
	_bbd.push_sample (x);
}



template <class BBD>
void	BbdLineReadable <BBD>::clear_buffers ()
{
	_bbd.clear_buffers ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class BBD>
double	BbdLineReadable <BBD>::do_get_sample_freq () const
{
	return _sample_freq;
}



template <class BBD>
int	BbdLineReadable <BBD>::do_get_ovrspl_l2 () const
{
	return _bbd.get_ovrspl_l2 ();
}



template <class BBD>
double	BbdLineReadable <BBD>::do_get_min_delay_time () const
{
	assert (_min_dly_time > 0);

	return _min_dly_time;
}



template <class BBD>
double	BbdLineReadable <BBD>::do_get_max_delay_time () const
{
	assert (_max_dly_time > 0);

	return _max_dly_time;
}



template <class BBD>
int	BbdLineReadable <BBD>::do_estimate_max_one_shot_proc_w_feedback (double min_dly_time) const
{
	const float    min_dly_bbd = float (min_dly_time) * _sample_freq * _speed;

	return _bbd.estimate_max_one_shot_proc_w_feedback (min_dly_bbd);
}



template <class BBD>
void	BbdLineReadable <BBD>::do_read_block (float dst_ptr [], int nbr_spl, double dly_beg, double dly_end, int pos_in_block) const
{
	const float    mult        = _sample_freq * _speed;
	const float    dly_beg_bbd = float (dly_beg) * mult;
	const float    dly_end_bbd = float (dly_end) * mult;

	_bbd.read_block (dst_ptr, nbr_spl, dly_beg_bbd, dly_end_bbd, pos_in_block);
}



template <class BBD>
float	BbdLineReadable <BBD>::do_read_sample (float dly) const
{
	const float    dly_bbd = dly * _sample_freq * _speed;

	return _bbd.read_sample (dly_bbd);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_BbdLineReadable_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
