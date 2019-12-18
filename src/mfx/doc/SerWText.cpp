/*****************************************************************************

        SerWText.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/neutral/ConvDigit.h"
#include "fstb/fnc.h"
#include "mfx/doc/SerWText.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SerWText::clear ()
{
	_content.clear ();
	_err_msg.clear ();
	_err_flag       = false;
	_nbr_elt_stack.clear ();
	_nbr_elt_line   = 0;
	_list_term_flag = false;
}



int		SerWText::terminate ()
{
	if (! _err_flag && ! _nbr_elt_stack.empty ())
	{
		_err_flag = true;
		_err_msg  = "List(s) not terminated";
	}

	return (_err_flag) ? -1 : 0;
}



bool	SerWText::has_error () const
{
	return _err_flag;
}



std::string	SerWText::get_error_msg () const
{
	return _err_msg;
}



const std::string &	SerWText::use_content ()
{
	return _content;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SerWText::do_begin_list ()
{
	if (! _err_flag && int (_nbr_elt_stack.size ()) >= _max_list_level)
	{
		_err_flag = true;
		_err_msg  = "Lists not terminated, exceeded level limit";
	}

	if (! _err_flag)
	{
		if (_nbr_elt_line > 0 || _list_term_flag)
		{
			_content += "\n";
			add_indent ();
		}
		else if (! _nbr_elt_stack.empty ())
		{
			_content += "\t";
		}
		_content += "{";

		_nbr_elt_stack.push_back (0);
		_nbr_elt_line = 0;
		_list_term_flag = false;
	}
}



void	SerWText::do_end_list ()
{
	if (! _err_flag && _nbr_elt_stack.empty ())
	{
		_err_flag = true;
		_err_msg  = "Unexpected end of list";
	}

	if (! _err_flag)
	{
		_nbr_elt_stack.pop_back ();

		_content += " }";

		_nbr_elt_line = 0;
		_list_term_flag = true;
	}
}



void	SerWText::do_write (float x)
{
	if (! _err_flag)
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "%g", x);
		add_base_elt (txt_0);
	}
}



void	SerWText::do_write (double x)
{
	if (! _err_flag)
	{
		char           txt_0 [127+1];
		fstb::snprintf4all (txt_0, sizeof (txt_0), "%.16g", x);
		add_base_elt (txt_0);
	}
}



void	SerWText::do_write (std::string s)
{
	if (! _err_flag)
	{
		s = "\"" + escape_str (s) + "\"";
		add_base_elt (s);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	SerWText::escape_str (std::string s)
{
	std::string    esc;
	size_t         pos_prev = 0;
	size_t         pos      = 0;
	const size_t   len      = s.length ();

	while (pos < len)
	{
		const int      c = static_cast <unsigned char> (s [pos]);
		if (c < 32 || c >= 127 || c == '\\' || c == '\"')
		{
			if (pos_prev < pos)
			{
				esc += s.substr (pos_prev, pos - pos_prev);
			}

			if (c == '\\' || c == '\"')
			{
				esc += "\\";
				esc += char (c);
			}
			else
			{
				esc += "\\x";
				esc += fstb::txt::neutral::ConvDigit::conv_digit_to_char (c >> 4);
				esc += fstb::txt::neutral::ConvDigit::conv_digit_to_char (c & 15);
			}

			pos_prev = pos + 1;
		}

		++ pos;
	}

	if (pos_prev < len)
	{
		esc += s.substr (pos_prev);
	}

	return esc;
}



void	SerWText::add_base_elt (std::string s)
{
	bool           lf_flag = (_nbr_elt_line >= 8);
	_nbr_elt_line &= 7;
	if (! _nbr_elt_stack.empty ())
	{
		++ _nbr_elt_stack.back ();
	}

	_content += (lf_flag) ? "\n" : " ";
	_content += s;

	++ _nbr_elt_line;
	_list_term_flag = false;
}



void	SerWText::add_indent ()
{
	const int      indent = int (_nbr_elt_stack.size ());
	for (int i = 0; i < indent; ++i)
	{
		_content += "\t";
	}
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
