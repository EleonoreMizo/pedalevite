/*****************************************************************************

        Dir.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_Dir_HEADER_INCLUDED)
#define lal_Dir_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace lal
{



enum class Dir
{

	V = 0, // Vertical (single column: i, 0)
	H, // Horizontal (single row: 0, i)
	D  // Diagonal (i, i)

}; // enum class Dir



}  // namespace lal



//#include "lal/Dir.hpp"



#endif   // lal_Dir_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
