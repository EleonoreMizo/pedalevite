/*****************************************************************************

        Dir.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_Dir_HEADER_INCLUDED)
#define mfx_piapi_Dir_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace piapi
{



enum Dir
{
	Dir_INVALID = -1,

	Dir_IN = 0,
	Dir_OUT,

	Dir_NBR_ELT

}; // enum Dir

template <Dir D>
class Dir_Inv
{
	static_assert (D == Dir_IN || D == Dir_OUT, "");
public:
	static const Dir _dir = Dir (1 - D);
};

template <typename DT>
inline Dir Dir_invert (DT d) { return Dir (1 - d); }



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/Dir.hpp"



#endif   // mfx_piapi_Dir_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
