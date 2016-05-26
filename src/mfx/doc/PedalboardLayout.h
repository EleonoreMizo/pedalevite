/*****************************************************************************

        PedalboardLayout.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PedalboardLayout_HEADER_INCLUDED)
#define mfx_doc_PedalboardLayout_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PedalActionGroup.h"
#include "mfx/Cst.h"

#include <array>



namespace mfx
{
namespace doc
{



class PedalboardLayout
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PedalboardLayout ()                              = default;
	               PedalboardLayout (const PedalboardLayout &other) = default;
	virtual        ~PedalboardLayout ()                             = default;

	PedalboardLayout &
	               operator = (const PedalboardLayout &other)       = default;

	void           merge_layout (const PedalboardLayout &other);

	std::array <PedalActionGroup, Cst::_nbr_pedals>
	               _pedal_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PedalboardLayout &other) const = delete;
	bool           operator != (const PedalboardLayout &other) const = delete;

}; // class PedalboardLayout



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PedalboardLayout.hpp"



#endif   // mfx_doc_PedalboardLayout_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
