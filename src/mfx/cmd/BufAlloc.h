/*****************************************************************************

        BufAlloc.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_BufAlloc_HEADER_INCLUDED)
#define mfx_cmd_BufAlloc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/Cst.h"

#include <array>



namespace mfx
{
namespace cmd
{



class BufAlloc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               BufAlloc (int reserved_size = 0);
	virtual        ~BufAlloc () = default;

	void           clear ();

	bool           has_room () const;
	int            alloc ();
	void           ret (int buf);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            find_buf_pos (int buf) const;

	// First part contains the used buffers,
	// second part contains the free buffers
	std::array <int, Cst::_max_nbr_buf>
	               _buf_list;
	const int      _reserved_size;
	int            _free_index;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BufAlloc (const BufAlloc &other)          = delete;
	BufAlloc &     operator = (const BufAlloc &other)        = delete;
	bool           operator == (const BufAlloc &other) const = delete;
	bool           operator != (const BufAlloc &other) const = delete;

}; // class BufAlloc



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/BufAlloc.hpp"



#endif   // mfx_cmd_BufAlloc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
