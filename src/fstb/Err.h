/*****************************************************************************

        Err.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Err_HEADER_INCLUDED)
#define	fstb_Err_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



enum Err
{
	Err_OK = 0,

	Err_ZONE = -10000,

	Err_EXCEPTION = Err_ZONE,
	Err_NO_ROOM,
	Err_NOT_FOUND,
	Err_CHAR_OUT_OF_RANGE,
	Err_INVALID_UTF8_LEAD_BYTE,
	Err_INVALID_UTF8_SEQ_LEN,
	Err_INVALID_UTF8_BYTE,
	Err_TRUNCATED_UTF8_SEQ,
	Err_INVALID_UTF16_LEAD_WORD,
	Err_INVALID_UTF16_END_WORD,
	Err_CANNOT_OPEN_FILE,
	Err_CANNOT_READ_DATA,
	Err_SP_PROHIBITED_CHAR,
	Err_SP_CHECK_BIDIR,
	Err_INVALID_PC_SEQ,
	Err_TRUNCATED_PE_SEQ,
	Err_INVALID_ESC_SEQ,
	Err_TRUNCATED_ESC_SEQ

};	// enum Err






}	// namespace fstb



//#include	"fstb/Err.hpp"



#endif	// fstb_Err_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
