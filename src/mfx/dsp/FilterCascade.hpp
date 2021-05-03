/*****************************************************************************

        FilterCascade.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_FilterCascade_CODEHEADER_INCLUDED)
#define mfx_dsp_FilterCascade_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class F1, class F2, class F3, class F4>
float	FilterCascade <F1, F2, F3, F4>::process_sample (float sample) noexcept
{
	sample = _f1.process_sample (sample);
	sample = _f2.process_sample (sample);
	sample = _f3.process_sample (sample);
	sample = _f4.process_sample (sample);

	return sample;
}



template <class F1, class F2, class F3, class F4>
typename FilterCascade <F1, F2, F3, F4>::Filter1Type &	FilterCascade <F1, F2, F3, F4>::use_filter_1 () noexcept
{
	return _f1;
}



template <class F1, class F2, class F3, class F4>
const typename FilterCascade <F1, F2, F3, F4>::Filter1Type &	FilterCascade <F1, F2, F3, F4>::use_filter_1 () const noexcept
{
	return _f1;
}



template <class F1, class F2, class F3, class F4>
typename FilterCascade <F1, F2, F3, F4>::Filter2Type &	FilterCascade <F1, F2, F3, F4>::use_filter_2 () noexcept
{
	return _f2;
}



template <class F1, class F2, class F3, class F4>
const typename FilterCascade <F1, F2, F3, F4>::Filter2Type &	FilterCascade <F1, F2, F3, F4>::use_filter_2 () const noexcept
{
	return _f2;
}



template <class F1, class F2, class F3, class F4>
typename FilterCascade <F1, F2, F3, F4>::Filter3Type &	FilterCascade <F1, F2, F3, F4>::use_filter_3 () noexcept
{
	return _f3;
}



template <class F1, class F2, class F3, class F4>
const typename FilterCascade <F1, F2, F3, F4>::Filter3Type &	FilterCascade <F1, F2, F3, F4>::use_filter_3 () const noexcept
{
	return _f3;
}



template <class F1, class F2, class F3, class F4>
typename FilterCascade <F1, F2, F3, F4>::Filter4Type &	FilterCascade <F1, F2, F3, F4>::use_filter_4 () noexcept
{
	return _f4;
}



template <class F1, class F2, class F3, class F4>
const typename FilterCascade <F1, F2, F3, F4>::Filter4Type &	FilterCascade <F1, F2, F3, F4>::use_filter_4 () const noexcept
{
	return _f4;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_FilterCascade_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
