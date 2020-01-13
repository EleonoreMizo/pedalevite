/*****************************************************************************

        SerWInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_SerWInterface_HEADER_INCLUDED)
#define mfx_doc_SerWInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



namespace mfx
{
namespace doc
{



class SerWInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SerWInterface ()                           = default;
	               SerWInterface (const SerWInterface &other) = default;
	               SerWInterface (SerWInterface &&other)      = default;
	virtual        ~SerWInterface ()                          = default;

	virtual SerWInterface &
	               operator = (const SerWInterface &other)    = default;
	virtual SerWInterface &
	               operator = (SerWInterface &&other)         = default;

	void           begin_list ();
	void           end_list ();
	void           write (float x);
	void           write (double x);
	void           write (std::string s);

	template <typename T>
	void           write (T x)
	{
		static_assert (
			   std::is_floating_point <T>::value
			|| std::is_integral <T>::value
			|| std::is_enum <T>::value,
			"Wrong type to write"
		);
		write (static_cast <double> (x));
	}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_begin_list () = 0;
	virtual void   do_end_list () = 0;
	virtual void   do_write (float x) = 0;
	virtual void   do_write (double x) = 0;
	virtual void   do_write (std::string s) = 0;



}; // class SerWInterface



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/SerWInterface.hpp"



#endif   // mfx_doc_SerWInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
