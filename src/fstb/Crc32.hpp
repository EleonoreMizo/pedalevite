/*****************************************************************************

        Crc32.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Crc32_CODEHEADER_INCLUDED)
#define fstb_Crc32_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Crc32::reset () noexcept
{
	_val = 0xFFFFFFFF;
}



/*
==============================================================================
Name: process_byte
Description:
	Insert a 8-bit number in CRC-controlled stream
Input parameters:
	- data_byte: value to add in the CRC. The 8 lower bits only are taken into
		account.
Throws: Nothing
==============================================================================
*/

void	Crc32::process_byte (int data_byte) noexcept
{
	_val = _lut [(_val ^ data_byte) & 0xFF] ^ (_val >> 8);
}



// Decompose data as bytes, LSB first (little endian).
template <typename T>
void	Crc32::process (T data) noexcept
{
	for (int k = 0; k < int (sizeof (T)); ++k)
	{
		process_byte (int (data));
		data >>= 8;
	}
}



uint32_t	Crc32::get_val () const noexcept
{
	return _val ^ 0xFFFFFFFF;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fstb



#endif   // fstb_Crc32_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
