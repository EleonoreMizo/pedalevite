/*****************************************************************************

        Msg.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_Msg_HEADER_INCLUDED)
#define mfx_Msg_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



class ProcessingContext;

class Msg
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Sender
	{
		Sender_INVALID = -1,

		Sender_CMD = 0,
		Sender_AUDIO,

		Sender_NBR_ELT
	};

	enum Type
	{
		Type_INVALID = -1,

		Type_CTX = 0,
		Type_PARAM,

		Type_NBR_ELT
	};

	class Ctx
	{
	public:
		const ProcessingContext *
		               _ctx_ptr;
	};

	class Param
	{
	public:
		int            _plugin_id;
		int            _index;
		float          _val;
	};

	union Content
	{
		Ctx            _ctx;
		Param          _param;
	};

	               Msg ()                        = default;
	               Msg (const Msg &other)        = default;
	virtual        ~Msg ()                       = default;
	Msg &          operator = (const Msg &other) = default;

	Sender         _sender = Sender_INVALID;  // Original sender. To know if a message should be recycled.
	Type           _type   = Type_INVALID;
	Content        _content;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Msg &other) const = delete;
	bool           operator != (const Msg &other) const = delete;

}; // class Msg



}  // namespace mfx



//#include "mfx/Msg.hpp"



#endif   // mfx_Msg_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
