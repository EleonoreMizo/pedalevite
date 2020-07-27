/*****************************************************************************

        Hash.h
        Author: Laurent de Soras, 2020

Hash functions with a low bias

Source: Chris Wellons
https://nullprogram.com/blog/2018/07/31/

Also: Vincent Lunot
https://lemire.me/blog/2017/09/18/computing-the-inverse-of-odd-integers/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_Hash_HEADER_INCLUDED)
#define fstb_Hash_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <cstdint>



namespace fstb
{



class Hash
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static fstb_FORCEINLINE fstb_CONSTEXPR14 uint32_t
	               hash (uint32_t x);
	static fstb_FORCEINLINE fstb_CONSTEXPR14 uint32_t
	               hash_inv (uint32_t x);

	static fstb_FORCEINLINE fstb_CONSTEXPR14 uint64_t
	               hash (uint64_t x);
	static fstb_FORCEINLINE fstb_CONSTEXPR14 uint64_t
	               hash_inv (uint64_t x);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Hash ()                               = delete;
	               Hash (const Hash &other)              = delete;
	               Hash (Hash &&other)                   = delete;
	               ~Hash ()                              = delete;
	Hash &         operator = (const Hash &other)        = delete;
	Hash &         operator = (Hash &&other)             = delete;
	bool           operator == (const Hash &other) const = delete;
	bool           operator != (const Hash &other) const = delete;

}; // class Hash



}  // namespace fstb



#include "fstb/Hash.hpp"



#endif   // fstb_Hash_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
