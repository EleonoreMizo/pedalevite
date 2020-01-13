/*****************************************************************************

        ActionTempo.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ActionTempo_HEADER_INCLUDED)
#define mfx_doc_ActionTempo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PedalActionSingleInterface.h"



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class ActionTempo final
:	public PedalActionSingleInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ActionTempo ()                         = default;
	               ActionTempo (SerRInterface &ser);
	               ActionTempo (const ActionTempo &other) = default;
	               ActionTempo (ActionTempo &&other)      = default;

	               ~ActionTempo ()                        = default;

	ActionTempo &  operator = (const ActionTempo &other)  = default;
	ActionTempo &  operator = (ActionTempo &&other)       = default;

	// PedalActionSingleInterface
	void           ser_write (SerWInterface &ser) const final;

	void           ser_read (SerRInterface &ser);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// PedalActionSingleInterface
	ActionType     do_get_type () const final;
	std::shared_ptr <PedalActionSingleInterface>
	               do_duplicate () const final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ActionTempo &other) const = delete;
	bool           operator != (const ActionTempo &other) const = delete;

}; // class ActionTempo



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ActionTempo.hpp"



#endif   // mfx_doc_ActionTempo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
