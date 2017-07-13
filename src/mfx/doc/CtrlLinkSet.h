/*****************************************************************************

        CtrlLinkSet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_CtrlLinkSet_HEADER_INCLUDED)
#define mfx_doc_CtrlLinkSet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CtrlLink.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class CtrlLinkSet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <CtrlLink> LinkSPtr;
	typedef std::vector <LinkSPtr> ModArray;

	               CtrlLinkSet ()  = default;
	               CtrlLinkSet (const CtrlLinkSet &other);
	virtual        ~CtrlLinkSet () = default;
	CtrlLinkSet &  operator = (const CtrlLinkSet &other);

	bool           is_empty () const;
	bool           is_similar (const CtrlLinkSet &other) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	LinkSPtr       _bind_sptr;
	ModArray       _mod_arr;   // Null pointers not allowed.



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           duplicate_children ();



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CtrlLinkSet &other) const = delete;
	bool           operator != (const CtrlLinkSet &other) const = delete;

}; // class CtrlLinkSet



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/CtrlLinkSet.hpp"



#endif   // mfx_doc_CtrlLinkSet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
