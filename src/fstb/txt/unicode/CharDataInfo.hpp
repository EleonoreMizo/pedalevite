/*****************************************************************************

        CharDataInfo.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_unicode_CharDataInfo_CURRENT_CODEHEADER)
	#error Recursive inclusion of CharDataInfo code header.
#endif
#define	fstb_txt_unicode_CharDataInfo_CURRENT_CODEHEADER

#if ! defined (fstb_txt_unicode_CharDataInfo_CODEHEADER_INCLUDED)
#define	fstb_txt_unicode_CharDataInfo_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



unsigned long	CharDataInfo::compute_internal_sort_code1 () const
{
	const unsigned long	sort_code =
		  (static_cast <unsigned long> (_categ                    ) << 24)
		+ (static_cast <unsigned long> (_kangxi_radical_stroke_cnt) << 16)
		+ (static_cast <unsigned long> (_residual_stroke_cnt      ) <<  8)
		+                               _digit;

	return (sort_code);
}



unsigned long	CharDataInfo::compute_internal_sort_code2 () const
{
	const unsigned long	sort_code =
		  (static_cast <unsigned long> (_bidi_class         ) << 24)
		+ (static_cast <unsigned long> (_grapheme_break_prop) << 16)
		+ (static_cast <unsigned long> (_line_break_prop    ) <<  8)
		+                               _word_break_prop;

	return (sort_code);
}



bool	operator < (const CharDataInfo &lhs, const CharDataInfo &rhs)
{
	assert (&lhs != 0);
	assert (&rhs != 0);

	const unsigned long	sc1_lhs = lhs.compute_internal_sort_code1 ();
	const unsigned long	sc1_rhs = rhs.compute_internal_sort_code1 ();
	if (sc1_lhs < sc1_rhs)
	{
		return (true);
	}
	else if (sc1_lhs == sc1_rhs)
	{
		if (lhs._upper < rhs._upper)
		{
			return (true);
		}
		else if (lhs._upper == rhs._upper)
		{
			if (lhs._lower < rhs._lower)
			{
				return (true);
			}
			else if (lhs._lower == rhs._lower)
			{
				if (lhs._title < rhs._title)
				{
					return (true);
				}
				else if (lhs._title == rhs._title)
				{
					const unsigned long	sc2_lhs = lhs.compute_internal_sort_code2 ();
					const unsigned long	sc2_rhs = rhs.compute_internal_sort_code2 ();
					if (sc2_lhs < sc2_rhs)
					{
						return (true);
					}
				}
			}
		}
	}

	return (false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_unicode_CharDataInfo_CODEHEADER_INCLUDED

#undef fstb_txt_unicode_CharDataInfo_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
