/*****************************************************************************

        ProgSwitchMode.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ProgSwitchMode_HEADER_INCLUDED)
#define mfx_doc_ProgSwitchMode_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace doc
{



enum class ProgSwitchMode
{
   DIRECT = 0,
   FADE_OUT_IN,

   NBR_ELT

}; // enum ProgSwitchMode



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ProgSwitchMode.hpp"



#endif   // mfx_doc_ProgSwitchMode_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
