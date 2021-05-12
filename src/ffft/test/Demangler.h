/*****************************************************************************

        Demangler.h
        Author: Laurent de Soras, 2021

Demangles a RTTI name into something human-readable.

https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (ffft_test_Demangler_HEADER_INCLUDED)
#define ffft_test_Demangler_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/def.h"

#if ffft_COMPILER == ffft_COMPILER_GCC
#include <cxxabi.h>
#endif



namespace ffft
{
namespace test
{



class Demangler
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit Demangler (const char *name_0)
	:	_name_0 (name_0)
	{
#if ffft_COMPILER == ffft_COMPILER_GCC
		const char *   dem_0 = abi::__cxa_demangle (name_0, 0, 0, &_status);
		if (_status == 0)
		{
			_name_0 = dem_0;
		}
#endif
	}

	~Demangler ()
	{
#if ffft_COMPILER == ffft_COMPILER_GCC
		if (_status == 0)
		{
			free (const_cast <char *> (_name_0));
		}
#endif
		_name_0 = nullptr;
	}

	const char *   get_name () const noexcept { return _name_0; }



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	const char *   _name_0 = nullptr;

#if ffft_COMPILER == ffft_COMPILER_GCC
	int            _status = 0;
#endif


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Demangler ()                               = delete;
	               Demangler (const Demangler &other)         = delete;
	               Demangler (Demangler &&other)              = delete;
	Demangler &    operator = (const Demangler &other)        = delete;
	Demangler &    operator = (Demangler &&other)             = delete;
	bool           operator == (const Demangler &other) const = delete;
	bool           operator != (const Demangler &other) const = delete;

}; // class Demangler



}  // namespace test
}  // namespace ffft



//#include "ffft/test/Demangler.hpp"



#endif   // ffft_test_Demangler_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
