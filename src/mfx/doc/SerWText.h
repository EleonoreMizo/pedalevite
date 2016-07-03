/*****************************************************************************

        SerWText.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_SerWText_HEADER_INCLUDED)
#define mfx_doc_SerWText_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/SerWInterface.h"

#include <vector>



namespace mfx
{
namespace doc
{



class SerWText
:	public SerWInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int
	               _max_list_level = 100;

	               SerWText ()  = default;
	virtual        ~SerWText () = default;

	void           clear ();
	int            terminate ();

	bool           has_error () const;
	std::string    get_error_msg () const;
	const std::string &
	               use_content ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// doc::mfx::SerWInterface
	virtual void   do_begin_list ();
	virtual void   do_end_list ();
	virtual void   do_write (double x);
	virtual void   do_write (std::string s);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	std::string    escape_str (std::string s);
	void           add_base_elt (std::string s);
	void           add_indent ();

	std::string    _content;
	std::string    _err_msg;
	bool           _err_flag = false;

	std::vector <int>                   // Current count of elements for the lists.
	               _nbr_elt_stack;
	int            _nbr_elt_line = 0;   // Number of base elements on the current line
	bool           _list_term_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SerWText (const SerWText &other)          = delete;
	SerWText &     operator = (const SerWText &other)        = delete;
	bool           operator == (const SerWText &other) const = delete;
	bool           operator != (const SerWText &other) const = delete;

}; // class SerWText



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/SerWText.hpp"



#endif   // mfx_doc_SerWText_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
