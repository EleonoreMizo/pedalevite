/*****************************************************************************

        PredLt.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_unicode_PredLt_CURRENT_CODEHEADER)
	#error Recursive inclusion of PredLt code header.
#endif
#define	fstb_txt_unicode_PredLt_CURRENT_CODEHEADER

#if ! defined (fstb_txt_unicode_PredLt_CODEHEADER_INCLUDED)
#define	fstb_txt_unicode_PredLt_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class C>
bool	PredLt <C>::operator () (char32_t lhs, char32_t rhs) const
{
	lhs = C::convert (lhs);
	rhs = C::convert (rhs);

	return (lhs < rhs);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_unicode_PredLt_CODEHEADER_INCLUDED

#undef fstb_txt_unicode_PredLt_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
