/*****************************************************************************

        NodeEvt.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_NodeEvt_HEADER_INCLUDED)
#define mfx_uitk_NodeEvt_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/Button.h"

#include <array>



namespace mfx
{
namespace uitk
{



class NodeEvt
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_BUTTON = 0,
		Type_ROTENC,
		Type_CURSOR,
		Type_TIMER,

		Type_NBR_ELT
	};

	enum Curs
	{
		Curs_ENTER = 0,
		Curs_LEAVE,

		Curs_NBR_ELT
	};

	static NodeEvt create_button (int target, Button but);
	static NodeEvt create_rotenc (int target, int rotenc, int val);
	static NodeEvt create_cursor (int target, Curs curs);
	static NodeEvt create_timer (int target);

	               NodeEvt (const NodeEvt &other)    = default;
	               NodeEvt (NodeEvt &&other)         = default;

	virtual        ~NodeEvt () = default;

	NodeEvt &      operator = (const NodeEvt &other) = default;
	NodeEvt &      operator = (NodeEvt &&other)      = default;


	int            get_target () const;
	Type           get_type () const;

	Button         get_button () const;
	bool           is_button_ex () const;
	Button         get_button_ex () const;

	int            get_rotenc () const;
	int            get_rotenc_val () const;

	bool           is_cursor () const;
	Curs           get_cursor () const;

	bool           is_timer () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	explicit       NodeEvt (int target, Type type);

	int            _target_id;
	Type           _type;
	std::array <int, 2>
	               _val_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               NodeEvt ()                               = delete;
	bool           operator == (const NodeEvt &other) const = delete;
	bool           operator != (const NodeEvt &other) const = delete;

}; // class NodeEvt



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/NodeEvt.hpp"



#endif   // mfx_uitk_NodeEvt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
