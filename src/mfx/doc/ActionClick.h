/*****************************************************************************

        ActionClick.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_ActionClick_HEADER_INCLUDED)
#define mfx_doc_ActionClick_HEADER_INCLUDED

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

class ActionClick
:	public PedalActionSingleInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Mode
	{
		Mode_OFF = 0,
		Mode_ON,
		Mode_TOGGLE,

		Mode_NBR_ELT
	};

	explicit       ActionClick (Mode mode, bool bar_flag);
	explicit       ActionClick (SerRInterface &ser);
	               ActionClick (const ActionClick &other) = default;
	virtual        ~ActionClick ()                        = default;

	ActionClick &  operator = (const ActionClick &other)  = default;

	virtual void   ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	Mode           _mode;
	bool           _bar_flag;     // One bar only



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

	               ActionClick ()                               = delete;
	bool           operator == (const ActionClick &other) const = delete;
	bool           operator != (const ActionClick &other) const = delete;

}; // class ActionClick



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/ActionClick.hpp"



#endif   // mfx_doc_ActionClick_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
