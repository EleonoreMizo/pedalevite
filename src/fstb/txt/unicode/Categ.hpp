/*****************************************************************************

        Categ.hpp
        Author: Laurent de Soras, 2008

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (fstb_txt_unicode_Categ_CURRENT_CODEHEADER)
	#error Recursive inclusion of Categ code header.
#endif
#define	fstb_txt_unicode_Categ_CURRENT_CODEHEADER

#if ! defined (fstb_txt_unicode_Categ_CODEHEADER_INCLUDED)
#define	fstb_txt_unicode_Categ_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CategClass	Categ_conv_to_class (Categ categ)
{
	assert (categ >= 0);

	const CategClass	cc = static_cast <CategClass> (categ >> Categ_BITS);
	assert (cc >= 0);
	assert (cc < CategClass_NBR_ELT);

	return (cc);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#endif	// fstb_txt_unicode_Categ_CODEHEADER_INCLUDED

#undef fstb_txt_unicode_Categ_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
