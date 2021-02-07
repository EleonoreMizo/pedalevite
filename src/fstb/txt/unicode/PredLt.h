/*****************************************************************************

        PredLt.h
        Author: Laurent de Soras, 2008

Template arguments:

- C: character converter, before comparison (useful for the case). Requires :
	static Char C::convert (Char c);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_PredLt_HEADER_INCLUDED)
#define	fstb_txt_unicode_PredLt_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace fstb
{
namespace txt
{
namespace unicode
{



class PredLt
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	inline bool    operator () (char32_t lhs, char32_t rhs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						PredLt ()                         = delete;
						PredLt (const PredLt &other)      = delete;
						PredLt (PredLt &&other)           = delete;
	PredLt &			operator = (const PredLt &other)  = delete;
	PredLt &			operator = (PredLt &&other)       = delete;
	bool				operator == (const PredLt &other) = delete;
	bool				operator != (const PredLt &other) = delete;

};	// class PredLt



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



#include	"fstb/txt/unicode/PredLt.hpp"



#endif	// fstb_txt_unicode_PredLt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
