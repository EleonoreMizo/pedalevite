/*****************************************************************************

        SignalPort.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_SignalPort_HEADER_INCLUDED)
#define mfx_doc_SignalPort_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class SignalPort
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SignalPort ()                        = default;
	               ~SignalPort ()                       = default;
	               SignalPort (const SignalPort &other) = default;
	               SignalPort (SignalPort &&other)      = default;

	SignalPort &   operator = (const SignalPort &other) = default;
	SignalPort &   operator = (SignalPort &&other)      = default;

	bool           operator == (const SignalPort &other) const;
	bool           operator != (const SignalPort &other) const;
	bool           operator < (const SignalPort &other) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	int            _slot_id   = -1;
	int            _sig_index = -1;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class SignalPort



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/SignalPort.hpp"



#endif   // mfx_doc_SignalPort_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
