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

#include "fstb/fnc.h"

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
void	Lpf1p <T>::set_coef (T c) noexcept
{
   assert (c >= 0);
   assert (c <= 1);

   _coef = c;
}



template <typename T>
T	Lpf1p <T>::process_sample (T x) noexcept
{
   const T        y { _mem_y + (x - _mem_y) * _coef };
   _mem_y = y;

   return y;
}



template <typename T>
T	Lpf1p <T>::constant_block (T x, int nbr_spl) noexcept
{
   assert (nbr_spl > 0);

   const T        c_n { T (1.f) - fstb::ipowp (T (1.f) - _coef, nbr_spl) };
   const T        y { _mem_y + (x - _mem_y) * c_n };
   _mem_y = y;

   return y;
}



template <typename T>
T &	Lpf1p <T>::use_state () noexcept
{
   return _mem_y;
}



template <typename T>
const T &	Lpf1p <T>::use_state () const noexcept
{
   return _mem_y;
}



template <typename T>
void	Lpf1p <T>::clear_buffers () noexcept
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
