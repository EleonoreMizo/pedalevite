/*****************************************************************************

        ActionToggleFx.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ActionToggleFx_HEADER_INCLUDED)
#define mfx_doc_ActionToggleFx_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/FxId.h"
#include "mfx/doc/PedalActionSingleInterface.h"



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class ActionToggleFx
:	public PedalActionSingleInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ActionToggleFx (const ActionToggleFx &other) = default;
	virtual        ~ActionToggleFx ()                           = default;

	ActionToggleFx &
	               operator = (const ActionToggleFx &other)     = default;

	virtual void   ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	FxId           _fx_id;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PedalActionSingleInterface
	virtual ActionType
	               do_get_type () const;
	virtual PedalActionSingleInterface *
	               do_duplicate () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ActionToggleFx ()                               = delete;
	bool           operator == (const ActionToggleFx &other) const = delete;
	bool           operator != (const ActionToggleFx &other) const = delete;

}; // class ActionToggleFx



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ActionToggleFx.hpp"



#endif   // mfx_doc_ActionToggleFx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
