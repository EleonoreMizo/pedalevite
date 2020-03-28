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

class ActionToggleFx final
:	public PedalActionSingleInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ActionToggleFx (const FxId &fx_id);
	explicit       ActionToggleFx (SerRInterface &ser);
	               ActionToggleFx (const ActionToggleFx &other) = default;
	               ActionToggleFx (ActionToggleFx &&other)      = default;

	               ~ActionToggleFx ()                           = default;

	ActionToggleFx &
	               operator = (const ActionToggleFx &other)     = default;
	ActionToggleFx &
	               operator = (ActionToggleFx &&other)          = default;

	// PedalActionSingleInterface
	void           ser_write (SerWInterface &ser) const final;

	void           ser_read (SerRInterface &ser);

	FxId           _fx_id; // _type is ignored, it is always the main plug-in.



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

	               ActionToggleFx ()                               = delete;
	bool           operator == (const ActionToggleFx &other) const = delete;
	bool           operator != (const ActionToggleFx &other) const = delete;

}; // class ActionToggleFx



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ActionToggleFx.hpp"



#endif   // mfx_doc_ActionToggleFx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
