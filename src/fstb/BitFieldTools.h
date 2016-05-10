/*****************************************************************************

        BitFieldTools.h
        Author: Laurent de Soras, 2006

Template parameters:

- T is the type of the variables containing groups of booleans.

- DL2 is log2 of the number of booleans per group.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_BitFieldTools_HEADER_INCLUDED)
#define fstb_BitFieldTools_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <climits>



namespace fstb
{



template <typename T, int DL2>
class BitFieldTools
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

   enum {         BITDEPTH_L2 = DL2 };
   enum {         BITDEPTH    = 1 << BITDEPTH_L2 };

   typedef  T  GroupType;

	static long		calculate_nbr_groups (long nbr_elt);
   static inline void
                  calculate_group_and_pos (long &group, int &gpos, long pos);
   static inline void
                  calculate_group_and_mask (long &group, GroupType &mask, long pos);

   static bool    get_bit (const GroupType bit_arr [], long pos);
   static void    set_bit (GroupType bit_arr [], long pos, bool flag);
   static void    clear_bit (GroupType bit_arr [], long pos);
   static void    fill_bit (GroupType bit_arr [], long pos);

   static void    activate_range (GroupType bit_arr [], long pos, long nbr_elt);
   static void    deactivate_range (GroupType bit_arr [], long pos, long nbr_elt);

   static long    get_next_bit_set_from (const GroupType bit_arr [], long pos, long length);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static_assert (BITDEPTH <= sizeof (GroupType) * CHAR_BIT, "BITDEPTH");



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ~BitFieldTools ()                              = delete;
	               BitFieldTools ()                               = delete;
	               BitFieldTools (const BitFieldTools &other)     = delete;
	BitFieldTools &
	               operator = (const BitFieldTools &other)        = delete;
	bool           operator == (const BitFieldTools &other) const = delete;
	bool           operator != (const BitFieldTools &other) const = delete;

}; // class BitFieldTools



}  // namespace fstb



#include "fstb/BitFieldTools.hpp"



#endif   // fstb_BitFieldTools_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
