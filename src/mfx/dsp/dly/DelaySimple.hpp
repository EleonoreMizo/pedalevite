/*****************************************************************************

        DelaySimple.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED)
#define mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace dly
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	DelaySimple::read_at (int d) const
{
	assert (d >= 0);
	assert (d <= _max_dly);

	return _buf [(_pos_w - d) & _mask];
}



float	DelaySimple::process_sample (float x)
{
	_buf [_pos_w] = x;
	const float    y = _buf [delay (_pos_w)];

	_pos_w = (_pos_w + 1) & _mask;

	return y;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	DelaySimple::delay (int pos) const
{
	return (pos - _dly) & _mask;
}



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dly_DelaySimple_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
