/*****************************************************************************

        Lpf1p.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Lpf1p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Lpf1p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// 1          -> neutral, cutoff at Nyquist
// close to 0 -> low cutoff frequency
template <typename T>
void	Lpf1p <T>::set_coef (T c)
{
   assert (c >= 0);
   assert (c <= 1);

   _coef = c;
}



template <typename T>
T	Lpf1p <T>::process_sample (T x)
{
   const T        y { _mem_y + (x - _mem_y) * _coef };
   _mem_y = y;

   return y;
}



template <typename T>
T &	Lpf1p <T>::use_state ()
{
   return _mem_y;
}



template <typename T>
const T &	Lpf1p <T>::use_state () const
{
   return _mem_y;
}



template <typename T>
void	Lpf1p <T>::clear_buffers ()
{
   _mem_y = T (0.f);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Lpf1p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
