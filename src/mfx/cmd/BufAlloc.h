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
	int            alloc (int use_count = 1);
	void           use_more (int buf, int use_count = 1);
	void           use_more_if_std (int buf, int use_count = 1);
	void           ret (int buf, int use_count = 1);
	void           ret_if_std (int buf, int use_count = 1);
	int            get_use_count (int buf) const;
	int            get_nbr_alloc_buf () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class BufInfo
	{
	public:
		int            _buf_list_pos = -1; // In _buf_list. -1 = reserved
		int            _use_count    = 0;  // Reference counting
	};

	int            find_buf_pos (int buf) const;

	// First part contains the reserved and used buffers,
	// second part contains the free buffers
	std::array <int, Cst::_max_nbr_buf>
	               _buf_list;
	const int      _reserved_size; // These buffers are allocated forever and cannot be freed
	int            _free_index;    // >= _reserved_size
	std::array <BufInfo, Cst::_max_nbr_buf>
	               _buf_info_list; // Data below _reserved_size is not valid



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
