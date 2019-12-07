/*****************************************************************************

        Cnx.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_Cnx_HEADER_INCLUDED)
#define mfx_cmd_Cnx_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/CnxEnd.h"



namespace mfx
{
namespace cmd
{



class Cnx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Cnx ()                        = default;
	               Cnx (const Cnx &other)        = default;
	virtual        ~Cnx ()                       = default;

	Cnx &          operator = (const Cnx &other) = default;

	inline bool    operator < (const Cnx &other) const;
	inline bool    operator == (const Cnx &other) const;
	inline bool    operator != (const Cnx &other) const;

	inline bool    is_valid () const;
	inline void    invalidate ();

	CnxEnd         _src;
	CnxEnd         _dst;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:




}; // class Cnx



}  // namespace cmd
}  // namespace mfx



#include "mfx/cmd/Cnx.hpp"



#endif   // mfx_cmd_Cnx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
