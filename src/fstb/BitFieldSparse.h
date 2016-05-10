/*****************************************************************************

        BitFieldSparse.h
        Author: Laurent de Soras, 2006

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_BitFieldSparse_HEADER_INCLUDED)
#define fstb_BitFieldSparse_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>

#include <cstdint>



namespace fstb
{



class BitFieldSparse
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:


	               BitFieldSparse ();
	explicit       BitFieldSparse (long nbr_elt);
	               BitFieldSparse (const BitFieldSparse &other) = default;
	virtual        ~BitFieldSparse ()                           = default;

	BitFieldSparse &
	               operator = (const BitFieldSparse &other)     = default;

	void           set_nbr_elt (long nbr_elt);
	long           get_nbr_elt () const;
	void           clear ();
	void           fill ();

   bool           get_bit (long pos) const;
   void           set_bit (long pos, bool flag);
   void           clear_bit (long pos);
   void           fill_bit (long pos);

   void           activate_range (long pos, long nbr_elt);
   void           deactivate_range (long pos, long nbr_elt);

   long           get_next_bit_set_from (long pos, long stop_pos = -1) const;
	bool           has_a_bit_set () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

   enum {         BITDEPTH_L2 = 5 };	// 32 bits
   enum {         BITDEPTH = 1 << BITDEPTH_L2 };

	typedef	unsigned long	GroupType;

	class BfLevel
	{
	public:
		typedef	std::vector <GroupType>	GroupArray;
		GroupArray		_group_arr;
		long				_nbr_elt;
	};

	typedef	std::vector <BfLevel>	LevelArray;

	long				get_next_bit_set_from_rec (int lvl_index, long start, long stop) const;

	LevelArray		_lvl_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const BitFieldSparse &other) const = delete;
	bool           operator != (const BitFieldSparse &other) const = delete;

}; // class BitFieldSparse



}  // namespace fstb



//#include "fstb/BitFieldSparse.hpp"



#endif   // fstb_BitFieldSparse_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
