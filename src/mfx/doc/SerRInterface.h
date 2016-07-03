/*****************************************************************************

        SerRInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_SerRInterface_HEADER_INCLUDED)
#define mfx_doc_SerRInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <string>
#include <type_traits>



namespace mfx
{
namespace doc
{



class SerRInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~SerRInterface () = default;

	int            begin_list (int &nbr_elt);
	int            end_list ();
	int            read (double &x);
	int            read (std::string &s);

	// Helper functions
	int            begin_list () { int dummy; return begin_list (dummy); }

	int            read (float &x)
	{
		double         tmp;
		const int      ret_val = read (tmp);
		if (ret_val == 0)
		{
			x = float (tmp);
		}
		return ret_val;
	}
	template <typename T>
	int            read (T &x)
	{
		static_assert (
			(   std::is_integral <T>::value
			 || std::is_enum <T>::value    ),
			"Wrong type to read"
		);
		double         tmp;
		const int      ret_val = read (tmp);
		if (ret_val == 0)
		{
			x = T (fstb::round_int64 (tmp));
		}
		return ret_val;
	}


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_begin_list (int &nbr_elt) = 0;
	virtual int    do_end_list () = 0;
	virtual int    do_read (double &x) = 0;
	virtual int    do_read (std::string &s) = 0;



private:



}; // class SerRInterface



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/SerRInterface.hpp"



#endif   // mfx_doc_SerRInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
