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

   static constexpr int BITDEPTH_L2 = DL2;
   static constexpr int BITDEPTH    = 1 << BITDEPTH_L2;

   typedef  T  GroupType;

	static_assert (BITDEPTH <= sizeof (GroupType) * CHAR_BIT, "BITDEPTH");

	static int     calculate_nbr_groups (int nbr_elt) noexcept;
   static inline void
                  calculate_group_and_pos (int &group, int &gpos, int pos) noexcept;
   static inline void
                  calculate_group_and_mask (int &group, GroupType &mask, int pos) noexcept;

   static bool    get_bit (const GroupType bit_arr [], int pos) noexcept;
   static void    set_bit (GroupType bit_arr [], int pos, bool flag) noexcept;
   static void    clear_bit (GroupType bit_arr [], int pos) noexcept;
   static void    fill_bit (GroupType bit_arr [], int pos) noexcept;

   static void    activate_range (GroupType bit_arr [], int pos, int nbr_elt) noexcept;
   static void    deactivate_range (GroupType bit_arr [], int pos, int nbr_elt) noexcept;

   static int     get_next_bit_set_from (const GroupType bit_arr [], int pos, int length) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



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
